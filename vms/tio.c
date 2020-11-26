#ifndef __USE_BSD
#define __USE_BSD 1
#endif
#ifndef __USE_GNU
#define __USE_GNU 1
#endif
#ifndef __USE_XOPEN
#define __USE_XOPEN 1
#endif
#ifndef __USE_MISC
#define __USE_MISC 1
#endif

#include <errno.h>
#include <stdlib.h>
#include <stsdef.h>
#include <descrip.h>
#include <efndef.h>
#include <string.h>
#include <iodef.h>
#include <unixlib.h>
#include <unixio.h>
#include <unistd.h>
#include <ssdef.h>
#include <libclidef.h>

#define __NEW_STARLET 1
#include <ttdef.h>
#include <tt3def.h>
#include <tt2def.h>

#include "termios.h"

/* General structure of these routines is that they will check the file
 * descriptor to see if it is a tty and what its name is, and then use a
 * VMS channel for it to do the QIOW, and a structure of UNIX attributes
 * to actually do the I/O.  The first access of a tty file descriptor will
 * cause the VMS channel to be created and the structure of UNIX attributes
 * to be populated.
 */


#pragma member_alignment save
#pragma nomember_alignment longword
#pragma message save
#pragma message disable misalgndmem

/* Generic VMS item list structure */
struct itmlst_3 {
  unsigned short int buflen;
  unsigned short int itmcode;
  void *bufadr;
  unsigned short int *retlen;
};

struct tt_framing_st {
    unsigned tt_v_framesize : 4;
    unsigned tt_v_altframe : 1;
    unsigned tt_v_altrpar : 1;
    unsigned tt_v_parity : 1;
    unsigned tt_v_odd : 1;
    unsigned tt_v_twostop : 1;
    unsigned tt_v_disparerr : 1;
    unsigned tt_v_altdispar : 1;
    unsigned tt_v_break : 1;
    unsigned tt_v_fill_43_ : 4;
};


struct term_mode_iosb_st {
    unsigned short sts;
    unsigned char txspeed;
    unsigned char rxspeed;
    unsigned char cr_fill;
    unsigned char lf_fill;
    struct tt_framing_st framing;
};

struct term_char_st {
    unsigned char class;
    unsigned char type;
    unsigned short page_width;
    TTDEF ttdef;
    TT2DEF tt2def;
#if (__CRTL_VER >= 080200000) && !defined (__VAX)
    TT3DEF tt3def;
#endif
};

struct term_speed_st {
    unsigned char txspeed;
    unsigned char rxspeed;
    unsigned short speed_fill_mbz;
};

struct term_fill_st {
    unsigned short cr_fill;
    unsigned short lf_fill;
};

struct term_read_iosb_st {
    unsigned short sts;
    unsigned short count;
    unsigned short terminator;
    unsigned short terminator_count;
};

struct term_write_iosb_st {
    unsigned short sts;
    unsigned short count;
    unsigned short unknown1;
    unsigned short unknown2;
};

#pragma message restore
#pragma member_alignment restore

int LIB$DISABLE_CTRL(const unsigned long * new_mask,
		     unsigned long * old_mask);

int LIB$ENABLE_CTRL(const unsigned long * new_mask,
		    unsigned long * old_mask);

int LIB$SIGNAL(int);

int SYS$ASSIGN(
	const struct dsc$descriptor_s * devnam,
        unsigned short * chan,
        unsigned long acmode,
        const struct dsc$descriptor_s * mbxnam,
        unsigned long flags);

int   SYS$CRELNM(
	const unsigned long * attr,
	const struct dsc$descriptor_s * table_dsc,
	const struct dsc$descriptor_s * name_dsc,
	const unsigned char * acmode,
	const struct itmlst_3 * item_list);

int SYS$DASSGN(unsigned short chan);

unsigned long SYS$QIOW(
	unsigned long efn,
        unsigned short chan,
        unsigned long func,
        void * iosb,
        void (* astadr)(void *),
        ...);

int SYS$READEF(unsigned long efn, unsigned long * state);

int   SYS$TRNLNM(
	const unsigned long * attr,
	const struct dsc$descriptor_s * table_dsc,
	struct dsc$descriptor_s * name_dsc,
	const unsigned char * acmode,
	const struct itmlst_3 * item_list);

struct vms_info_st {
	unsigned short channel;		/* VMS channel assigned */
	unsigned short ref_cnt;		/* Number of references */
	char * device_name;		/* VMS device name for channel */
	unsigned long oldctrlmask;	/* Old Control Mask */
	struct termios * term_attr;	/* Unix terminal attributes */
	struct term_char_st *vms_char;  /* VMS terminal characteristics */
	struct term_mode_iosb_st * vms_iosb; /* IOSB from sense mode */
};


/* Array to hold file descriptors that we are intercepting I/O for. */
static struct vms_info_st * vms_info = NULL;
static int vms_info_size = -1;



/* fd lookup routine.  We need our special data with some file descriptors */

static struct vms_info_st * vms_lookup_fd(int fd) {

