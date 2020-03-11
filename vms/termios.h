/*
 *************************************************************************
 *                                                                       *
 * Copyright 2000 Compaq Computer Corporation                            *
 *                                                                       *
 * COMPAQ Registered in U.S. Patent and Trademark Office.                *
 *                                                                       *
 *************************************************************************
 * IMPORTANT: Carefully read the License Terms below before              *
 * proceeding.  By use of these materials you agree to these terms.      *
 * If you do not agree to these terms, you may not use this software or  *
 * the accompanying documentation.                                       *
 *************************************************************************
 * LICENSE TERMS                                                         *
 * 1. GRANT                                                              *
 * Compaq Computer Corporation ("COMPAQ") grants you the right to use,   *
 * modify, and distribute the following source code (the "Software")     *
 * on any number of computers. You may use the Software as part of       *
 * creating a software program or product intended for commercial or     *
 * non-commercial distribution in machine-readable source code, binary,  *
 * or executable formats. You may distribute the Software as             *
 * machine-readable source code provided this license is not removed     *
 * from the Software and any modifications are conspicuously indicated.  *
 * 2. COPYRIGHT                                                          *
 * The Software is owned by COMPAQ and its suppliers and is protected by *
 * copyright laws and international treaties.  Your use of the Software  *
 * and associated documentation is subject to the applicable copyright   *
 * laws and the express rights and restrictions of these terms.          *
 * 3. RESTRICTIONS                                                       *
 * You may not remove any copyright, trademark, or other proprietary     *
 * notices from the Software or the associated  documentation.           *
 * You are responsible for compliance with all applicable export or      *
 * re-export control laws and regulations if you export the Software.    *
 * This license is governed by and is to be construed under the laws     *
 * of the State of Texas.                                                *
 *                                                                       *
 * DISCLAIMER OF WARRANTY AND LIABILITY                                  *
 * Compaq shall not be liable for technical or editorial errors or       *
 * omissions contained herein. The information contained herein is       *
 * subject to change without notice.                                     *
 *                                                                       *
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.       *
 * THE ENTIRE RISK ARISING OUT OF THE USE OF THIS SOFTWARE REMAINS WITH  *
 * RECIPIENT.  IN NO EVENT SHALL COMPAQ BE LIABLE FOR ANY DIRECT,        *
 * CONSEQUENTIAL, INCIDENTAL, SPECIAL, PUNITIVE OR OTHER DAMAGES         *
 * WHATSOEVER (INCLUDING WITHOUT LIMITATION DAMAGES FOR LOSS OF BUSINESS *
 * PROFITS, BUSINESS INTERRUPTION, OR LOSS OF BUSINESS INFORMATION),     *
 * EVEN IF COMPAQ HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES    *
 * AND WHETHER IN AN ACTION OF CONTRACT OR TORT INCLUDING NEGLIGENCE.    *
 *                                                                       *
 * If you have any questions concerning this license, please contact:    *
 * Compaq Computer Corporation, Software Business Practices, ZKO1-2/D22, *
 * 110 Spit Brook Road, Nashua, NH. 03062-2698.                          *
 *                                                                       *
 *************************************************************************
 */

# ifndef __TERMIOS_LOADED
# define __TERMIOS_LOADED 1


