#ifndef OS_H
#define OS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXT_NAME ".cxc"
#define BUF_SIZE 4096	// must be 16x

// filename max length
#define NAME_MAX 256
// path max length
#define PATH_MAX 4096

#define RET_END 4
#define RET_SKIP 3
#define RET_YES 2
#define RET_NO 1
#define RET_ERROR 0

#define X_NONE 0		// 0x00
#define X_ENCRYPT 1 	// 0x01
#define X_DECRYPT 2 	// 0x02
#define X_CHECK 4		// 0x08
#define X_MD5SUM 8		// 0x10
#define X_UPDATE 16 	// 0x04

	
/**
	The operating system, must be one of: (OS_x)

	MACX		- Mac OS X
	MAC9		- Mac OS 9
	MSDOS		- MS-DOS and Windows
	OS2			- OS/2
	OS2EMX		- XFree86 on OS/2 (not PM)
	WIN32		- Win32 (Windows 95/98/ME and Windows NT/2000/XP)
	CYGWIN		- Cygwin
	SOLARIS		- Sun Solaris
	HPUX		- HP-UX
	ULTRIX		- DEC Ultrix
	LINUX		- Linux
	FREEBSD		- FreeBSD
	NETBSD		- NetBSD
	OPENBSD		- OpenBSD
	BSDI		- BSD/OS
	IRIX		- SGI Irix
	OSF			- HP Tru64 UNIX
	SCO			- SCO OpenServer 5
	UNIXWARE	- UnixWare 7, Open UNIX 8
	AIX			- AIX
	HURD		- GNU Hurd
	DGUX		- DG/UX
	RELIANT		- Reliant UNIX
	DYNIX		- DYNIX/ptx
	QNX			- QNX
	QNX6		- QNX RTP 6.1
	LYNX		- LynxOS
	BSD4		- Any BSD 4.4 system
	UNIX		- Any UNIX BSD/SYSV system
*/

#if defined(__APPLE__) && defined(__GNUC__)
#	define OS_MACX
#elif defined(__MACOSX__)
#	define OS_MACX
#elif defined(macintosh)
#	define OS_MAC9
#elif defined(__CYGWIN__)
#	define OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#	define OS_MSDOS
#elif defined(__OS2__)
#	if defined(__EMX__)
#		define OS_OS2EMX
#	else
#		define OS_OS2
#	endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#	define OS_WIN32
#	define OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#	define OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#	define OS_WIN32
#elif defined(__sun) || defined(sun)
#	define OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#	define OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#	define OS_ULTRIX
#elif defined(sinix)
#	define OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#	define OS_LINUX
#elif defined(__FreeBSD__)
#	define OS_FREEBSD
#	define OS_BSD4
#elif defined(__NetBSD__)
#	define OS_NETBSD
#	define OS_BSD4
#elif defined(__OpenBSD__)
#	define OS_OPENBSD
#	define OS_BSD4
#elif defined(__bsdi__)
#	define OS_BSDI
#	define OS_BSD4
#elif defined(__sgi)
#	define OS_IRIX
#elif defined(__osf__)
#	define OS_OSF
#elif defined(_AIX)
#	define OS_AIX
#elif defined(__Lynx__)
#	define OS_LYNX
#elif defined(__GNU_HURD__)
#	define OS_HURD
#elif defined(__DGUX__)
#	define OS_DGUX
#elif defined(__QNXNTO__)
#	define OS_QNX6
#elif defined(__QNX__)
#	define OS_QNX
#elif defined(_SEQUENT_)
#	define OS_DYNIX
#elif defined(_SCO_DS)			/* SCO OpenServer 5 + GCC */
#	define OS_SCO
#elif defined(__USLC__)			/* all SCO platforms + UDK or OUDK */
#	define OS_UNIXWARE
#	define OS_UNIXWARE7
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#	define OS_UNIXWARE
#	define OS_UNIXWARE7
#else
#	error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(OS_MAC9) || defined(OS_MACX)
#	define OS_MAC
#endif

#if defined(OS_MAC9) || defined(OS_MSDOS) || defined(OS_OS2) || defined(OS_WIN32) || defined(OS_WIN64)
#	undef OS_UNIX
#elif !defined(OS_UNIX)
#	define OS_UNIX
#endif

#if defined(OS_WIN32) || defined(OS_WIN64) || defined(OS_MSDOS) || defined(OS_CYGWIN)
#	define XCP_WIN
#else 
#	define XCP_LINUX
#endif

#endif