    /* Make sure memory of file descriptors is set up. */
    if (vms_info == NULL) {

	/* Find out how many channels are possible */
	vms_info_size = getdtablesize();

	/* Allocate the array */
	vms_info = malloc(vms_info_size * sizeof(struct vms_info_st));
	if (vms_info == NULL) {

	    /* We are probably out of memory, so degrade gracefully */
	    vms_info_size = -1;
	} else {
	    memset(vms_info, 0, vms_info_size * sizeof(struct vms_info_st));
	}
    }
    return &vms_info[fd];
}




/* When we encounter a terminal device, we need to see if it is one
 * that we already know about.
 * The first time we encounter a terminal device, we need to look up its
 * Unix terminal characteristics so we can do terminal I/O properly on it.
 * Poll/select from glib are modified to only deassign channels that they
 * assigned.  Eventually this module should become common to glib so that
 * glib can get the better terminal support
 */


static int vms_channel_lookup(int fd, unsigned short *channel)
{
int status;
char device_name[256];
char * retname;
struct dsc$descriptor_s dev_desc;
int call_stat;
unsigned short chan;
struct vms_info_st * info;

    status = -1;

    /* Make sure memory of file descriptors is set up. */
    if (vms_info == NULL) {

	/* Find out how many channels are possible */
	vms_info_size = getdtablesize();

	/* Allocate the array */
	vms_info = malloc(vms_info_size * sizeof(struct vms_info_st));
	if (vms_info == NULL) {

	    /* We are probably out of memory, so degrade gracefully */
	    vms_info_size = -1;
	} else {
	    memset(vms_info, 0, vms_info_size * sizeof(struct vms_info_st));
	}
    }

    /* Do we know about this one? */
    info = vms_lookup_fd(fd);
    if (info->ref_cnt > 0) {
	/* We found it */
	*channel = info->channel;

	/* Do not increment the ref count for STDIN/STDOUT/STDERR */
	if (fd > 2) {
	    info->ref_cnt++;
	}
	return 0;
    }

     /* get the name */
    /*--------------*/
    retname = getname(fd, device_name, 1);
    if (retname != NULL) {

	/* Store the name */
	info->device_name = strdup(device_name);

	 /* Assign the channel */
	/*--------------------*/
	dev_desc.dsc$a_pointer = device_name;
	dev_desc.dsc$w_length = strlen(device_name);
	dev_desc.dsc$b_dtype = DSC$K_DTYPE_T;
	dev_desc.dsc$b_class = DSC$K_CLASS_S;
	call_stat = SYS$ASSIGN(&dev_desc, &chan, 0, 0, 0);
	if ($VMS_STATUS_SUCCESS(call_stat)) {
	    *channel = chan;
	    info->channel = chan;
	    info->ref_cnt = 1;
	    status = 0;
	}

	/* Initialize the rest of the structure */
	info->term_attr = NULL;
    }

    return status;
}


/* Close the vms_channel on the last lookup */

static int vms_channel_close(int fd) {
    int status = 0;

    if (vms_info[fd].ref_cnt) {
	if (fd > 2) {
	    vms_info[fd].ref_cnt--;
	}
    }
    if (vms_info[fd].ref_cnt == 0) {
    int call_stat;

	/* All references done, time to clean up */

	/* Cancel any ASTs for signals */

	/* Remove the channel */
	call_stat = SYS$DASSGN(vms_info[fd].channel);
	if ($VMS_STATUS_SUCCESS(call_stat)) {
	    status = 0;
	} else {
	    errno = EIO;
	    status = -1;
	}

	/* Clean up the cached device name */
	if (vms_info[fd].device_name != NULL) {
	    free(vms_info[fd].device_name);
	    vms_info[fd].device_name = NULL;
	}

	/* Clean up the VMS terminal attributes */
	if (vms_info[fd].vms_char != NULL) {
	    free(vms_info[fd].vms_char);
	    vms_info[fd].vms_char = NULL;
	}
	if (vms_info[fd].vms_iosb != NULL) {
	    free(vms_info[fd].vms_iosb);
	    vms_info[fd].vms_iosb = NULL;
	}

	/* Clean up the Unix terminal attributes */
	if (vms_info[fd].term_attr != NULL) {
	    free(vms_info[fd].term_attr);
	    vms_info[fd].term_attr = NULL;
	}
    }
    return status;
}





/* vms_speed_to_unix
 *
 * speed - VMS Speed value
 *
 * returns Unix speed value or 0 if speed can not be calculated.
 *
 */

static int vms_speed_to_unix_speed(unsigned char speed) {
    switch(speed) {
    case TT$C_BAUD_50:		return B50;
    case TT$C_BAUD_75:		return B75;
    case TT$C_BAUD_110: 	return B110;
    case TT$C_BAUD_134: 	return B134;
    case TT$C_BAUD_150: 	return B150;
    case TT$C_BAUD_300: 	return B300;
    case TT$C_BAUD_600: 	return B600;
    case TT$C_BAUD_1200:	return B1200;
    case TT$C_BAUD_2400:	return B2400;
    case TT$C_BAUD_4800:	return B4800;
    case TT$C_BAUD_7200:	return B7200;
    case TT$C_BAUD_9600:	return B9600;
    case TT$C_BAUD_19200:	return B19200;
    case TT$C_BAUD_38400:	return B38400;
    }
    return 0;
}

