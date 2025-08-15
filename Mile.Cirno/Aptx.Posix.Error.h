/*
 * PROJECT:    Application Portability Technology eXtensions
 * FILE:       Aptx.Posix.Error.h
 * PURPOSE:    Definition for POSIX Error Codes
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef APTX_POSIX_ERROR
#define APTX_POSIX_ERROR

// *****************************************************************************
// Version 7 Unix Error Codes
//

// As the current observations, all major POSIX-compliant environments like BSD,
// Linux, and Windows CRT use the same error codes inherited from Version 7
// Unix, except the value 11 is EDEADLK instead of EAGAIN in BSD.

// Operation not permitted
#define APTX_EPERM 1

// No such file or directory
#define APTX_ENOENT 2

// No such process
#define APTX_ESRCH 3

// Interrupted function call
#define APTX_EINTR 4

// Input/output error
#define APTX_EIO 5

// No such device or address
#define APTX_ENXIO 6

// Argument list too long
#define APTX_E2BIG 7

// Exec format error
#define APTX_ENOEXEC 8

// Bad file descriptor
#define APTX_EBADF 9

// No child processes
#define APTX_ECHILD 10

// Out of memory
#define APTX_ENOMEM 12

// Permission denied
#define APTX_EACCES 13

// Bad address
#define APTX_EFAULT 14

// Block device required
#define APTX_ENOTBLK 15

// Device or resource busy
#define APTX_EBUSY 16

// File exists
#define APTX_EEXIST 17

// Cross-device link
#define APTX_EXDEV 18

// No such device
#define APTX_ENODEV 19

// Not a directory
#define APTX_ENOTDIR 20

// Is a directory
#define APTX_EISDIR 21

// Invalid argument
#define APTX_EINVAL 22

// Too many open files in system
#define APTX_ENFILE 23

// Too many open files
#define APTX_EMFILE 24

// Inappropriate I/O control operation
#define APTX_ENOTTY 25

// Text file busy
#define APTX_ETXTBSY 26

// File too large
#define APTX_EFBIG 27

// No space left on device
#define APTX_ENOSPC 28

// Illegal seek
#define APTX_ESPIPE 29

// Read-only file system
#define APTX_EROFS 30

// Too many links
#define APTX_EMLINK 31

// Broken pipe
#define APTX_EPIPE 32

// Numerical argument out of domain
#define APTX_EDOM 33

// Result too large or too small
#define APTX_ERANGE 34

// *****************************************************************************
// BSD Specific Error Codes
//

// This file is generated from the following sources via regex replacement:
// https://github.com/freebsd/freebsd-src/blob/release/14.3.0/lib/libc/nls/C.msg
// https://github.com/NetBSD/src/blob/netbsd-10/lib/libc/nls/C.msg
// https://github.com/openbsd/src/blob/master/lib/libc/gen/errlist.c
// Use Visual Studio Code with the regex replacement for C.msg:
// - Find: ^\$ (.+)\n(\d+) (.+)$
// - Replace: \n// $3\n#define BSD_$1 $2
// Use Visual Studio Code with the regex replacement for errlist.c:
// - Find: ^\t"(.+)",(\t+)/\*(\s+)(\d+) - (.+) \*/$
// - Replace: \n// $1\n#define BSD_$5 $4

// Resource deadlock avoided
#define APTX_BSD_EDEADLK 11

// Resource temporarily unavailable
#define APTX_BSD_EAGAIN 35

// Resource temporarily unavailable
#define APTX_BSD_EWOULDBLOCK APTX_BSD_EAGAIN

// Operation now in progress
#define APTX_BSD_EINPROGRESS 36

// Operation already in progress
#define APTX_BSD_EALREADY 37

// Socket operation on non-socket
#define APTX_BSD_ENOTSOCK 38

// Destination address required
#define APTX_BSD_EDESTADDRREQ 39

// Message too long
#define APTX_BSD_EMSGSIZE 40

// Protocol wrong type for socket
#define APTX_BSD_EPROTOTYPE 41

// Protocol option not available
#define APTX_BSD_ENOPROTOOPT 42

// Protocol not supported
#define APTX_BSD_EPROTONOSUPPORT 43

