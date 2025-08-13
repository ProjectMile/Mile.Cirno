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

// This file is generated from the following source via regex replacement:
// https://github.com/jasonwhite/syscalls/blob/0.6.18/src/errno/generated.rs
// Use Visual Studio Code with the regex replacement:
// Find: ^        (.+)\((.+)\) = "(.+)",$
// Replace: \n// $3\n#define LX_$1 $2

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

// Bad file number
#define LX_EBADF 9

// No child processes
#define LX_ECHILD 10

// Try again
#define LX_EAGAIN 11

// Out of memory
#define LX_ENOMEM 12

// Permission denied
#define LX_EACCES 13

// Bad address
#define LX_EFAULT 14

// Block device required
#define LX_ENOTBLK 15

// Device or resource busy
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

// File table overflow
#define LX_ENFILE 23

// Too many open files
#define LX_EMFILE 24

// Not a typewriter
#define LX_ENOTTY 25

// Text file busy
#define LX_ETXTBSY 26

// File too large
#define LX_EFBIG 27

// No space left on device
#define LX_ENOSPC 28

// Illegal seek
#define LX_ESPIPE 29

// Read-only file system
#define LX_EROFS 30

// Too many links
#define LX_EMLINK 31

// Broken pipe
#define LX_EPIPE 32

// Math argument out of domain of func
#define LX_EDOM 33

// Math result not representable
#define LX_ERANGE 34

// Resource deadlock would occur
#define LX_EDEADLK 35

// File name too long
#define LX_ENAMETOOLONG 36

// No record locks available
#define LX_ENOLCK 37

// Invalid system call number
#define LX_ENOSYS 38

// Directory not empty
#define LX_ENOTEMPTY 39

// Too many symbolic links encountered
#define LX_ELOOP 40

// No message of desired type
#define LX_ENOMSG 42

// Identifier removed
#define LX_EIDRM 43

// Channel number out of range
#define LX_ECHRNG 44

// Level 2 not synchronized
#define LX_EL2NSYNC 45

// Level 3 halted
#define LX_EL3HLT 46

// Level 3 reset
#define LX_EL3RST 47

// Link number out of range
#define LX_ELNRNG 48

// Protocol driver not attached
#define LX_EUNATCH 49

// No CSI structure available
#define LX_ENOCSI 50

// Level 2 halted
#define LX_EL2HLT 51

// Invalid exchange
#define LX_EBADE 52

// Invalid request descriptor
#define LX_EBADR 53

// Exchange full
#define LX_EXFULL 54

// No anode
#define LX_ENOANO 55

// Invalid request code
#define LX_EBADRQC 56

// Invalid slot
#define LX_EBADSLT 57

// Bad font file format
#define LX_EBFONT 59

// Device not a stream
#define LX_ENOSTR 60

// No data available
#define LX_ENODATA 61

// Timer expired
#define LX_ETIME 62

// Out of streams resources
#define LX_ENOSR 63

// Machine is not on the network
#define LX_ENONET 64

// Package not installed
#define LX_ENOPKG 65

// Object is remote
#define LX_EREMOTE 66

// Link has been severed
#define LX_ENOLINK 67

// Advertise error
#define LX_EADV 68

// Srmount error
#define LX_ESRMNT 69

// Communication error on send
#define LX_ECOMM 70

// Protocol error
#define LX_EPROTO 71

// Multihop attempted
#define LX_EMULTIHOP 72

// RFS specific error
#define LX_EDOTDOT 73

// Not a data message
#define LX_EBADMSG 74

// Value too large for defined data type
#define LX_EOVERFLOW 75

// Name not unique on network
#define LX_ENOTUNIQ 76

// File descriptor in bad state
#define LX_EBADFD 77

// Remote address changed
#define LX_EREMCHG 78

// Can not access a needed shared library
#define LX_ELIBACC 79

// Accessing a corrupted shared library
#define LX_ELIBBAD 80

// .lib section in a.out corrupted
#define LX_ELIBSCN 81

// Attempting to link in too many shared libraries
#define LX_ELIBMAX 82

// Cannot exec a shared library directly
#define LX_ELIBEXEC 83

// Illegal byte sequence
#define LX_EILSEQ 84

// Interrupted system call should be restarted
#define LX_ERESTART 85

// Streams pipe error
#define LX_ESTRPIPE 86

// Too many users
#define LX_EUSERS 87