/* unix_speed_to_vms
 *
 * speed - Unix Speed value
 *
 * returns VMS speed value or 0 if speed can not be calculated.
 *
 */

static int unix_speed_to_vms_speed(unsigned char speed) {
    switch(speed) {
    case B50:		return TT$C_BAUD_50;
    case B75:		return TT$C_BAUD_75;
    case B110: 		return TT$C_BAUD_110;
    case B134: 		return TT$C_BAUD_134;
    case B150: 		return TT$C_BAUD_150;
    case B300: 		return TT$C_BAUD_300;
    case B600: 		return TT$C_BAUD_600;
    case B1200:		return TT$C_BAUD_1200;
    case B2400:		return TT$C_BAUD_2400;
    case B4800:		return TT$C_BAUD_4800;
    case B7200:		return TT$C_BAUD_7200;
    case B9600:		return TT$C_BAUD_9600;
    case B19200:	return TT$C_BAUD_19200;
    case B38400:	return TT$C_BAUD_38400;
    }
    return 0;
}

/* vms_term_qio_tcgetattr
 * Does the actual work of getting those VMS terminal characteristics
 * that can be mapped to Unix terminal characteristics.
 *
 * vms_info - on input, the VMS device channel, and other information.
 *            on output, updated to reflect the current status.
 *
 * my_attr  - Structure containing the equivalent Unix characteristics.
 *
 * Returns 0 for success, -1 for an error and sets errno on failure.
 *
 */
int vms_term_qio_tcgetattr(struct vms_info_st * vms_info,
			   struct termios *my_attr) {
int status;
struct term_mode_iosb_st * mode_iosb;
struct term_char_st * termchar;
const unsigned long newmask = 0;
unsigned int opost_active = 0;
unsigned int any_fill = 0;
unsigned int cflag_active = 0;
int ret_stat = 0;

    /* Get the old control character mask */
    status = LIB$ENABLE_CTRL(&newmask, &vms_info->oldctrlmask);
    if (!$VMS_STATUS_SUCCESS(status)) {
	errno = EIO;
	return -1;
    }

    /* Set up the structures to hold the information if not already there */
    if (vms_info->vms_char == NULL) {
	vms_info->vms_char = malloc(sizeof(struct term_char_st));
	if (vms_info->vms_char == NULL) {
	    return -1;
	}
    }
    termchar = vms_info->vms_char;

    if (vms_info->vms_iosb == NULL) {
	vms_info->vms_iosb = malloc(sizeof(struct term_mode_iosb_st));
	if (vms_info->vms_iosb == NULL) {
	    return -1;
	}
    }
    mode_iosb = vms_info->vms_iosb;

    if (vms_info->term_attr == NULL) {
	vms_info->term_attr = malloc(sizeof(struct termios));
	if (vms_info->term_attr == NULL) {
	    return -1;
	}
    }

    status = SYS$QIOW
       (EFN$C_ENF,
	vms_info->channel,
	IO$_SENSEMODE,
	vms_info->vms_iosb,
	NULL,
	NULL,
	termchar, sizeof(struct term_char_st), 0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) &&
	$VMS_STATUS_SUCCESS(mode_iosb->sts)) {
	    /* We have data */
	    ret_stat = 0;
    } else {
	/* Something really wrong */
	ret_stat = -1;
	errno = EIO;
	return ret_stat;
    }

    memset(my_attr, 0, sizeof(struct termios));

    /* Input Flags */
    my_attr->c_iflag |= IGNBRK; /* VMS driver handles BREAK */
/*			BKRINT     VMS driver handles BREAK */
/*			IGNPAR     VMS fatal error on bad parity */
/*                      PARMRK     VMS can not mark parity errors */
    if (mode_iosb->framing.tt_v_disparerr) {
        my_attr->c_iflag |= INPCK;  /* Parity is enabled */
    }
    if (!termchar->ttdef.tt$v_eightbit) {
	my_attr->c_iflag |= ISTRIP;  /* Strip 8th bit off characters */
    }
/*			INLCR	VMS terminates on linefeed and passes through */
/*			IGNCR   VMS can not ignore CR */
    my_attr->c_iflag |= ICRNL;   /* VMS RMS maps CR to LF on input */
    if (termchar->ttdef.tt$v_ttsync) {
	my_attr->c_iflag |= IXON;  /* XON/XOFF on output */
    }
    if (termchar->ttdef.tt$v_hostsync) {
	my_attr->c_iflag |= IXOFF; /* XON/XOFF on input */
    }
#ifdef __USE_BSD
/*			IXANY	VMS will not do this. */
    my_attr->c_iflag |= IMAXBEL;    /* VMS rings bell when queue full */
#endif
#ifdef __USE_GNU
/*			IUCLC   VMS will not translate upper to lower */
#endif


    /* Output flags */
