/* File: VMS_SELECT_HACK.C */
/* Hack to make select() appear to work on pipes and terminals */

#define __NEW_STARLET 1

#include <builtins.h>
#include <dcdef.h>
#include <descrip.h>
#include <dvidef.h>
#include <efndef.h>
#include <errno.h>
#include <iledef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <poll.h>
#include <siginfo.h>
#include <socket.h>
#include <ssdef.h>
#include <starlet.h>
#include <string.h>
#include <stsdef.h>
#include <time.h>
#include <unistd.h>
#include <unixio.h>
#include <unixlib.h>

#include "vms/vms_spawn_helper.h"
#include "vms/vms_select.h"
#include "vms/vms_sleep.h"

int vms_channel_lookup_by_name(char* name, unsigned short *channel) {
    int status;
    struct dsc$descriptor_s dev_desc;
    int call_stat;
    unsigned short chan;

    status = -1;

    if (name != NULL) {
        /* Assign the channel */
        /*--------------------*/
        dev_desc.dsc$a_pointer = name;
        dev_desc.dsc$w_length = strlen(name);
        dev_desc.dsc$b_dtype = DSC$K_DTYPE_T;
        dev_desc.dsc$b_class = DSC$K_CLASS_S;
        call_stat = SYS$ASSIGN(&dev_desc, &chan, 0, 0, 0);
        if ($VMS_STATUS_SUCCESS(call_stat)) {
            *channel = chan;
            status = 0;
        }
    }
    return status;
}

int vms_channel_lookup(int fd, unsigned short *channel) {
    char devicename[256];
    return vms_channel_lookup_by_name(getname(fd, devicename, 1), channel);
}

int vms_channel_free(unsigned short channel) {
    return sys$dassgn(channel);
}

struct vms_pollfd_st {
    struct pollfd *fd_desc_ptr;
    unsigned short channel;
    unsigned short pad;
};

static int select_terminal(const struct vms_pollfd_st *term_array, int ti) {
    int ret_stat = 0;
    IOSB iosb;
    union typeahead_st {
        unsigned short numchars;
        unsigned long  dummy[2];
    } typeahead;

    /* Loop through the terminal channels */
    for (int i = 0; i < ti; i++) {
        /* assume output is always available */
        term_array[i].fd_desc_ptr->revents = term_array[i].fd_desc_ptr->events & POLL_OUT;
        /* Poll input status */
        if (term_array[i].fd_desc_ptr->events & POLL_IN) {
            int status = SYS$QIOW(EFN$C_ENF, term_array[i].channel,
                        IO$_SENSEMODE | IO$M_TYPEAHDCNT, &iosb, NULL, 0,
                        &typeahead, sizeof(typeahead), 0, 0, 0, 0);
            if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
                if (typeahead.numchars != 0) {
                    term_array[i].fd_desc_ptr->revents = term_array[i].fd_desc_ptr->events & POLL_IN;
                }
            } else {
                /* Something really wrong */
                ret_stat = -1;
                errno = EIO;
                break;
            }
        }
        /* Increment the return status */
        if (term_array[i].fd_desc_ptr->revents != 0) {
            ret_stat++;
        }
    }
    return ret_stat;
}

static int select_mbx(const struct vms_pollfd_st *pipe_array, int pi) {
    int ret_stat = 0;
    IOSB iosb;

    /* Loop through the pipes */
    for (int i = 0; i < pi; i++) {
        pipe_array[i].fd_desc_ptr->revents = 0;
        /* Check the mailbox status */
        if (pipe_array[i].fd_desc_ptr->events & (POLL_IN | POLL_OUT)) {
            int status = SYS$QIOW(EFN$C_ENF, pipe_array[i].channel, IO$_SENSEMODE,
                &iosb, NULL, 0, 0, 0, 0, 0, 0, 0);
            if ($VMS_STATUS_SUCCESS(status) && $VMS_STATUS_SUCCESS(iosb.iosb$w_status)) {
                /* Got some information */
                if (iosb.iosb$w_bcnt != 0) {
                    /* There is data to read */
                    pipe_array[i].fd_desc_ptr->revents = pipe_array[i].fd_desc_ptr->events & POLL_IN;
                } else {
                    /* Pipe is empty, ok to write */
                    pipe_array[i].fd_desc_ptr->revents = pipe_array[i].fd_desc_ptr->events & POLL_OUT;
                }
            } else {
                /* Something really wrong */
                ret_stat = -1;
                errno = EIO;
                break;
            }
        }
        /* Increment the return status */
        if (pipe_array[i].fd_desc_ptr->revents != 0) {
            ret_stat++;
        }
    }
    return ret_stat;
}