// Socket type not supported
#define APTX_BSD_ESOCKTNOSUPPORT 44

// Operation not supported
#define APTX_BSD_EOPNOTSUPP 45

// Protocol family not supported
#define APTX_BSD_EPFNOSUPPORT 46

// Address family not supported by protocol family
#define APTX_BSD_EAFNOSUPPORT 47

// Address already in use
#define APTX_BSD_EADDRINUSE 48

// Can't assign requested address
#define APTX_BSD_EADDRNOTAVAIL 49

// Network is down
#define APTX_BSD_ENETDOWN 50

// Network is unreachable
#define APTX_BSD_ENETUNREACH 51

// Network dropped connection on reset
#define APTX_BSD_ENETRESET 52

// Software caused connection abort
#define APTX_BSD_ECONNABORTED 53

// Connection reset by peer
#define APTX_BSD_ECONNRESET 54

// No buffer space available
#define APTX_BSD_ENOBUFS 55

// Socket is already connected
#define APTX_BSD_EISCONN 56

// Socket is not connected
#define APTX_BSD_ENOTCONN 57

// Can't send after socket shutdown
#define APTX_BSD_ESHUTDOWN 58

// Too many references: can't splice
#define APTX_BSD_ETOOMANYREFS 59

// Operation timed out
#define APTX_BSD_ETIMEDOUT 60

// Connection refused
#define APTX_BSD_ECONNREFUSED 61

// Too many levels of symbolic links
#define APTX_BSD_ELOOP 62

// File name too long
#define APTX_BSD_ENAMETOOLONG 63

// Host is down
#define APTX_BSD_EHOSTDOWN 64

// No route to host
#define APTX_BSD_EHOSTUNREACH 65

// Directory not empty
#define APTX_BSD_ENOTEMPTY 66

// Too many processes
#define APTX_BSD_EPROCLIM 67

// Too many users
#define APTX_BSD_EUSERS 68

// Disk quota exceeded
#define APTX_BSD_EDQUOT 69

// Stale NFS file handle
#define APTX_BSD_ESTALE 70

// Too many levels of remote in path
#define APTX_BSD_EREMOTE 71

// RPC struct is bad
#define APTX_BSD_EBADRPC 72

// RPC version wrong
#define APTX_BSD_ERPCMISMATCH 73

// RPC program not available
#define APTX_BSD_EPROGUNAVAIL 74

// Program version wrong
#define APTX_BSD_EPROGMISMATCH 75

// Bad procedure for program
#define APTX_BSD_EPROCUNAVAIL 76

// No locks available
#define APTX_BSD_ENOLCK 77

// Function not implemented
#define APTX_BSD_ENOSYS 78

// Inappropriate file type or format
#define APTX_BSD_EFTYPE 79

// Authentication error
#define APTX_BSD_EAUTH 80

// Need authenticator
#define APTX_BSD_ENEEDAUTH 81

// Identifier removed
#define APTX_FREEBSD_EIDRM 82

// Identifier removed
#define APTX_NETBSD_EIDRM 82

// IPsec processing failure
#define APTX_OPENBSD_EIPSEC 82

// No message of desired type
#define APTX_FREEBSD_ENOMSG 83

// No message of desired type
#define APTX_NETBSD_ENOMSG 83

// Attribute not found
#define APTX_OPENBSD_ENOATTR 83

// Value too large to be stored in data type
#define APTX_FREEBSD_EOVERFLOW 84

// Value too large to be stored in data type
#define APTX_NETBSD_EOVERFLOW 84

// Illegal byte sequence
#define APTX_OPENBSD_EILSEQ 84

// Operation canceled
#define APTX_FREEBSD_ECANCELED 85

// Illegal byte sequence
#define APTX_NETBSD_EILSEQ 85

// No medium found
#define APTX_OPENBSD_ENOMEDIUM 85

// Illegal byte sequence
#define APTX_FREEBSD_EILSEQ 86

// Not supported
#define APTX_NETBSD_ENOTSUP 86

// Wrong medium type
#define APTX_OPENBSD_EMEDIUMTYPE 86