#if defined __USE_BSD || defined __USE_XOPEN
    my_attr->c_oflag |= ONLCR;  /* VMS RMS does this automatically */
    opost_active = 1;
#endif
#ifdef __USE_BSD
/*			OXTABS	 (ALIAS for TAB3)  Expand tabs to spaces */
/*			ONOEOT		VMS does not pass through ^Z EOF */
/*					to output.			 */
/*					Ignore for now.			 */
#endif
#if defined __USE_BSD || defined __USE_XOPEN
/*			OCRNL	VMS will not map CR to NL on output */
/*			ONOCR   Will VMS discard CRs when on column 0? */
    my_attr->c_oflag |= ONLRET; /*VMS RMS adds a CR to NL on output */
    opost_active = 1;
#endif
#if defined __USE_MISC || defined __USE_XOPEN
/* NLDLY */
/*				NL0 - No delay */
/*				NL1 - Delay 0.10 seconds (2 chars) */
/* FFDLY */
/*				FF0 - No delay */
/*				FF1 - Delay about 2 seconds */
/* VTDLY */
/*				VT0 - No delay */
/*				VT1 - Delay about 2 seconds */
    if (mode_iosb->lf_fill != 0) {
	any_fill = 1;
	opost_active = 1;
	my_attr->c_oflag |= (VT1|FF1|VT1);
    }
/* CRDLY */
/*				CR0 - No delay */
/*				CR1 - column dependent (2 chars) */
/*				CR2 - Delay 0.10 seconds (4 chars) */
/*				CR3 - Delay 0.15 seconds */
    if (mode_iosb->cr_fill != 0) {
	any_fill = 1;
	opost_active = 1;
	if (mode_iosb->cr_fill > 4) {
	    my_attr->c_oflag |= CR3;
	} else if (mode_iosb->cr_fill > 2) {
	    my_attr->c_oflag |= CR2;
	} else {
	    my_attr->c_oflag |= CR1;
	}
    }
/* TABDLY */
/*				TAB0 - No delay */
/*				TAB1 - column dependent */
/*				TAB2 - Delay 0.10 seconds (2 chars) */
/*				TAB3 - Expand TAB to spaces */
    if (!termchar->ttdef.tt$v_mechtab) {
	my_attr->c_oflag |= TAB3;	/* Expand tabs to spaces */
	opost_active = 1;
    }
/* BSDLY Not present on VMS */
#endif
#ifdef __USE_GNU
    if (!termchar->ttdef.tt$v_lower) {
	my_attr->c_oflag |= OLCUC;	/* VMS output all upper case */
	opost_active = 1;
    }
#endif
/*			    OFDEL	X/Open says use DEL instead of NULL */
/*					GNU/Linux does not have a definition */
#ifdef __USE_XOPEN
    if (any_fill) {
					/* X/Open Says this means 2	  */
					/* characters sent for delay	  */
	my_attr->c_oflag |= OFILL;	/* Send fill characters for delay */
	opost_active = 1;
    }
#endif

    if (opost_active) {
	my_attr->c_oflag |= OPOST;  /* VMS At least one post feature active */
    }

    /* Control Modes */
    if (mode_iosb->framing.tt_v_altframe) {
	switch(mode_iosb->framing.tt_v_framesize) {
	case 6: my_attr->c_cflag |= CS6;
		cflag_active = 1;
		break;
	case 7: my_attr->c_cflag |= CS7;
		cflag_active = 1;
		break;
	case 8: my_attr->c_cflag |= CS8;
		cflag_active = 1;
		break;
	}
    }
    if (mode_iosb->framing.tt_v_twostop) {  /* two stop bits */
	my_attr->c_cflag |= CSTOPB;
	cflag_active = 1;
    }
    my_attr->c_cflag |= CREAD;	/* VMS not easily available, so say ready */
    cflag_active = 1;
    if (mode_iosb->framing.tt_v_parity) { /* Enable parity */
	my_attr->c_cflag |= PARENB;
	cflag_active = 1;
    }
    if (mode_iosb->framing.tt_v_odd) { /* Odd parity */
	my_attr->c_cflag |=  PARODD;
	cflag_active = 1;
    }
    if (!termchar->ttdef.tt$v_modem ) {
	my_attr->c_cflag |= CLOCAL;	/* Local terminal */
	cflag_active = 1;
    } else {
	my_attr->c_cflag |= HUPCL;	/* Hang up on last close */
    }

#ifdef __USE_BSD
#if (__CRTL_VER >= 080200000) && !defined (__VAX)
    if (termchar->tt3def.tt3$v_rts_flow) {
        my_attr->c_cflag |= CRTSCTS;	/* VMS 8.2 has CTRCTS flow control */
	cflag_active = 1;
    }
#else
    if (termchar->tt2def.tt2$v_commsync) {
	my_attr->c_cflag |= CRTSCTS;	/* Comm sync uses hardware flow ctrl */
	cflag_active = 1;
    }
