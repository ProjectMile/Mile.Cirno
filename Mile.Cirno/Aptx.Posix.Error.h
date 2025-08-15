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
// Linux, and Windows CRT use the same error codes inherited from Version 7 Unix.

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

// Resource temporarily unavailable
#define APTX_EAGAIN 11

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

#endif // !APTX_POSIX_ERROR