// Socket operation on non-socket
#define LX_ENOTSOCK 88

// Destination address required
#define LX_EDESTADDRREQ 89

// Message too long
#define LX_EMSGSIZE 90

// Protocol wrong type for socket
#define LX_EPROTOTYPE 91

// Protocol not available
#define LX_ENOPROTOOPT 92

// Protocol not supported
#define LX_EPROTONOSUPPORT 93

// Socket type not supported
#define LX_ESOCKTNOSUPPORT 94

// Operation not supported on transport endpoint
#define LX_EOPNOTSUPP 95

// Protocol family not supported
#define LX_EPFNOSUPPORT 96

// Address family not supported by protocol
#define LX_EAFNOSUPPORT 97

// Address already in use
#define LX_EADDRINUSE 98

// Cannot assign requested address
#define LX_EADDRNOTAVAIL 99

// Network is down
#define LX_ENETDOWN 100

// Network is unreachable
#define LX_ENETUNREACH 101

// Network dropped connection because of reset
#define LX_ENETRESET 102

// Software caused connection abort
#define LX_ECONNABORTED 103

// Connection reset by peer
#define LX_ECONNRESET 104

// No buffer space available
#define LX_ENOBUFS 105

// Transport endpoint is already connected
#define LX_EISCONN 106

// Transport endpoint is not connected
#define LX_ENOTCONN 107

// Cannot send after transport endpoint shutdown
#define LX_ESHUTDOWN 108

// Too many references: cannot splice
#define LX_ETOOMANYREFS 109

// Connection timed out
#define LX_ETIMEDOUT 110

// Connection refused
#define LX_ECONNREFUSED 111

// Host is down
#define LX_EHOSTDOWN 112

// No route to host
#define LX_EHOSTUNREACH 113

// Operation already in progress
#define LX_EALREADY 114

// Operation now in progress
#define LX_EINPROGRESS 115

// Stale file handle
#define LX_ESTALE 116

// Structure needs cleaning
#define LX_EUCLEAN 117

// Not a XENIX named type file
#define LX_ENOTNAM 118

// No XENIX semaphores available
#define LX_ENAVAIL 119

// Is a named type file
#define LX_EISNAM 120

// Remote I/O error
#define LX_EREMOTEIO 121

// Quota exceeded
#define LX_EDQUOT 122

// No medium found
#define LX_ENOMEDIUM 123

// Wrong medium type
#define LX_EMEDIUMTYPE 124

// Operation Canceled
#define LX_ECANCELED 125

// Required key not available
#define LX_ENOKEY 126

// Key has expired
#define LX_EKEYEXPIRED 127

// Key has been revoked
#define LX_EKEYREVOKED 128

// Key was rejected by service
#define LX_EKEYREJECTED 129

// Owner died
#define LX_EOWNERDEAD 130

// State not recoverable
#define LX_ENOTRECOVERABLE 131

// Operation not possible due to RF-kill
#define LX_ERFKILL 132

// Memory page has hardware error
#define LX_EHWPOISON 133

// Restart syscall
#define LX_ERESTARTSYS 512

// Restart if no interrupt
#define LX_ERESTARTNOINTR 513

// restart if no handler..
#define LX_ERESTARTNOHAND 514

// No ioctl command
#define LX_ENOIOCTLCMD 515

// restart by calling sys_restart_syscall
#define LX_ERESTART_RESTARTBLOCK 516

// Driver requests probe retry
#define LX_EPROBE_DEFER 517

// open found a stale dentry
#define LX_EOPENSTALE 518

// Parameter not supported
#define LX_ENOPARAM 519

// Illegal NFS file handle
#define LX_EBADHANDLE 521

// Update synchronization mismatch
#define LX_ENOTSYNC 522

// Cookie is stale
#define LX_EBADCOOKIE 523

// Operation is not supported
#define LX_ENOTSUPP 524

// Buffer or request is too small
#define LX_ETOOSMALL 525

// An untranslatable error occurred
#define LX_ESERVERFAULT 526

// Type not supported by server
#define LX_EBADTYPE 527

// Request initiated, but will not complete before timeout
#define LX_EJUKEBOX 528

// iocb queued, will get completion event
#define LX_EIOCBQUEUED 529

// conflict with recalled state
#define LX_ERECALLCONFLICT 530

// NFS file lock reclaim refused
#define LX_ENOGRACE 531

#endif // !APTX_LINUX_ERROR