#endif
    if (termchar->tt2def.tt2$v_commsync) {
	my_attr->c_cflag |= CDTRCTS;	/* Comm sync uses hardware flow ctrl */
	cflag_active = 1;
    }
    if (termchar->ttdef.tt$v_modem) {
	my_attr->c_cflag |= MDMBUF;	/* Modem connected terminal */
	cflag_active = 1;
    }

    if (!cflag_active) {
	my_attr->c_cflag |= CIGNORE; /* Only if no control flags are set */
    }
#endif

    /* Local Modes */
    if (termchar->ttdef.tt$v_scope) {   /* VMS terminal SCOPE set */
	my_attr->c_lflag |= ECHOKE;
	my_attr->c_lflag |= ECHOE;
    } else {
	my_attr->c_lflag |= ECHOK;
    }

    if (!termchar->ttdef.tt$v_noecho) {  /* Characters are echoed */
	my_attr->c_lflag |= ECHO;
    }
/*			ENCHONL   VMS will not echo NL with echo off */
#ifdef __USE_BSD
    my_attr->c_lflag |= ECHOPRT;	/* VMS echos erase characters */
    my_attr->c_lflag |= ECHOCTL;	/* VMS echos control characters */
#endif

					/* VMS pass all input characters */
    if (!termchar->ttdef.tt$v_passall && !termchar->tt2def.tt2$v_pasthru) {
	my_attr->c_lflag |= ISIG;	/* VMS normal mode */
	my_attr->c_lflag |= ICANON;
    }
#ifdef __USE_BSD
/*			ALTWERASE	VMS is hard coded */
#endif
    if (!termchar->ttdef.tt$v_passall && !termchar->tt2def.tt2$v_pasthru) {
	my_attr->c_lflag |= IEXTEN; /* VMS does ^O and ^V normally */
    } else {
	my_attr->c_lflag |= EXTPROC; /* Application, not interactive */
    }
/*			TOSTOP		VMS does not have SIGTTOU */
#ifdef __USE_BSD
/*			FLUSHO		VMS and Linux do not do this. */
    if ((vms_info->oldctrlmask & LIB$M_CLI_CTRLT) == 0) {
	my_attr->c_lflag |= NOKERNINFO; /* VMS Control-T Disabled */
    }
/*			PENDIN		Not sure how to do on VMS */
#endif
/*			NOFLSH		VMS does not do this */

/*			XCASE		VMS does not have this */

    /* Control Character Array */
    my_attr->c_cc[VEOF] = 26;		/* VMS EOF is Control-Z */
					/* Unix EOF is Control-D */
    my_attr->c_cc[VEOL] = 0;		/* Not used with VMS/Linux */
#ifdef __USE_BSD
    my_attr->c_cc[VEOL2] = 0;		/* Not used with VMS/Linux */
#endif
    my_attr->c_cc[VERASE] = 127;	/* VMS default is DEL */
#if (__CRTL_VER >= 080200000) && !defined (__VAX)
    if (termchar->tt3def.tt3$v_bs) {
	my_attr->c_cc[VERASE] = 8;	/* VMS 8.2 can set BS */
    }
#endif
#ifdef __USE_BSD
    my_attr->c_cc[VWERASE] = 10;	/* VMS uses LF to delete word */
					/* Unix uses Control-W */
#endif
    my_attr->c_cc[VKILL] = 21;		/* VMS uses Control-U to delete line */
    my_attr->c_cc[VREPRINT] = 18;	/* VMS uses Control-R to reprint */
    my_attr->c_cc[VINTR] = 3;		/* VMS uses Control-C */
    my_attr->c_cc[VQUIT] = 0;		/* VMS does not have Quit Character */
					/* Unix uses Control-\ */
    my_attr->c_cc[VSUSP] = 0;		/* VMS does not have Suspend */
#ifdef __USE_BSD
    my_attr->c_cc[VDSUSP] = 0;	/* VMS does not have a Delayed suspend */
				/* Unix uses Control-y */
#endif
    my_attr->c_cc[VSTART] = 17;		/* VMS uses Control-Q */
    my_attr->c_cc[VSTOP] = 19;		/* VMS uses Control-S */
    my_attr->c_cc[VLNEXT] = 22;		/* VMS uses Control-V */
    my_attr->c_cc[VDISCARD] = 15;	/* VMS uses Control-O */
    my_attr->c_cc[VSTATUS] = 20;	/* VMS uses Control-T */

    /* Speed section */
    my_attr->c_ospeed = vms_speed_to_unix_speed(mode_iosb->txspeed);
    if (mode_iosb->rxspeed == 0) {
	my_attr->c_ispeed = my_attr->c_ospeed;
    } else {
	my_attr->c_ispeed = vms_speed_to_unix_speed(mode_iosb->rxspeed);
    }

    memcpy(vms_info->term_attr, my_attr, sizeof(struct termios));

    return ret_stat;
}


