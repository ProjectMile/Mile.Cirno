/*
 * PROJECT:    Application Portability Technology eXtensions
 * FILE:       Aptx.Berkeley.Error.h
 * PURPOSE:    Definition for Berkeley Software Distribution (BSD) Error Codes
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef APTX_BSD_ERROR
#define APTX_BSD_ERROR

// This file is generated from the following sources via regex replacement:
// https://github.com/freebsd/freebsd-src/blob/release/14.3.0/lib/libc/nls/C.msg
// https://github.com/NetBSD/src/blob/netbsd-10/lib/libc/nls/C.msg
// https://github.com/openbsd/src/blob/master/lib/libc/gen/errlist.c
// Use Visual Studio Code with the regex replacement for C.msg:
// Find: ^\$ (.+)\n(\d+) (.+)$
// Replace: \n// $3\n#define BSD_$1 $2
// Use Visual Studio Code with the regex replacement for errlist.c:
// Find: ^\t"(.+)",(\t+)/\*(\s+)(\d+) - (.+) \*/$
// Replace: \n// $1\n#define BSD_$5 $4

// Operation not permitted
#define BSD_EPERM 1

// No such file or directory
#define BSD_ENOENT 2

// No such process
#define BSD_ESRCH 3

// Interrupted system call
#define BSD_EINTR 4

// Input/output error
#define BSD_EIO 5

// Device not configured
#define BSD_ENXIO 6

// Argument list too long
#define BSD_E2BIG 7

// Exec format error
#define BSD_ENOEXEC 8

// Bad file descriptor
#define BSD_EBADF 9

// No child processes
#define BSD_ECHILD 10

// Resource deadlock avoided
#define BSD_EDEADLK 11

// Cannot allocate memory
#define BSD_ENOMEM 12

// Permission denied
#define BSD_EACCES 13

// Bad address
#define BSD_EFAULT 14

// Block device required
#define BSD_ENOTBLK 15

// Device busy
#define BSD_EBUSY 16

// File exists
#define BSD_EEXIST 17

// Cross-device link
#define BSD_EXDEV 18

// Operation not supported by device
#define BSD_ENODEV 19

// Not a directory
#define BSD_ENOTDIR 20

// Is a directory
#define BSD_EISDIR 21

// Invalid argument
#define BSD_EINVAL 22

// Too many open files in system
#define BSD_ENFILE 23

// Too many open files
#define BSD_EMFILE 24

// Inappropriate ioctl for device
#define BSD_ENOTTY 25

// Text file busy
#define BSD_ETXTBSY 26

// File too large
#define BSD_EFBIG 27

// No space left on device
#define BSD_ENOSPC 28

// Illegal seek
#define BSD_ESPIPE 29

// Read-only file system
#define BSD_EROFS 30

// Too many links
#define BSD_EMLINK 31

// Broken pipe
#define BSD_EPIPE 32

// Numerical argument out of domain
#define BSD_EDOM 33

// Result too large or too small
#define BSD_ERANGE 34

// Resource temporarily unavailable
#define BSD_EAGAIN 35

// Resource temporarily unavailable
#define BSD_EWOULDBLOCK BSD_EAGAIN

// Operation now in progress
#define BSD_EINPROGRESS 36

// Operation already in progress
#define BSD_EALREADY 37

// Socket operation on non-socket
#define BSD_ENOTSOCK 38

// Destination address required
#define BSD_EDESTADDRREQ 39

// Message too long
#define BSD_EMSGSIZE 40

// Protocol wrong type for socket
#define BSD_EPROTOTYPE 41

// Protocol option not available
#define BSD_ENOPROTOOPT 42

// Protocol not supported
#define BSD_EPROTONOSUPPORT 43

// Socket type not supported
#define BSD_ESOCKTNOSUPPORT 44

// Operation not supported
#define BSD_EOPNOTSUPP 45

// Protocol family not supported
#define BSD_EPFNOSUPPORT 46

// Address family not supported by protocol family
#define BSD_EAFNOSUPPORT 47

// Address already in use
#define BSD_EADDRINUSE 48

// Can't assign requested address
#define BSD_EADDRNOTAVAIL 49

// Network is down
#define BSD_ENETDOWN 50

// Network is unreachable
#define BSD_ENETUNREACH 51

// Network dropped connection on reset
#define BSD_ENETRESET 52

// Software caused connection abort
#define BSD_ECONNABORTED 53

// Connection reset by peer
#define BSD_ECONNRESET 54

// No buffer space available
#define BSD_ENOBUFS 55

// Socket is already connected
#define BSD_EISCONN 56

// Socket is not connected
#define BSD_ENOTCONN 57

// Can't send after socket shutdown
#define BSD_ESHUTDOWN 58