// Attribute not found
#define APTX_FREEBSD_ENOATTR 87

// Operation canceled
#define APTX_NETBSD_ECANCELED 87

// Value too large to be stored in data type
#define APTX_OPENBSD_EEOVERFLOW 87

// Programming error
#define APTX_FREEBSD_EDOOFUS 88

// Bad or Corrupt message
#define APTX_NETBSD_EBADMSG 88

// Operation canceled
#define APTX_OPENBSD_ECANCELED 88

// Bad message
#define APTX_FREEBSD_EBADMSG 89

// No message available
#define APTX_NETBSD_ENODATA 89

// Identifier removed
#define APTX_OPENBSD_EIDRM 89

// Multihop attempted
#define APTX_FREEBSD_EMULTIHOP 90

// No STREAM resources
#define APTX_NETBSD_ENOSR 90

// No message of desired type
#define APTX_OPENBSD_ENOMSG 90

// Link has been severed
#define APTX_FREEBSD_ENOLINK 91

// Not a STREAM
#define APTX_NETBSD_ENOSTR 91

// Not supported
#define APTX_OPENBSD_ENOTSUP 91

// Protocol error
#define APTX_FREEBSD_EPROTO 92

// STREAM ioctl timeout
#define APTX_NETBSD_ETIME 92

// Bad message
#define APTX_OPENBSD_EBADMSG 92

// Capabilities insufficient
#define APTX_FREEBSD_ENOTCAPABLE 93

// Attribute not found
#define APTX_NETBSD_ENOATTR 93

// State not recoverable
#define APTX_OPENBSD_ENOTRECOVERABLE 93

// Not permitted in capability mode
#define APTX_FREEBSD_ECAPMODE 94

// Multihop attempted
#define APTX_NETBSD_EMULTIHOP 94

// Previous owner died
#define APTX_OPENBSD_EOWNERDEAD 94

// State not recoverable
#define APTX_FREEBSD_ENOTRECOVERABLE 95

// Link has been severed
#define APTX_NETBSD_ENOLINK 95

// Protocol error
#define APTX_OPENBSD_EPROTO 95

// Previous owner died
#define APTX_FREEBSD_EOWNERDEAD 96

// Protocol error
#define APTX_NETBSD_EPROTO 96

// Integrity check failed
#define APTX_FREEBSD_EINTEGRITY 97

// Previous owner died
#define APTX_NETBSD_EOWNERDEAD 97

// State not recoverable
#define APTX_NETBSD_ENOTRECOVERABLE 98

// *****************************************************************************
// Linux Specific Error Codes
//

// This file is generated from the following source via regex replacement:
// https://github.com/jasonwhite/syscalls/blob/0.6.18/src/errno/generated.rs
// Use Visual Studio Code with the regex replacement:
// - Find: ^        (.+)\((.+)\) = "(.+)",$
// - Replace: \n// $3\n#define LX_$1 $2

// Try again
#define APTX_LINUX_EAGAIN 11

// Resource deadlock would occur
#define APTX_LINUX_EDEADLK 35

// File name too long
#define APTX_LINUX_ENAMETOOLONG 36

// No record locks available
#define APTX_LINUX_ENOLCK 37

// Invalid system call number
#define APTX_LINUX_ENOSYS 38

// Directory not empty
#define APTX_LINUX_ENOTEMPTY 39

// Too many symbolic links encountered
#define APTX_LINUX_ELOOP 40

// Operation would block
#define APTX_LINUX_EWOULDBLOCK APTX_LINUX_EAGAIN

// No message of desired type
#define APTX_LINUX_ENOMSG 42

// Identifier removed
#define APTX_LINUX_EIDRM 43

// Channel number out of range
#define APTX_LINUX_ECHRNG 44

// Level 2 not synchronized
#define APTX_LINUX_EL2NSYNC 45

// Level 3 halted
#define APTX_LINUX_EL3HLT 46

// Level 3 reset
#define APTX_LINUX_EL3RST 47

// Link number out of range
#define APTX_LINUX_ELNRNG 48

// Protocol driver not attached
#define APTX_LINUX_EUNATCH 49

// No CSI structure available
#define APTX_LINUX_ENOCSI 50