/* vms_term_qio_tcsetattr
 * Does the actual work of setting those VMS terminal characteristics
 * that can be set, ignoring the rest.
 * The vms_term_qio_tcgetattr routine needs to initially populate the vms_info
 * structure that is passed by reference before this call.
 * The vms_info structure will be updated to reflect the changes.
 *
 * vms_info - on input, the VMS device channel, and other information.
 *            on output, updated to reflect any change requested.
 *
 * my_attr  - Readonly structure containing the desired Unix characteristics.
 *
 * We do not seem to be able to set fill here and have not tested
 * Setting speed.  Attempting to set parity returns a SS$_BADPARAM
 * On VMS these should be set before running the program.
 *
 * Returns 0 for success, -1 for an error and sets errno on failure.
 *
 */
int vms_term_qio_tcsetattr(struct vms_info_st * vms_info,
			   const struct termios *my_attr) {
int status;
struct term_mode_iosb_st * mode_iosb;
struct term_char_st * termchar;
const unsigned long newmask = LIB$M_CLI_CTRLT;
unsigned long oldmask;
int is_modem;
int is_scope;
int ret_stat;
struct term_mode_iosb_st set_mode_iosb;

    /* Set up the structures to hold the information if not already there */
    if (vms_info->vms_char == NULL) {
	struct termios old_attr;
	ret_stat = vms_term_qio_tcgetattr(vms_info, &old_attr);
	if (ret_stat < 0) {
	    return ret_stat;
	}
    }

    termchar = vms_info->vms_char;
    mode_iosb = vms_info->vms_iosb;

    is_modem = termchar->ttdef.tt$v_modem;
    is_scope = termchar->ttdef.tt$v_scope;

    /* Input Flags */
/*			IGNBRK     VMS driver handles BREAK */
/*			BKRINT     VMS driver handles BREAK */
/*			IGNPAR     VMS fatal error on bad parity */
/*                      PARMRK     VMS can not mark parity errors */

/* On VMS, we are not setting the framing or parity this way */
/*			INPCK	   Parity is enabled ? */

    if ((my_attr->c_iflag & ISTRIP) != 0) {  /* Strip 8th bit off characters */
        termchar->ttdef.tt$v_eightbit = 0;
    } else {
        termchar->ttdef.tt$v_eightbit = 1;
    }
/*			INLCR	VMS terminates on linefeed and passes through */
/*			IGNCR   VMS can not ignore CR */
/*			ICRNL   VMS RMS maps CR to LF on input */

    if ((my_attr->c_iflag & IXON) != 0) {  /* XON/XOFF on output */
        termchar->ttdef.tt$v_ttsync = 1;
    } else {
        termchar->ttdef.tt$v_ttsync = 0;
    }
    if ((my_attr->c_iflag & IXOFF) != 0) { /* XON/XOFF on input */
        termchar->ttdef.tt$v_hostsync = 1;
    } else {
        termchar->ttdef.tt$v_hostsync = 0;
    }
#ifdef __USE_BSD
/*			IXANY	VMS will not do this. */
/*			IMAXBEL VMS rings bell when queue full */
#endif
#ifdef __USE_GNU
/*			IUCLC   VMS will not translate upper to lower */
#endif


    /* Output flags */
#if defined __USE_BSD || defined __USE_XOPEN
/*			ONLCR	VMS RMS does this automatically */
#endif

#ifdef __USE_BSD
/*			OXTABS	Alias for TAB3 */
/*			ONOEOT		VMS does not pass through ^Z EOF */
/*					to output.			 */
/*					Ignore for now.			 */
#endif
#if defined __USE_BSD || defined __USE_XOPEN
/*			OCRNL	VMS will not map CR to NL on output */
/*			ONOCR   Will VMS discard CRs when on column 0? */
/*			ONLRET  VMS RMS adds a CR to NL on output */
#endif
#if defined __USE_MISC || defined __USE_XOPEN
/* NLDLY */
/*				NL0 - No delay */
/*				NL1 - Delay 0.10 seconds (2 chars) */
/* FFDLY */
/*				FF0 - No delay */
/*				FF1 - Delay about 2 seconds */
/* VTDLY */
/*				VT0 - No delay */
/*				VT1 - Delay about 2 seconds */
/* CRDLY */
/*				CR0 - No delay */
/*				CR1 - column dependent (2 chars) */
/*				CR2 - Delay 0.10 seconds (4 chars) */
/*				CR3 - Delay 0.15 seconds */
/* TABDLY */
/*				TAB0 - No delay */
/*				TAB1 - column dependent */
/*				TAB2 - Delay 0.10 seconds (2 chars) */
/*				TAB3 - Expand TAB to spaces */
    if ((my_attr->c_oflag & TAB3) != 0) {	/* Expand tabs to spaces */
	termchar->ttdef.tt$v_mechtab = 0;
    } else {
	termchar->ttdef.tt$v_mechtab = 1;
    }


/* BSDLY Not present on VMS */
#endif
#ifdef __USE_GNU
    if ((my_attr->c_oflag & OLCUC) != 0) {  /* VMS output all upper case */
	termchar->ttdef.tt$v_lower = 0;
    } else {
	termchar->ttdef.tt$v_lower = 1;
    }
#endif
/*			    OFDEL	X/Open says use DEL instead of NULL */
/*					GNU/Linux does not have a definition */
#ifdef __USE_XOPEN
					/* X/Open Says this means 2	  */
					/* characters sent for delay	  */
/*			OFILL	*/	/* Send fill characters for delay */
#endif

/*			OPOST		VMS At least one post feature active */

    /* Control Modes */

#ifdef __USE_BSD
/*			CIGNORE		Only if no control flags are set */
    if ((my_attr->c_cflag & CIGNORE) != 0) {
#endif

/*		CSIZE == CS6 , CS7, CS8  Frame size not set here on VMS */
/*			CSTOPB		Stop bits not set here on VMS */

/*  			CREAD	    VMS not easily available, so say ready */

/*			PARENB	    VMS not setting parity here */
/*			PARODD	    VMS not setting Odd parity here */

/*			HUPCL		Hangup modem on logout */
/*			CLOCAL		Local terminal - Not modem */
/* Resolving conflict by having HUPCL override CLOCAL */
/* Setting modem by sensemode should disconnect or logout the process */
	if ((my_attr->c_cflag & CLOCAL) != 0) {
	    is_modem = 0;
	}
	if ((my_attr->c_cflag & HUPCL) != 0) {
	    is_modem = 1;
	}

#ifdef __USE_BSD
	/* VMS 8.2 has CTRCTS flow control */
#if (__CRTL_VER >= 080200000) && !defined (__VAX)
	if ((my_attr->c_cflag & CRTSCTS) != 0) {
	    termchar->tt3def.tt3$v_rts_flow = 1;
	} else {
	    termchar->tt3def.tt3$v_rts_flow = 0;
	}
#else
	if ((my_attr->c_cflag & CRTSCTS) != 0) {
	    /* Comm sync uses hardware flow ctrl */
	    termchar->tt2def.tt2$v_commsync = 1;
	} else {
	   termchar->tt2def.tt2$v_commsync = 0;
	}
#endif
	/* Comm sync uses hardware flow ctrl */
	if ((my_attr->c_cflag & CDTRCTS) != 0) {
	    termchar->tt2def.tt2$v_commsync = 1;
	} else {
	    termchar->tt2def.tt2$v_commsync = 0;
	}
	if ((my_attr->c_cflag & MDMBUF) != 0) {	/* Modem connected terminal */
	    is_modem = 1;
	}
#endif

#ifdef __USE_BSD
    }
#endif

    /* Local Modes */
    if (((my_attr->c_lflag & ECHOKE) != 0) ||
	((my_attr->c_lflag & ECHOE) != 0)) {
	is_scope = 1;			/* VMS terminal SCOPE set */
    } else if ((my_attr->c_lflag & ECHOK) != 0) {
	is_scope = 0;			/* VMS terminal SCOPE clear */
    }


/*			ECHOK		Echo NL after kill (non-scope) */

    if ((my_attr->c_lflag & ECHO) != 0) {
	termchar->ttdef.tt$v_noecho = 0;  /* Characters are echoed */
    } else {
	termchar->ttdef.tt$v_noecho = 1;  /* Characters are not echoed */
    }
/*			ENCHONL   VMS will not echo NL with echo off */
#ifdef __USE_BSD
/*			ECHOPRT   VMS hardcopy echos erase characters */
/*			ECHOCTL	  VMS echos control characters */
#endif

    if (((my_attr->c_lflag & ISIG) != 0) ||
	((my_attr->c_lflag & ICANON) != 0) ||
	((my_attr->c_lflag & IEXTEN) != 0)) {

	/* VMS interpret input characters */
	termchar->tt2def.tt2$v_pasthru = 0;
    } else {
	/* VMS pass-through input characters */
	termchar->tt2def.tt2$v_pasthru = 1;
    }
#ifdef __USE_BSD
/*			ALTWERASE	VMS is hard coded */
#endif
/*			IEXTEN		VMS does ^O and ^V normally */

/*			EXTPROC		Application, not interactive */
/*					Need to set before entering */
/*			TOSTOP		VMS does not have SIGTTOU */
#ifdef __USE_BSD
/*			FLUSHO		VMS and Linux do not do this. */
    if ((my_attr->c_lflag & NOKERNINFO) != 0) {  /* VMS Control-T handling */
	if ((vms_info->oldctrlmask & LIB$M_CLI_CTRLT) != 0) {
	    status = LIB$DISABLE_CTRL(&newmask, &oldmask);
	}
    } else {
	if ((vms_info->oldctrlmask & LIB$M_CLI_CTRLT) == 0) {
	    status = LIB$ENABLE_CTRL(&newmask, &oldmask);
	}
    }
/*			PENDIN		Not sure how to do on VMS */
#endif
/*			NOFLSH		VMS does not do this */

/*			XCASE		VMS does not have this */

    /* Control Character Array */
/*    my_attr->c_cc[VEOF] = 26; */	/* VMS EOF is Control-Z */
					/* Unix EOF is Control-D */
/*    my_attr->c_cc[VEOL] = 0; */	/* Not used with VMS/Linux */
#ifdef __USE_BSD
/*    my_attr->c_cc[VEOL2] = 0; */	/* Not used with VMS/Linux */
#endif
/*    my_attr->c_cc[VERASE] = 127; */	/* VMS default is DEL */
#if (__CRTL_VER >= 080200000) && !defined (__VAX)
    if (my_attr->c_cc[VERASE] == 8) {	/* VMS 8.2 can set BS */
    	termchar->tt3def.tt3$v_bs = 1;
    } else {
	termchar->tt3def.tt3$v_bs = 0;
    }
#endif
#ifdef __USE_BSD
/*  my_attr->c_cc[VWERASE] = 10; */	/* VMS uses LF to delete word */
					/* Unix uses Control-W */
#endif
/*    my_attr->c_cc[VKILL] = 21; */	/* VMS uses Control-U to delete line */
/*    my_attr->c_cc[VREPRINT] = 18; */	/* VMS uses Control-R to reprint */
/*    my_attr->c_cc[VINTR] = 3;	*/	/* VMS uses Control-C */
/*    my_attr->c_cc[VQUIT] = 0;	*/	/* VMS does not have Quit Character */
					/* Unix uses Control-\ */
/*    my_attr->c_cc[VSUSP] = 0; */	/* VMS does not have Suspend */
#ifdef __USE_BSD
/*    my_attr->c_cc[VDSUSP] = 0; */ /* VMS does not have a Delayed suspend */
				/* Unix uses Control-y */
#endif
/*    my_attr->c_cc[VSTART] = 17; */	/* VMS uses Control-Q */
/*    my_attr->c_cc[VSTOP] = 19; */	/* VMS uses Control-S */
/*    my_attr->c_cc[VLNEXT] = 22; */	/* VMS uses Control-V */
/*    my_attr->c_cc[VDISCARD] = 15; */	/* VMS uses Control-O */
/*    my_attr->c_cc[VSTATUS] = 20; */	/* VMS uses Control-T */

    /* Is this a video terminal? */
    termchar->ttdef.tt$v_scope = is_scope;

    /* Is this a modem? */
    termchar->ttdef.tt$v_modem = is_modem;

    status = SYS$QIOW
       (EFN$C_ENF,
	vms_info->channel,
	IO$_SETMODE,
	&set_mode_iosb,
	NULL,
	NULL,
	termchar, sizeof(struct term_char_st),
	0, 0, 0, 0);
    if ($VMS_STATUS_SUCCESS(status) &&
	$VMS_STATUS_SUCCESS(set_mode_iosb.sts)) {

	    /* We set the data, cache it */
	    vms_info->term_attr->c_iflag = my_attr->c_iflag;
	    vms_info->term_attr->c_oflag = my_attr->c_oflag;
	    vms_info->term_attr->c_lflag = my_attr->c_lflag;
	    if (termchar->tt2def.tt2$v_pasthru) {
	        vms_info->term_attr->c_lflag &= ~(ISIG|ICANON|IEXTEN);
	    }

	    /* Except if CIGNORE set for changes */
	    if ((my_attr->c_cflag & CIGNORE) == 0) {
		vms_info->term_attr->c_cflag = my_attr->c_cflag;
	    }

	    /* VMS 8.2 can set BS, so this could have changed */
	    vms_info->term_attr->c_cc[VERASE] = my_attr->c_cc[VERASE];

    } else {
	/* Something really wrong */
	ret_stat = -1;
	errno = EIO;
    }

    return ret_stat;
}



  /********************************/
 /* termios replacement routines */