// Too many references: can't splice
#define BSD_ETOOMANYREFS 59

// Operation timed out
#define BSD_ETIMEDOUT 60

// Connection refused
#define BSD_ECONNREFUSED 61

// Too many levels of symbolic links
#define BSD_ELOOP 62

// File name too long
#define BSD_ENAMETOOLONG 63

// Host is down
#define BSD_EHOSTDOWN 64

// No route to host
#define BSD_EHOSTUNREACH 65

// Directory not empty
#define BSD_ENOTEMPTY 66

// Too many processes
#define BSD_EPROCLIM 67

// Too many users
#define BSD_EUSERS 68

// Disk quota exceeded
#define BSD_EDQUOT 69

// Stale NFS file handle
#define BSD_ESTALE 70

// Too many levels of remote in path
#define BSD_EREMOTE 71

// RPC struct is bad
#define BSD_EBADRPC 72

// RPC version wrong
#define BSD_ERPCMISMATCH 73

// RPC program not available
#define BSD_EPROGUNAVAIL 74

// Program version wrong
#define BSD_EPROGMISMATCH 75

// Bad procedure for program
#define BSD_EPROCUNAVAIL 76

// No locks available
#define BSD_ENOLCK 77

// Function not implemented
#define BSD_ENOSYS 78

// Inappropriate file type or format
#define BSD_EFTYPE 79

// Authentication error
#define BSD_EAUTH 80

// Need authenticator
#define BSD_ENEEDAUTH 81

// Identifier removed
#define FREEBSD_EIDRM 82

// Identifier removed
#define NETBSD_EIDRM 82

// IPsec processing failure
#define OPENBSD_EIPSEC 82

// No message of desired type
#define FREEBSD_ENOMSG 83

// No message of desired type
#define NETBSD_ENOMSG 83

// Attribute not found
#define OPENBSD_ENOATTR 83

// Value too large to be stored in data type
#define FREEBSD_EOVERFLOW 84

// Value too large to be stored in data type
#define NETBSD_EOVERFLOW 84

// Illegal byte sequence
#define OPENBSD_EILSEQ 84

// Operation canceled
#define FREEBSD_ECANCELED 85

// Illegal byte sequence
#define NETBSD_EILSEQ 85

// No medium found
#define OPENBSD_ENOMEDIUM 85

// Illegal byte sequence
#define FREEBSD_EILSEQ 86

// Not supported
#define NETBSD_ENOTSUP 86

// Wrong medium type
#define OPENBSD_EMEDIUMTYPE 86

// Attribute not found
#define FREEBSD_ENOATTR 87

// Operation canceled
#define NETBSD_ECANCELED 87

// Value too large to be stored in data type
#define OPENBSD_EEOVERFLOW 87

// Programming error
#define FREEBSD_EDOOFUS 88

// Bad or Corrupt message
#define NETBSD_EBADMSG 88

// Operation canceled
#define OPENBSD_ECANCELED 88

// Bad message
#define FREEBSD_EBADMSG 89

// No message available
#define NETBSD_ENODATA 89

// Identifier removed
#define OPENBSD_EIDRM 89

// Multihop attempted
#define FREEBSD_EMULTIHOP 90

// No STREAM resources
#define NETBSD_ENOSR 90

// No message of desired type
#define OPENBSD_ENOMSG 90

// Link has been severed
#define FREEBSD_ENOLINK 91

// Not a STREAM
#define NETBSD_ENOSTR 91

// Not supported
#define OPENBSD_ENOTSUP 91

// Protocol error
#define FREEBSD_EPROTO 92

// STREAM ioctl timeout
#define NETBSD_ETIME 92

// Bad message
#define OPENBSD_EBADMSG 92

// Capabilities insufficient
#define FREEBSD_ENOTCAPABLE 93

// Attribute not found
#define NETBSD_ENOATTR 93

// State not recoverable
#define OPENBSD_ENOTRECOVERABLE 93

// Not permitted in capability mode
#define FREEBSD_ECAPMODE 94

// Multihop attempted
#define NETBSD_EMULTIHOP 94

// Previous owner died
#define OPENBSD_EOWNERDEAD 94

// State not recoverable
#define FREEBSD_ENOTRECOVERABLE 95

// Link has been severed
#define NETBSD_ENOLINK 95

// Protocol error
#define OPENBSD_EPROTO 95

// Previous owner died
#define FREEBSD_EOWNERDEAD 96

// Protocol error
#define NETBSD_EPROTO 96

// Integrity check failed
#define FREEBSD_EINTEGRITY 97

// Previous owner died
#define NETBSD_EOWNERDEAD 97

// State not recoverable
#define NETBSD_ENOTRECOVERABLE 98

#endif // !APTX_BSD_ERROR