// Level 2 halted
#define APTX_LINUX_EL2HLT 51

// Invalid exchange
#define APTX_LINUX_EBADE 52

// Invalid request descriptor
#define APTX_LINUX_EBADR 53

// Exchange full
#define APTX_LINUX_EXFULL 54

// No anode
#define APTX_LINUX_ENOANO 55

// Invalid request code
#define APTX_LINUX_EBADRQC 56

// Invalid slot
#define APTX_LINUX_EBADSLT 57

// Resource deadlock would occur
#define APTX_LINUX_EDEADLOCK APTX_LINUX_EDEADLK

// Bad font file format
#define APTX_LINUX_EBFONT 59

// Device not a stream
#define APTX_LINUX_ENOSTR 60

// No data available
#define APTX_LINUX_ENODATA 61

// Timer expired
#define APTX_LINUX_ETIME 62

// Out of streams resources
#define APTX_LINUX_ENOSR 63

// Machine is not on the network
#define APTX_LINUX_ENONET 64

// Package not installed
#define APTX_LINUX_ENOPKG 65

// Object is remote
#define APTX_LINUX_EREMOTE 66

// Link has been severed
#define APTX_LINUX_ENOLINK 67

// Advertise error
#define APTX_LINUX_EADV 68

// Srmount error
#define APTX_LINUX_ESRMNT 69

// Communication error on send
#define APTX_LINUX_ECOMM 70

// Protocol error
#define APTX_LINUX_EPROTO 71

// Multihop attempted
#define APTX_LINUX_EMULTIHOP 72

// RFS specific error
#define APTX_LINUX_EDOTDOT 73

// Not a data message
#define APTX_LINUX_EBADMSG 74

// Value too large for defined data type
#define APTX_LINUX_EOVERFLOW 75

// Name not unique on network
#define APTX_LINUX_ENOTUNIQ 76

// File descriptor in bad state
#define APTX_LINUX_EBADFD 77

// Remote address changed
#define APTX_LINUX_EREMCHG 78

// Can not access a needed shared library
#define APTX_LINUX_ELIBACC 79

// Accessing a corrupted shared library
#define APTX_LINUX_ELIBBAD 80

// .lib section in a.out corrupted
#define APTX_LINUX_ELIBSCN 81

// Attempting to link in too many shared libraries
#define APTX_LINUX_ELIBMAX 82

// Cannot exec a shared library directly
#define APTX_LINUX_ELIBEXEC 83

// Illegal byte sequence
#define APTX_LINUX_EILSEQ 84

// Interrupted system call should be restarted
#define APTX_LINUX_ERESTART 85

// Streams pipe error
#define APTX_LINUX_ESTRPIPE 86

// Too many users
#define APTX_LINUX_EUSERS 87

// Socket operation on non-socket
#define APTX_LINUX_ENOTSOCK 88

// Destination address required
#define APTX_LINUX_EDESTADDRREQ 89

// Message too long
#define APTX_LINUX_EMSGSIZE 90

// Protocol wrong type for socket
#define APTX_LINUX_EPROTOTYPE 91

// Protocol not available
#define APTX_LINUX_ENOPROTOOPT 92

// Protocol not supported
#define APTX_LINUX_EPROTONOSUPPORT 93

// Socket type not supported
#define APTX_LINUX_ESOCKTNOSUPPORT 94

// Operation not supported on transport endpoint
#define APTX_LINUX_EOPNOTSUPP 95

// Protocol family not supported
#define APTX_LINUX_EPFNOSUPPORT 96

// Address family not supported by protocol
#define APTX_LINUX_EAFNOSUPPORT 97

// Address already in use
#define APTX_LINUX_EADDRINUSE 98

// Cannot assign requested address
#define APTX_LINUX_EADDRNOTAVAIL 99

// Network is down
#define APTX_LINUX_ENETDOWN 100

// Network is unreachable
#define APTX_LINUX_ENETUNREACH 101

// Network dropped connection because of reset
#define APTX_LINUX_ENETRESET 102

// Software caused connection abort
#define APTX_LINUX_ECONNABORTED 103

// Connection reset by peer
#define APTX_LINUX_ECONNRESET 104