/********************************/
int tcgetattr(int fd, struct termios * buf) {
int result;
unsigned short chan;
int status;

    /* Would someone call this on a non-terminal? */
#if 0
    if (!isatty(fd)) {
	errno = EBADF;
	return -1;
    }
#endif
    /* Open the channel if needed */
    status = vms_channel_lookup(fd, &chan);
    if (status == 0) {


	/* We need to first get the current settings */
	status = vms_term_qio_tcgetattr(&vms_info[fd], buf);

	if (status != 0) {
	    vms_channel_close(fd);
	    return -1;
	}


	/* Unix mode, just look up the last cached value */
	memcpy(buf, vms_info[fd].term_attr, sizeof (struct termios));
	status = 0;

	/* close the channel if we opened it */
	vms_channel_close(fd);
    }
    return status;
}



int tcsetattr(int fd, int action, const struct termios * buf) {

int result;
unsigned short chan;
int status;

    /* Would someone call this on a non-terminal? */
#if 0
    if (!isatty(fd)) {
	errno = EBADF;
	return -1;
    }
#endif
    if (action != TCSANOW) {
	/* Unless we totally take over the I/O, the best that */
	/* we can do is attempt a a fsync() */
	fsync(fd);
    }

    /* Open the channel if needed */
    status = vms_channel_lookup(fd, &chan);
    if (status == 0) {
	/* Now attempt to set them */
	status = vms_term_qio_tcsetattr(&vms_info[fd], buf);
    }

    /* close the channel if we opened it */
    vms_channel_close(fd);
    return status;
}