# if __DECC_VER<50000000 && __DECCXX_VER<50000000   /* PROLOGUE version X-7 */
#   error POSIX for OpenVMS V3.0 requires DEC C or DEC C++ V5.0 or later
# endif
# if __64BITS_A || __64BITS_B
#   error The /INTS compile time option is not supported
# endif
# pragma __environment __save
# pragma __environment __header_defaults
# pragma __extern_model __strict_refdef
# if __cplusplus
extern "C" {
# endif
# ifndef __CHAR_SP
#   define __CHAR_SP 1
#   ifdef __INITIAL_POINTER_SIZE
#   pragma __required_pointer_size __long
#   endif
    typedef char *	__char_lp;	/* 64-bit pointer */
    typedef void *	__void_lp;	/* 64-bit pointer */
    typedef int *	__int_lp;	/* 64-bit pointer */
    typedef const char *__kchar_lp;	/* 64-bit pointer */
    typedef const void *__kvoid_lp;	/* 64-bit pointer */
    typedef const int  *__kint_lp;	/* 64-bit pointer */
#   ifdef __INITIAL_POINTER_SIZE
#   pragma __required_pointer_size __short
#   endif
    typedef char *	__char_sp;	/* 32-bit pointer */
    typedef void *	__void_sp;	/* 32-bit pointer */
    typedef int *	__int_sp;	/* 32-bit pointer */
    typedef const char *__kchar_sp;	/* 32-bit pointer */
    typedef const void *__kvoid_sp;	/* 32-bit pointer */
    typedef const int  *__kint_sp;	/* 64-bit pointer */
# endif

typedef unsigned long	tcflag_t;
typedef unsigned char	speed_t;
typedef unsigned char	cc_t;

# define VINTR	 0
# define VQUIT	 1
# define VERASE	 2
# define VKILL	 3
# define VEOF	 4
# define VMIN	 5
# define VEOL	 6
# define VTIME	 7
# define VSUSP	 8
# define VSTART  9
# define VSTOP	10
# define VDEBUG	11

# define VEOL2		12		/* BRC 22-Jun-2017 */
# define VWERASE 	13		/* BRC 22-Jun-2017 */
# define VREPRINT	14		/* BRC 22-Jun-2017 */
# define VDSUSP		14		/* BRC 22-Jun-2017 */
# define VLNEXT		16		/* BRC 22-Jun-2017 */
# define VDISCARD	17		/* BRC 22-Jun-2017 */
# define VSTATUS	18		/* BRC 22-Jun-2017 */

# define NCCS	20	/* Includes room for future extensions		    */

struct termios
{
    tcflag_t	c_iflag;
#   define		IGNBRK	0000001
#   define		BRKINT	0000002
#   define		IGNPAR	0000004
#   define		PARMRK	0000010
#   define		INPCK	0000020
#   define		ISTRIP	0000040
#   define		INLCR	0000100
#   define		IGNCR	0000200
#   define		ICRNL	0000400
#   define		IUCLC	0001000
#   define		IXON	0002000
#   define		IXANY	0004000
#   define		IXOFF	0010000
#   define 		IMAXBEL 0020000		/* BRC 22-Jun-2017 */

    tcflag_t	c_oflag;
#   define		OPOST	0000001
#   define		OLCUC	0000002
#   define		ONLCR	0000004
#   define		OCRNL	0000010
#   define		ONOCR	0000020
#   define		ONLRET	0000040
#   define		OFILL	0000100
#   define		OFDEL	0000200
#   define		NLDLY	0000400
#   define		    NL0	0000000
#   define		    NL1	0000400
#   define		VTDLY	0001000
#   define		    VT0	0000000
#   define		    VT1	0001000
#   define		TABDLY	0006000
#   define		   TAB0	0000000
#   define		   TAB1	0002000
#   define		   TAB2	0004000
#   define		   TAB3	0006000
#   define		CRDLY	0030000
#   define		    CR0	0000000
#   define		    CR1	0010000
#   define		    CR2	0020000
#   define		    CR3	0030000
#   define		FFDLY	0040000
#   define		    FF0	0000000
#   define		    FF1	0040000
#   define		BSDLY	0100000
#   define		    BS0	0000000
#   define		    BS1	0100000

    tcflag_t	c_cflag;
#   define 		CIGNORE 0000001 		/* BRC 22-Jun-2017 */
#   define		CSIZE	0000060
#   define		    CS5	0000000
#   define		    CS6	0000020
#   define		    CS7	0000040
#   define		    CS8	0000060
#   define		CSTOPB	0000100
#   define		CREAD	0000200
#   define		PARENB	0000400
#   define		PARODD	0001000
#   define		HUPCL	0002000
#   define		CLOCAL	0004000
#   define 		CRTSCTS 0200000			/* BRC 22-Jun-2017 */
#   define 		CDTRCTS 0400000			/* BRC 22-Jun-2017 */
#   define 		MDMBUF  4000000			/* BRC 22-Jun-2017 */

    tcflag_t	c_lflag;
#ifdef	__USE_BSD
# define ECHOKE	(1 << 0)	/* Visual erase for KILL.  */
#endif
#define	_ECHOE	(1 << 1)	/* Visual erase for ERASE.  */
#define	ECHOE	_ECHOE
#define	_ECHOK	(1 << 2)	/* Echo NL after KILL.  */
#define	ECHOK	_ECHOK
#define	_ECHO	(1 << 3)	/* Enable echo.  */
#define	ECHO	_ECHO
#define	_ECHONL	(1 << 4)	/* Echo NL even if ECHO is off.  */
#define	ECHONL	_ECHONL
#ifdef	__USE_BSD
# define ECHOPRT	(1 << 5)	/* Hardcopy visual erase.  */
# define ECHOCTL	(1 << 6)	/* Echo control characters as ^X.  */
#endif
#define	_ISIG	(1 << 7)	/* Enable signals.  */
#define	ISIG	_ISIG
#define	_ICANON	(1 << 8)	/* Do erase and kill processing.  */
#define	ICANON	_ICANON
#ifdef	__USE_BSD
# define ALTWERASE (1 << 9)	/* Alternate WERASE algorithm.  */
#endif
#define	_IEXTEN	(1 << 10)	/* Enable DISCARD and LNEXT.  */
#define	IEXTEN	_IEXTEN
#define	EXTPROC	(1 << 11)	/* External processing.  */
#define	_TOSTOP	(1 << 22)	/* Send SIGTTOU for background output.  */
#define	TOSTOP	_TOSTOP
#ifdef	__USE_BSD
# define FLUSHO	(1 << 23)	/* Output being flushed (state).  */
# define NOKERNINFO (1 << 25)	/* Disable VSTATUS.  */
# define PENDIN	(1 << 29)	/* Retype pending input (state).  */
#endif
#define	_NOFLSH	(1 << 31)	/* Disable flush after interrupt.  */
#define	NOFLSH	_NOFLSH
#if 0
#   define		ISIG	0000001
#   define		ICANON	0000002
#   define		XCASE	0000004
#   define		ECHO	0000010
#   define		ECHOE	0000020
#   define		ECHOK	0000040
#   define		ECHONL	0000100
#   define		NOFLSH	0000200
#   define		TOSTOP	(0x40<<16)
#   define		IEXTEN	(0x80<<16)
#endif

    speed_t	c_ispeed;
    speed_t	c_ospeed;
#   define		    B0	 0
#   define		   B50	 1
#   define		   B75	 2
#   define		  B110	 3
#   define		  B134	 4
#   define		  B150	 5
#   define		  B200 255	/* Not available on VAX/VMS	    */
#   define		  B300	 6
#   define		  B600	 7
#   define		 B1200	 8
#   define		 B1800	 9
#   define		 B2000	10	/* Non-standard speed		    */
#   define		 B2400	11
#   define		 B3600	12	/* Non-standard speed		    */
#   define		 B4800	13
#   define		 B7200	14	/* Non-standard speed		    */
#   define		 B9600	15
#   define		B19200	16
#   define		B38400	17

    cc_t	c_cc [NCCS];
} ;

# define TCSANOW	0
# define TCSADRAIN	1
# define TCSAFLUSH	2

# define TCIFLUSH	0
# define TCOFLUSH	1
# define TCIOFLUSH	2

# define TCOOFF		0
# define TCOON		1
# define TCIOFF		2
# define TCION		3

# if __INITIAL_POINTER_SIZE > 0
#   pragma __pointer_size __long
# endif

int tcgetattr       (int __fd, struct termios * __termios_p);
int tcsetattr       (int __fd, int __opt, const struct termios * __termios_p);
int tcsendbreak     (int __fd, int __duration);
int tcdrain         (int __fd);
int tcflush         (int __fd, int __queue);
int tcflow          (int __fd, int __action);

# if __cplusplus
    inline speed_t cfgetispeed (const struct termios * __termios_p)
    {
	return __termios_p->c_ispeed;
    }
    inline speed_t cfgetospeed (const struct termios * __termios_p)
    {
	return __termios_p->c_ospeed;
    }
    inline int cfsetispeed (struct termios * __termios_p, speed_t __speed)
    {
	__termios_p->c_ispeed = __speed;  return 0;
    }
    inline int cfsetospeed (struct termios * __termios_p, speed_t __speed)
    {
	__termios_p->c_ospeed = __speed;  return 0;
    }
# else
    speed_t cfgetispeed (const struct termios * __termios_p);
    speed_t cfgetospeed (const struct termios * __termios_p);
    int cfsetispeed     (struct termios * __termios_p, speed_t __speed);
    int cfsetospeed     (struct termios * __termios_p, speed_t __speed);
#   define cfgetispeed(tp)	((tp)->c_ispeed)
#   define cfgetospeed(tp)	((tp)->c_ospeed)
#   define cfsetispeed(tp,sp)	((tp)->c_ispeed=(sp), 0)
#   define cfsetospeed(tp,sp)	((tp)->c_ospeed=(sp), 0)
# endif

# if __cplusplus				    /* EPILOGUE version X-5 */
}
# endif
# pragma __environment __restore

# endif		/* _TERMIOS_LOADED */

