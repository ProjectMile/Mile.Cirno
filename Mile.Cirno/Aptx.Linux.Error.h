/*
 * PROJECT:    Application Portability Technology eXtensions
 * FILE:       Aptx.Linux.Error.h
 * PURPOSE:    Definition for Linux Error Codes
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef APTX_LINUX_ERROR
#define APTX_LINUX_ERROR

 // References:
 // - https://github.com/freebsd/freebsd-src/tree/release/14.3.0
 //   - /sys/compat/linux/linux_errno.h
 //   - /sys/compat/linuxkpi/common/include/linux/errno.h
 // - https://git.musl-libc.org/cgit/musl/tree
 //   - /src/errno/__strerror.h

 // Operation not permitted
#define LX_EPERM 1
// No such file or directory
#define LX_ENOENT 2
// No such process
#define LX_ESRCH 3
// Interrupted system call
#define LX_EINTR 4
// I/O error
#define LX_EIO 5
// No such device or address
#define LX_ENXIO 6
// Argument list too long
#define LX_E2BIG 7
// Exec format error
#define LX_ENOEXEC 8
// Bad file descriptor
#define LX_EBADF 9
// No child process
#define LX_ECHILD 10
// Resource temporarily unavailable
#define LX_EAGAIN 11
// Out of memory
#define LX_ENOMEM 12
// Permission denied
#define LX_EACCES 13
// Bad address
#define LX_EFAULT 14
// Block device required
#define LX_ENOTBLK 15
// Resource busy
#define LX_EBUSY 16
// File exists
#define LX_EEXIST 17
// Cross-device link
#define LX_EXDEV 18
// No such device
#define LX_ENODEV 19
// Not a directory
#define LX_ENOTDIR 20
// Is a directory
#define LX_EISDIR 21
// Invalid argument
#define LX_EINVAL 22
// Too many open files in system
#define LX_ENFILE 23
// No file descriptors available
#define LX_EMFILE 24
// Not a tty
#define LX_ENOTTY 25
// Text file busy
#define LX_ETXTBSY 26
// File too large
#define LX_EFBIG 27
// No space left on device
#define LX_ENOSPC 28
// Invalid seek
#define LX_ESPIPE 29
// Read-only file system
#define LX_EROFS 30
// Too many links
#define LX_EMLINK 31
// Broken pipe
#define LX_EPIPE 32
// Domain error
#define LX_EDOM 33
// Result not representable
#define LX_ERANGE 34
// Resource deadlock would occur
#define LX_EDEADLK 35
// Filename too long
#define LX_ENAMETOOLONG 36
// No locks available
#define LX_ENOLCK 37
// Function not implemented
#define LX_ENOSYS 38
// Directory not empty
#define LX_ENOTEMPTY 39
// Symbolic link loop
#define LX_ELOOP 40
// Resource temporarily unavailable
#define LX_EWOULDBLOCK LX_EAGAIN
// No message of desired type
#define LX_ENOMSG 42
// Identifier removed
#define LX_EIDRM 43
#define LX_ECHRNG 44
#define LX_EL2NSYNC 45
#define LX_EL3HLT 46
#define LX_EL3RST 47
#define LX_ELNRNG 48
#define LX_EUNATCH 49
#define LX_ENOCSI 50
#define LX_EL2HLT 51
#define LX_EBADE 52
#define LX_EBADR 53
#define LX_EXFULL 54
#define LX_ENOANO 55
#define LX_EBADRQC 56
#define LX_EBADSLT 57
// Resource deadlock would occur
#define LX_EDEADLOCK LX_EDEADLK
#define LX_EBFONT 59
// Device not a stream
#define LX_ENOSTR 60
// No data available
#define LX_ENODATA 61
// Device timeout
#define LX_ENOTIME 62
// Out of streams resources
#define LX_ENOSR 63
#define LX_ENONET 64
#define LX_ENOPKG 65
#define LX_EREMOTE 66
// Link has been severed
#define LX_ENOLINK 67
#define LX_EADV 68
#define LX_ESRMNT 69
#define LX_ECOMM 70
// Protocol error
#define LX_EPROTO 71
// Multihop attempted
#define LX_EMULTIHOP 72
#define LX_EDOTDOT 73
// Bad message
#define LX_EBADMSG 74
// Value too large for data type
#define LX_EOVERFLOW 75
#define LX_ENOTUNIQ 76
// File descriptor in bad state
#define LX_EBADFD 77
#define LX_EREMCHG 78
#define LX_ELIBACC 79
#define LX_ELIBBAD 80
#define LX_ELIBSCN 81
#define LX_ELIBMAX 82
#define LX_ELIBEXEC 83
// Illegal byte sequence
#define LX_EILSEQ 84
#define LX_ERESTART 85
#define LX_ESTRPIPE 86
#define LX_EUSERS 87
// Not a socket
#define LX_ENOTSOCK 88
// Destination address required
#define LX_EDESTADDRREQ 89
// Message too large
#define LX_EMSGSIZE 90
// Protocol wrong type for socket
#define LX_EPROTOTYPE 91
// Protocol not available
#define LX_ENOPROTOOPT 92
// Protocol not supported
#define LX_EPROTONOTSUPPORT 93
// Socket type not supported
#define LX_ESOCKNOTSUPPORT 94
// Not supported
#define LX_EOPNOTSUPPORT 95
// Not supported
#define LX_ENOTSUP LX_EOPNOTSUPPORT
// Protocol family not supported
#define LX_EPFNOTSUPPORT 96
// Address family not supported by protocol
#define LX_EAFNOTSUPPORT 97
// Address in use
#define LX_EADDRINUSE 98
// Address not available
#define LX_EADDRNOTAVAIL 99
// Network is down
#define LX_ENETDOWN 100
// Network unreachable
#define LX_ENETUNREACH 101
// Connection reset by network
#define LX_ENETRESET 102
// Connection aborted
#define LX_ECONNABORTED 103
// Connection reset by peer
#define LX_ECONNRESET 104
// No buffer space available
#define LX_ENOBUFS 105
// Socket is connected
#define LX_EISCONN 106
// Socket not connected
#define LX_ENOTCONN 107
// Cannot send after socket shutdown
#define LX_ESHUTDOWN 108
#define LX_ETOOMANYREFS 109
// Operation timed out
#define LX_ETIMEDOUT 110
// Connection refused
#define LX_ECONNREFUSED 111
// Host is down
#define LX_EHOSTDOWN 112
// Host is unreachable
#define LX_EHOSTUNREACH 113
// Operation already in progress
#define LX_EALREADY 114
// Operation in progress
#define LX_EINPROGRESS 115
// Stale file handle
#define LX_ESTALE 116
#define LX_EUCLEAN 117
#define LX_ENOTNAM 118
#define LX_ENAVAIL 119
#define LX_EISNAM 120
// Remote I/O error
#define LX_EREMOTEIO 121
// Quota exceeded
#define LX_EDQUOT 122
// No medium found
#define LX_ENOMEDIUM 123
// Wrong medium type
#define LX_EMEDIUMTYPE 124
// Operation canceled
#define LX_ECANCELED 125
// Required key not available
#define LX_ENOKEY 126
// Key has expired
#define LX_EKEYEXPIRED 127
// Key has been revoked
#define LX_EKEYREVOKED 128
// Key was rejected by service
#define LX_EKEYREJECTED 129
// Previous owner died
#define LX_EOWNERDEAD 130
// State not recoverable
#define LX_ENOTRECOVERABLE 131
#define LX_ERFKILL 132
#define LX_EHWPOISON 133
#define LX_ERESTARTSYS 512
#define LX_ERESTARTNOINTR 513
#define LX_ERESTARTNOHAND 514
#define LX_ENOIOCTLCMD 515
#define LX_ERESTART_RESTARTBLOCK 516
#define LX_EPROBE_DEFER 517
#define LX_EOPENSTALE 518
#define LX_ENOPARAM 519

#endif // !APTX_LINUX_ERROR