static int select_x11_efn(const struct vms_pollfd_st *efn_array, int xi) {
    int ret_stat = 0;
    unsigned int state;

    /* Loop through the event flags */
    for (int i = 0; i < xi; i++) {
        /* assume output is always available */
        efn_array[i].fd_desc_ptr->revents = efn_array[i].fd_desc_ptr->events & POLL_OUT;

        /* Check the mailbox status */
        if (efn_array[i].fd_desc_ptr->events & (POLL_IN | POLL_OUT)) {
            int status = SYS$READEF(efn_array[i].fd_desc_ptr->fd, &state);
            if ($VMS_STATUS_SUCCESS(status)) {
                /* Got some information */
                if (status == SS$_WASSET) {
                    /* There is data to read */
                    efn_array[i].fd_desc_ptr->revents = efn_array[i].fd_desc_ptr->events & POLL_IN;
                }
            } else {
                /* Something really wrong */
                ret_stat = -1;
                errno = EIO;
                break;
            }
        }
        /* Increment the return status */
        if (efn_array[i].fd_desc_ptr->revents != 0) {
            ret_stat++;
        }
    }
    return ret_stat;
}

/*******************************************************************
 OpenVMS Open Source Community select hack...

 this code acts as a wrapper for poll() and supports:
        pipes
        mailboxes
        terminals
        sockets

 *******************************************************************/