// No buffer space available
#define APTX_LINUX_ENOBUFS 105

// Transport endpoint is already connected
#define APTX_LINUX_EISCONN 106

// Transport endpoint is not connected
#define APTX_LINUX_ENOTCONN 107

// Cannot send after transport endpoint shutdown
#define APTX_LINUX_ESHUTDOWN 108

// Too many references: cannot splice
#define APTX_LINUX_ETOOMANYREFS 109

// Connection timed out
#define APTX_LINUX_ETIMEDOUT 110

// Connection refused
#define APTX_LINUX_ECONNREFUSED 111

// Host is down
#define APTX_LINUX_EHOSTDOWN 112

// No route to host
#define APTX_LINUX_EHOSTUNREACH 113

// Operation already in progress
#define APTX_LINUX_EALREADY 114

// Operation now in progress
#define APTX_LINUX_EINPROGRESS 115

// Stale file handle
#define APTX_LINUX_ESTALE 116

// Structure needs cleaning
#define APTX_LINUX_EUCLEAN 117

// Not a XENIX named type file
#define APTX_LINUX_ENOTNAM 118

// No XENIX semaphores available
#define APTX_LINUX_ENAVAIL 119

// Is a named type file
#define APTX_LINUX_EISNAM 120

// Remote I/O error
#define APTX_LINUX_EREMOTEIO 121

// Quota exceeded
#define APTX_LINUX_EDQUOT 122

// No medium found
#define APTX_LINUX_ENOMEDIUM 123

// Wrong medium type
#define APTX_LINUX_EMEDIUMTYPE 124

// Operation Canceled
#define APTX_LINUX_ECANCELED 125

// Required key not available
#define APTX_LINUX_ENOKEY 126

// Key has expired
#define APTX_LINUX_EKEYEXPIRED 127

// Key has been revoked
#define APTX_LINUX_EKEYREVOKED 128

// Key was rejected by service
#define APTX_LINUX_EKEYREJECTED 129

// Owner died
#define APTX_LINUX_EOWNERDEAD 130

// State not recoverable
#define APTX_LINUX_ENOTRECOVERABLE 131

// Operation not possible due to RF-kill
#define APTX_LINUX_ERFKILL 132

// Memory page has hardware error
#define APTX_LINUX_EHWPOISON 133

// Restart syscall
#define APTX_LINUX_ERESTARTSYS 512

// Restart if no interrupt
#define APTX_LINUX_ERESTARTNOINTR 513

// restart if no handler..
#define APTX_LINUX_ERESTARTNOHAND 514

// No ioctl command
#define APTX_LINUX_ENOIOCTLCMD 515

// restart by calling sys_restart_syscall
#define APTX_LINUX_ERESTART_RESTARTBLOCK 516

// Driver requests probe retry
#define APTX_LINUX_EPROBE_DEFER 517

// open found a stale dentry
#define APTX_LINUX_EOPENSTALE 518

// Parameter not supported
#define APTX_LINUX_ENOPARAM 519

// Illegal NFS file handle
#define APTX_LINUX_EBADHANDLE 521

// Update synchronization mismatch
#define APTX_LINUX_ENOTSYNC 522

// Cookie is stale
#define APTX_LINUX_EBADCOOKIE 523

// Operation is not supported
#define APTX_LINUX_ENOTSUPP 524

// Buffer or request is too small
#define APTX_LINUX_ETOOSMALL 525

// An untranslatable error occurred
#define APTX_LINUX_ESERVERFAULT 526

// Type not supported by server
#define APTX_LINUX_EBADTYPE 527

// Request initiated, but will not complete before timeout
#define APTX_LINUX_EJUKEBOX 528

// iocb queued, will get completion event
#define APTX_LINUX_EIOCBQUEUED 529

// conflict with recalled state
#define APTX_LINUX_ERECALLCONFLICT 530

// NFS file lock reclaim refused
#define APTX_LINUX_ENOGRACE 531

// *****************************************************************************
// Windows Specific Error Codes
//

// Resource temporarily unavailable
#define APTX_WINDOWS_EAGAIN 11

#endif // !APTX_POSIX_ERROR
