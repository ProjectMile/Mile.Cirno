/*
 * PROJECT:    Application Portability Technology eXtensions
 * FILE:       Aptx.Posix.FileMode.h
 * PURPOSE:    Definition for POSIX File Mode Flags
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef APTX_POSIX_FILEMODE
#define APTX_POSIX_FILEMODE

// *****************************************************************************
// File type masks
//

// type of file mask
#define APTX_IFMT 0170000

// socket
#define APTX_IFSOCK 0140000

// symbolic link
#define APTX_IFLNK 0120000

// regular file
#define APTX_IFREG 0100000

// block special
#define APTX_IFBLK 0060000

// directory
#define APTX_IFDIR 0040000

// character special
#define APTX_IFCHR 0020000

// named pipe (fifo)
#define APTX_IFIFO 0010000

// set user id on execution
#define APTX_ISUID 0004000

// set group id on execution
#define APTX_ISGID 0002000

// save swapped text even after use
#define APTX_ISVTX 0001000

// *****************************************************************************
// File masks for owner
//

// RWX mask for owner
#define APTX_IRWXU 0000700

// R for owner
#define APTX_IRUSR 0000400

// W for owner
#define APTX_IWUSR 0000200

// X for owner
#define APTX_IXUSR 0000100

// *****************************************************************************
// File masks for group
//

// RWX mask for group
#define APTX_IRWXG 0000070

// R for group
#define APTX_IRGRP 0000040

// W for group
#define APTX_IWGRP 0000020

// X for group
#define APTX_IXGRP 0000010

// *****************************************************************************
// File masks for other
//

// RWX mask for other
#define APTX_IRWXO 0000007

// R for other
#define APTX_IROTH 0000004

// W for other
#define APTX_IWOTH 0000002

// X for other
#define APTX_IXOTH 0000001

#endif // !APTX_POSIX_FILEMODE