int vms_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                 struct timeval *timeout) {
    int ret_stat;
    int siif_index;
    int old_siif_value;

    const char *select_ignores_invalid_fd = "DECC$SELECT_IGNORES_INVALID_FD";

    /* Get old ignore setting and enable new */
    /* threaded applications need this setting enabled before this routine */
    siif_index = decc$feature_get_index(select_ignores_invalid_fd);
    if (siif_index >= 0) {
        old_siif_value = decc$feature_set_value(siif_index, 1, 1);
    }

    if (nfds != 0) {
        struct pollfd *select_array;
        struct vms_pollfd_st *term_array;
        struct vms_pollfd_st *pipe_array;
        struct vms_pollfd_st *xefn_array;
        int ti;
        int si;
        int pi;
        int xi;
        int status;

        /* Need structures to separate terminals and pipes */
        select_array = __ALLOCA(sizeof(struct pollfd) * nfds);
        if (select_array == NULL) {
            return -1;
        }
        term_array = __ALLOCA(sizeof(struct vms_pollfd_st) * nfds);
        if (term_array == NULL) {
            return -1;
        }
        pipe_array = __ALLOCA(sizeof(struct vms_pollfd_st) * nfds);
        if (pipe_array == NULL) {
            return -1;
        }
        xefn_array = __ALLOCA(sizeof(struct vms_pollfd_st) * nfds);
        if (xefn_array == NULL) {
            return -1;
        }

        /* Find all of the terminals and pipe fds  for polling */
        pi = 0, ti = 0, xi = 0, si = 0;
        for (int fd = 0; fd < nfds; ++fd) {
            /* Copy file descriptor arrays into a poll structure */
            select_array[fd].fd = fd;
            select_array[fd].events = 0;
            select_array[fd].revents = 0;

            /* Now separate out the pipes */
            if (isapipe(fd) == 1) {
                pipe_array[pi].fd_desc_ptr = &select_array[fd];
                if (readfds != NULL) {
                    if (FD_ISSET(fd, readfds)) {
                        select_array[fd].events |= POLL_IN;
                    }
                }
                if (writefds != NULL) {
                    if (FD_ISSET(fd, writefds)) {
                        select_array[fd].events |= POLL_OUT;
                    }
                }
                /* Only care about something with read/write events */
                if (select_array[fd].events != 0) {
                    status = vms_channel_lookup(fd, &pipe_array[pi].channel);
                    if (status == 0) {
                        pi++;
                    }
                }
            }
            /* Not a pipe, see if a terminal */
            else if (isatty(fd) == 1) {
                term_array[ti].fd_desc_ptr = &select_array[fd];
                if (readfds != NULL) {
                    if (FD_ISSET(fd, readfds)) {
                        select_array[fd].events |= POLL_IN;
                    }
                }
                if (writefds != NULL) {
                    if (FD_ISSET(fd, writefds)) {
                        select_array[fd].events |= POLL_OUT;
                    }
                }
                /* Only care about something with read/write events */
                if (select_array[fd].events != 0) {
                    status = vms_channel_lookup(fd, &term_array[ti].channel);
                    if (status == 0) {
                        ti++;
                    }
                }
            } else if (decc$get_sdc(fd) != 0) {
                /* Not pipe or terminal, use built in select on this */
                si++;
            } else {
                /* see if it is a mailbox and then treat as a pipe or X11 event if not
                * mailbox */
                pipe_array[pi].fd_desc_ptr = &select_array[fd];
                if (readfds != NULL) {
                    if (FD_ISSET(fd, readfds)) {
                        select_array[fd].events |= POLL_IN;
                    }
                }
                if (writefds != NULL) {
                    if (FD_ISSET(fd, writefds)) {
                        select_array[fd].events |= POLL_OUT;
                    }
                }
                /* Only care about something with read/write events */
                if (select_array[fd].events != 0) {
                    status = vms_channel_lookup(fd, &pipe_array[pi].channel);
                    if (status == 0) {
                        int mbx_len;
                        unsigned int mbx_char;
                        ILE3 item_list[2];
                        item_list[0].ile3$w_length = 4;
                        item_list[0].ile3$w_code = DVI$_DEVCLASS;
                        item_list[0].ile3$ps_bufaddr = (void *)&mbx_char;
                        item_list[0].ile3$ps_retlen_addr = (void *)&mbx_len;
                        memset(item_list + 1, 0, sizeof(item_list[1]));

                        status = SYS$GETDVIW(EFN$C_ENF, pipe_array[pi].channel, 0, &item_list, 0, 0, 0, 0);
                        if ($VMS_STATUS_SUCCESS(status)) {
                            if ((mbx_char & DC$_MAILBOX) != 0) {
                                pi++;
                            } else {
                                xi++;
                            }
                        } else {
                            select_array[fd].events = 0;
                            SYS$DASSGN(pipe_array[pi].channel);
                        }
                    } else {
                        select_array[fd].events = 0;
                    }
                }
            }
        }
        /* All fd is arranged ============================================*/
        if ((pi == 0) && (ti == 0) && (xi == 0)) {
            /* All sockets, let select do everything */
            ret_stat = select(nfds, readfds, writefds, exceptfds, timeout);
        } else {
            int utimeleft;    /* Microseconds left */
            int ti_stat;
            int pi_stat;
            int si_stat;
            int xi_stat;
            struct timespec end_time;

            ti_stat = 0;
            pi_stat = 0;
            si_stat = 0;
            xi_stat = 0;
            end_time.tv_sec = 0;
            end_time.tv_nsec = 0;
            #define FULL_NSEC 1000000000
            #define FULL_USEC 1000000
            utimeleft = FULL_USEC;
            if (timeout != NULL) {
                if (clock_gettime(CLOCK_REALTIME, &end_time)) {
                    end_time.tv_sec = 0;
                    end_time.tv_nsec = 0;
                } else {
                    end_time.tv_sec += timeout->tv_sec;
                    end_time.tv_nsec += timeout->tv_usec * (FULL_NSEC / FULL_USEC);
                    while (end_time.tv_nsec >= FULL_NSEC) {
                        ++end_time.tv_sec;
                        end_time.tv_nsec -= FULL_NSEC;
                    }
                }
            }
            ret_stat = 0;

            /* Terminals and or pipes (and or MBXs) and or sockets  */
            /* Now we have to periodically poll everything with timeout */
            while (ret_stat == 0) {
                int sleeptime;  // microseconds 1/1000000
                if (ti != 0) {
                    ti_stat = select_terminal(term_array, ti);
                }
                if (pi != 0) {
                    pi_stat = select_mbx(pipe_array, pi);
                }
                if (xi != 0) {
                    xi_stat = select_x11_efn(xefn_array, xi);
                }
                if (ti_stat != 0 || pi_stat != 0 || xi_stat != 0) {
                    sleeptime = 0;
                } else {
                    sleeptime = 100;
                    if (utimeleft < sleeptime) {
                        sleeptime = utimeleft;
                    }
                }
                if (si == 0) {
                    /* sleep for shorter of 100 microseconds or timeout and retry */
                    if (sleeptime > 0) {
                        vms_sleep(sleeptime);
                    }
                } else {
                    /* let select consume the time*/
                    struct timeval sleep_timeout;
                    sleep_timeout.tv_sec = 0;
                    sleep_timeout.tv_usec = sleeptime;
                    si_stat = select(nfds, readfds, writefds, exceptfds, &sleep_timeout);
                }
                /* one last poll of terminals and pipes */
                if ((sleeptime > 0) || (si_stat > 0)) {
                    if ((ti != 0) && (ti_stat == 0)) {
                        ti_stat = select_terminal(term_array, ti);
                    }
                    if ((pi != 0) && (pi_stat == 0)) {
                        pi_stat = select_mbx(pipe_array, pi);
                    }
                    if ((xi != 0) && (xi_stat == 0)) {
                        xi_stat = select_x11_efn(xefn_array, xi);
                    }
                }

                /* Gather up any results */
                if ((ti_stat == -1) || (pi_stat == -1) || (si_stat == -1) || (xi_stat == -1)) {
                    ret_stat = -1;
                } else {
                    ret_stat = ti_stat + pi_stat + si_stat + xi_stat;
                }

                /* Timed out? */
                if (timeout != NULL) {
                    struct timespec cur_time;
                    if (clock_gettime(CLOCK_REALTIME, &cur_time)) {
                        // failed to obtain current time
                        break;
                    } else {
                        if (cur_time.tv_sec > end_time.tv_sec ||
                            cur_time.tv_sec == end_time.tv_sec && cur_time.tv_nsec >= end_time.tv_nsec) {
                            // time out
                            break;
                        } else {
                            // calculate utimeleft
                            if (end_time.tv_sec - cur_time.tv_sec <= 1) {
                                utimeleft = (end_time.tv_sec - cur_time.tv_sec) * FULL_USEC;
                                utimeleft += (end_time.tv_nsec - cur_time.tv_nsec) / (FULL_NSEC / FULL_USEC);
                                if (utimeleft < 0 || utimeleft > FULL_USEC) {
                                    // something wrong
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        /* Copy the pipe and terminal information */
        for (int fd = 0; fd < nfds; ++fd) {
            if (select_array[fd].events != 0) {
                if (readfds != NULL) {
                    if (select_array[fd].revents & POLL_IN) {
                        FD_SET(fd, readfds);
                    } else {
                        FD_CLR(fd, readfds);
                    }
                }
                if (writefds != NULL) {
                    if (select_array[fd].revents & POLL_OUT) {
                        FD_SET(fd, writefds);
                    } else {
                        FD_CLR(fd, writefds);
                    }
                }
            }
        }
        /* clean */
        while (pi > 0) {
            --pi;
            SYS$DASSGN(pipe_array[pi].channel);
        }
        while (ti > 0) {
            --ti;
            SYS$DASSGN(term_array[ti].channel);
        }
    } else {
        /* when nfds == 0 */
        ret_stat = select(nfds, readfds, writefds, exceptfds, timeout);
    }

    /* Restore old setting */
    if (siif_index >= 0) {
        decc$feature_set_value(siif_index, 1, old_siif_value);
    }

    return ret_stat;
}
