﻿/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Protocol.h
 * PURPOSE:   Definition for Plan 9 File System Protocol
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_CIRNO_PROTOCOL
#define MILE_CIRNO_PROTOCOL

#include <stdint.h>

/**
 * @brief The definition of Plan 9 File System Protocol Message Type.
 */
typedef enum _MILE_CIRNO_PROTOCOL_MESSAGE_TYPE
{
    // Structures Definitions (%Name%<%Type%>[%Length%])
    //
    // %Name%<Header>[%Length%]
    //   size[4] type[1] tag[2]
    // %Name%<String>[%Length%]
    //   length[2] string[length]
    // %Name%<Qid>[%Length%]
    //   type[1] version[4] path[8]
    // %Name%<DirectoryEntry>[%Length%]
    //   qid<Qid>[1] offset[8] type[1] name<String>[1]
    // %Name%<Stat>[%Length%]
    //   9P2000, 9P2000.L and 9P2000.W 
    //     size[2] type[2] dev[4] qid<Qid>[1] mode[4] atime[4] mtime[4]
    //     length[8] name[String][1] uid[String][1] gid[String][1]
    //     muid[String][1]
    //   9P2000.u
    //     size[2] type[2] dev[4] qid<Qid>[1] mode[4] atime[4] mtime[4]
    //     length[8] name[String][1] uid[String][1] gid[String][1]
    //     muid[String][1] extension[String][1] n_uid[4] n_gid[4] n_muid[4]
    // %Name%<WindowsDirectoryEntry>[%Length%]
    //   entry<DirectoryEntry>[1] stat<Stat>[1]

    /* 9P2000.L */

    // Illegal
    MileCirnoLinuxErrorRequestMessage = 6,
    // header<Header>[1] ecode[4]
    MileCirnoLinuxErrorResponseMessage,
    // header<Header>[1] fid[4]
    MileCirnoStatFsRequestMessage = 8,
    // header<Header>[1] type[4] bsize[4] blocks[8] bfree[8] bavail[8] files[8]
    // ffree[8] fsid[8] namelen[4]
    MileCirnoStatFsResponseMessage,
    // header<Header>[1] fid[4] flags[4]
    MileCirnoLinuxOpenRequestMessage = 12,
    // header<Header>[1] qid<Qid>[1] iounit[4]
    MileCirnoLinuxOpenResponseMessage,
    // header<Header>[1] fid[4] name<String>[1] flags[4] mode[4] gid[4]
    MileCirnoLinuxCreateRequestMessage = 14,
    // header<Header>[1] qid<Qid>[1] iounit[4]
    MileCirnoLinuxCreateResponseMessage,
    // header<Header>[1] fid[4] name<String>[1] symtgt<String>[1] gid[4]
    MileCirnoSymLinkRequestMessage = 16,
    // header<Header>[1] qid<Qid>[1]
    MileCirnoSymLinkResponseMessage,
    // header<Header>[1] dfid[4] name<String>[1] mode[4] major[4] minor[4]
    // gid[4]
    MileCirnoMkNodRequestMessage = 18,
    // header<Header>[1] qid<Qid>[1]
    MileCirnoMkNodResponseMessage,
    // header<Header>[1] fid[4] dfid[4] name<String>[1]
    MileCirnoRenameRequestMessage = 20,
    // header<Header>[1]
    MileCirnoRenameResponseMessage,
    // header<Header>[1] fid[4]
    MileCirnoReadLinkRequestMessage = 22,
    // header<Header>[1] target<String>[1]
    MileCirnoReadLinkResponseMessage,
    // header<Header>[1] fid[4] request_mask[8]
    MileCirnoGetAttrRequestMessage = 24,
    // header<Header>[1] valid[8] qid<Qid>[1] mode[4] uid[4] gid[4] nlink[8]
    // rdev[8] size[8] blksize[8] blocks[8] atime_sec[8] atime_nsec[8]
    // mtime_sec[8] mtime_nsec[8] ctime_sec[8] ctime_nsec[8] btime_sec[8]
    // btime_nsec[8] gen[8] data_version[8]
    MileCirnoGetAttrResponseMessage,
    // header<Header>[1] fid[4] valid[4] mode[4] uid[4] gid[4] size[8]
    // atime_sec[8] atime_nsec[8] mtime_sec[8] mtime_nsec[8]
    MileCirnoSetAttrRequestMessage = 26,
    // header<Header>[1]
    MileCirnoSetAttrResponseMessage,
    // header<Header>[1] fid[4] newfid[4] name<String>[1]
    MileCirnoXattrWalkRequestMessage = 30,
    // header<Header>[1] size[8]
    MileCirnoXattrWalkResponseMessage,
    // header<Header>[1] fid[4] name<String>[1] attr_size[8] flags[4]
    MileCirnoXattrCreateRequestMessage = 32,
    // header<Header>[1]
    MileCirnoXattrCreateResponseMessage,
    // header<Header>[1] fid[4] offset[8] count[4]
    MileCirnoReadDirRequestMessage = 40,
    // header<Header>[1] count[4] data<DirectoryEntry>[count]
    MileCirnoReadDirResponseMessage,
    // header<Header>[1] fid[4]
    MileCirnoFsyncRequestMessage = 50,
    // header<Header>[1]
    MileCirnoFsyncResponseMessage,
    // header<Header>[1] fid[4] type[1] flags[4] start[8] length[8] proc_id[4]
    // client_id<String>[1]
    MileCirnoLockRequestMessage = 52,
    // header<Header>[1] status[1]
    MileCirnoLockResponseMessage,
    // header<Header>[1] fid[4] type[1] start[8] length[8] proc_id[4]
    // client_id<String>[1]
    MileCirnoGetLockRequestMessage = 54,
    // header<Header>[1] type[1] start[8] length[8] proc_id[4]
    // client_id<String>[1]
    MileCirnoGetLockResponseMessage,
    // header<Header>[1] dfid[4] fid[4] name<String>[1]
    MileCirnoLinkRequestMessage = 70,
    // header<Header>[1]
    MileCirnoLinkResponseMessage,
    // header<Header>[1] dfid[4] name<String>[1] mode[4] gid[4]
    MileCirnoMkDirRequestMessage = 72,
    // header<Header>[1] qid<Qid>[1]
    MileCirnoMkDirResponseMessage,
    // header<Header>[1] olddirfid[4] oldname<String>[1] newdirfid[4]
    // newname<String>[1]
    MileCirnoRenameAtRequestMessage = 74,
    // header<Header>[1]
    MileCirnoRenameAtResponseMessage,
    // header<Header>[1] dirfd[4] name<String>[1] flags[4]
    MileCirnoUnlinkAtRequestMessage = 76,
    // header<Header>[1]
    MileCirnoUnlinkAtResponseMessage,

    /* 9P2000 */

    // header<Header>[1] msize[4] version<String>[1]
    MileCirnoVersionRequestMessage = 100,
    // header<Header>[1] msize[4] version<String>[1]
    MileCirnoVersionResponseMessage,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] afid[4] uname<String>[1] aname<String>[1]
    // 9P2000.L
    //   header<Header>[1] afid[4] uname<String>[1] aname<String>[1] n_uname[4]
    // 9P2000.W
    //   Not used
    MileCirnoAuthRequestMessage = 102,
    // Not used in 9P2000.W
    // header<Header>[1] aqid<Qid>[1]
    MileCirnoAuthResponseMessage,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] fid[4] afid[4] uname<String>[1] aname<String>[1]
    // 9P2000.L and 9P2000.W
    //   header<Header>[1] fid[4] afid[4] uname<String>[1] aname<String>[1]
    //   n_uname[4]
    MileCirnoAttachRequestMessage = 104,
    // header<Header>[1] qid<Qid>[1]
    MileCirnoAttachResponseMessage,
    // Illegal
    MileCirnoErrorRequestMessage = 106,
    // 9P2000
    //   header<Header>[1] ename<String>[1]
    // 9P2000.u
    //   header<Header>[1] ename<String>[1] errno[4]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoErrorResponseMessage,
    // header<Header>[1] oldtag[2]
    MileCirnoFlushRequestMessage = 108,
    // header<Header>[1]
    MileCirnoFlushResponseMessage,
    // header<Header>[1] fid[4] newfid[4] nwname[2] wname<String>[nwname]
    MileCirnoWalkRequestMessage = 110,
    // header<Header>[1] nwqid[2] wqid<Qid>[nwqid]
    MileCirnoWalkResponseMessage,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] fid[4] mode[1]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoOpenRequestMessage = 112,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] qid[13] iounit[4]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoOpenResponseMessage,
    // 9P2000
    //   header<Header>[1] fid[4] name<String>[1] perm[4] mode[1]
    // 9P2000.u
    //   header<Header>[1] fid[4] name<String>[1] perm[4] mode[1]
    //   extension<String>[1]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoCreateRequestMessage = 114,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] qid[13] iounit[4]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoCreateResponseMessage,
    // header<Header>[1] fid[4] offset[8] count[4]
    MileCirnoReadRequestMessage = 116,
    // header<Header>[1] count[4] data[count]
    MileCirnoReadResponseMessage,
    // header<Header>[1] fid[4] offset[8] count[4] data[count]
    MileCirnoWriteRequestMessage = 118,
    // header<Header>[1] count[4]
    MileCirnoWriteResponseMessage,
    // header<Header>[1] fid[4]
    MileCirnoClunkRequestMessage = 120,
    // header<Header>[1]
    MileCirnoClunkResponseMessage,
    // header<Header>[1] fid[4]
    MileCirnoRemoveRequestMessage = 122,
    // header<Header>[1]
    MileCirnoRemoveResponseMessage,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] fid[4]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoStatRequestMessage = 124,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] stat<Stat>[n]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoStatResponseMessage,
    // 9P2000 and 9P2000.u
    //   header<Header>[1] fid[4] stat<Stat>[n]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoWriteStatRequestMessage = 126,
    // 9P2000 and 9P2000.u
    //   header<Header>[1]
    // 9P2000.L and 9P2000.W
    //   Not used
    MileCirnoWriteStatResponseMessage,

    /* 9P2000.W */

    // Undocumented
    // header<Header>[1] fid[4] flags[4]
    MileCirnoAccessRequestMessage = 128,
    // Undocumented
    // header<Header>[1]
    MileCirnoAccessResponseMessage,
    // Undocumented
    // header<Header>[1] fid[4] offset[8] count[4]
    MileCirnoWindowsReadDirRequestMessage = 130,
    // Undocumented
    // header<Header>[1] count[4] data<WindowsDirectoryEntry>[count]
    MileCirnoWindowsReadDirResponseMessage,
    // Undocumented
    // header<Header>[1] (Unknown)
    MileCirnoWindowsOpenRequestMessage = 132,
    // Undocumented
    // header<Header>[1] (Unknown)
    MileCirnoWindowsOpenResponseMessage,

} MILE_CIRNO_PROTOCOL_MESSAGE_TYPE, *PMILE_CIRNO_PROTOCOL_MESSAGE_TYPE;

// @ref: https://github.com/freebsd/freebsd-src/blob/main/contrib/lib9p/fcall.h
// todo: add documented comments.

/*
 * Type code bits in (the first byte of) a qid.
 */
typedef enum _MILE_CRINO_PROTOCOL_QID_TYPE {
    MileCrinoProtocolQTypeDirectory         = 0x80,
    MileCrinoProtocolQTypeAppendOnlyFiles   = 0x40,
    MileCrinoProtocolQTypeExclusiveUseFiles = 0x20,
    MileCrinoProtocolQTypeMountedChannel    = 0x10,
    MileCrinoProtocolQTypeAuthenticationFile= 0x08,
    MileCrinoProtocolQTypeTemporaryFile     = 0x04,
    MileCrinoProtocolQTypeSymbolicLink      = 0x02,
    MileCrinoProtocolQTypePlainFile         = 0x00,

} MILE_CRINO_PROTOCOL_QID_TYPE, *PMILE_CRINO_PROTOCOL_QID_TYPE;

/*
 * Extra permission bits in create and file modes (stat).
 */
#define MILE_CRINO_IN_DIRECTORY_MODE 0x80000000
typedef enum _MILE_CRINO_PROTOCOL_DIRECTORY_MODE {
    MileCrinoProtocolDirectoryModeAppend            = 0x40000000,
    MileCrinoProtocolDirectoryModeExclude           = 0x20000000,
    MileCrinoProtocolDirectoryModeMount             = 0x10000000,
    MileCrinoProtocolDirectoryModeAuthenticationFile= 0x08000000,
    MileCrinoProtocolDirectoryModeTemporaryFile     = 0x04000000,
    MileCrinoProtocolDirectoryModeSymbolicLink      = 0x02000000,
    // 9p2000Protocol.U extension.
    MileCrinoProtocolDirectoryModeDevice            = 0x00800000,
    MileCrinoProtocolDirectoryModeNamedPipe         = 0x00200000,
    MileCrinoProtocolDirectoryModeSocket            = 0x00100000,
    MileCrinoProtocolDirectoryModeSetUID            = 0x00080000,
    MileCrinoProtocolDirectoryModeSetGID            = 0x00040000,

} MILE_CRINO_PROTOCOL_DIRECTORY_MODE, *PMILE_CRINO_PROTOCOL_DIRECTORY_MODE;

typedef enum _MILE_CRINO_PROTOCOL_OPEN_MODE {
    MileCrinoProtocolOpenModeRead           = 0,
    MileCrinoProtocolOpenModeWrite          = 1,
    MileCrinoProtocolOpenModeReadAndWrite   = 2,
    MileCrinoProtocolOpenModeExecute        = 3,
    MileCrinoProtocolOpenModeAccessModeMask = 3,
    MileCrinoProtocolOpenModeTruncate       = 16,
    MileCrinoProtocolOpenModeCloseOnExecute = 32,
    MileCrinoProtocolOpenModeRemoveOnClose  = 64,
    MileCrinoProtocolOpenModeDirectAccess   = 128,

} MILE_CRINO_PROTOCOL_OPEN_MODE, *PMILE_CRINO_PROTOCOL_OPEN_MODE;

// todo: find a more elegant way of naming:0)
typedef enum _MILE_CRINO_PROTOCOL_L_OPEN_TYPE {
    MileCrinoProtocolLOpenCreate                    = 000000100U,
    MileCrinoProtocolLOpenExclude                   = 000000200U,
    MileCrinoProtocolLOpenNoControllingTerminalType = 000000400U,
    MileCrinoProtocolLOpenTruncate                  = 000001000U,
    MileCrinoProtocolLOpenAppend                    = 000002000U,
    MileCrinoProtocolLOpenNoneBlock                 = 000004000U,
    MileCrinoProtocolLOpenDataSynchronize           = 000010000U,
    MileCrinoProtocolLOpenFileAsynchronize          = 000020000U,
    MileCrinoProtocolLOpenDirect                    = 000040000U,
    MileCrinoProtocolLOpenLargeFile                 = 000100000U,
    MileCrinoProtocolLOpenDirectory                 = 000200000U,
    MileCrinoProtocolLOpenNoFollow                  = 000400000U,
    MileCrinoProtocolLOpenNoAccessTime              = 001000000U,
    MileCrinoProtocolLOpenCloseOnExecute            = 002000000U,
    MileCrinoProtocolLOpenSynchronize               = 004000000U,
    MileCrinoProtocolLOpenPath                      = 010000000U,
    MileCrinoProtocolLOpenTemporaryFile             = 020000000U,

} MILE_CRINO_PROTOCOL_L_OPEN_TYPE, *PMILE_CRINO_PROTOCOL_L_OPEN_TYPE;

// protocol magic number?
#define MileCrinoProtocolFileSystemType 0x01021997
/* Fields in req->request_mask and reply->valid for Tgetattr, Rgetattr. */

typedef enum _MILE_CRINO_PROTOCOL_GET_ATTRIBUTE_FLAG {
    MileCrinoProtocolGetAttributeMode           = 0x00000001,
    MileCrinoProtocolGetAttributeNLink          = 0x00000002,
    MileCrinoProtocolGetAttributeUID            = 0x00000004,
    MileCrinoProtocolGetAttributeGID            = 0x00000008,
    MileCrinoProtocolGetAttributeRawDevice      = 0x00000010,
    MileCrinoProtocolGetAttributeAccessTime     = 0x00000020,
    MileCrinoProtocolGetAttributeModifiedTime   = 0x00000040,
    MileCrinoProtocolGetAttributeChangeTime     = 0x00000080,
    MileCrinoProtocolGetAttributeInodeNumber    = 0x00000100,
    MileCrinoProtocolGetAttributeSize           = 0x00000200,
    MileCrinoProtocolGetAttributeBlocks         = 0x00000400,
    /* everything up to and including BLOCKS is BASIC */
    MileCrinoProtocolGetAttributeBasic =
        MileCrinoProtocolGetAttributeMode           |
        MileCrinoProtocolGetAttributeNLink          |
        MileCrinoProtocolGetAttributeUID            |
        MileCrinoProtocolGetAttributeGID            |
        MileCrinoProtocolGetAttributeRawDevice      |
        MileCrinoProtocolGetAttributeAccessTime     |
        MileCrinoProtocolGetAttributeModifiedTime   |
        MileCrinoProtocolGetAttributeChangeTime     |
        MileCrinoProtocolGetAttributeInodeNumber    |
        MileCrinoProtocolGetAttributeSize           |
        MileCrinoProtocolGetAttributeBlocks,
    MileCrinoProtocolGetAttributeBirthTime  = 0x00000800,
    MileCrinoProtocolGetAttributeGenration  = 0x00001000,
    MileCrinoProtocolGetAttributeDataVersion= 0x00002000,
    MileCrinoGetAttrributeAll =
        MileCrinoProtocolGetAttributeBasic      |
        MileCrinoProtocolGetAttributeBirthTime  |
        MileCrinoProtocolGetAttributeGenration  |
        MileCrinoProtocolGetAttributeDataVersion
} MILE_CRINO_PROTOCOL_GET_ATTRIBUTE_FLAG, *PMILE_CRINO_PROTOCOL_GET_ATTRIBUTE_FLAG;


/* Fields in req->valid for Tsetattr. */
typedef enum _MILE_CRINO_PROTOCOL_SET_ATTRIBUTE_FLAG {
    // todo
    MileCrinoProtocolSetAttributeMode           = 0x00000001,
    MileCrinoProtocolSetAttributeUID            = 0x00000002,
    MileCrinoProtocolSetAttributeGID            = 0x00000004,
    MileCrinoProtocolSetAttributeSize           = 0x00000008,
    MileCrinoProtocolSetAttributeAccessTime     = 0x00000010,
    MileCrinoProtocolSetAttributeModifiedTime   = 0x00000020,
    MileCrinoProtocolSetAttributeChangeTime     = 0x00000040,
    MileCrinoProtocolSetAttributeAccessTimeSet  = 0x00000080,
    MileCrinoProtocolSetAttributeModifiedTimeSet= 0x00000100,
} MILE_CRINO_PROTOCOL_SET_ATTRIBUTE_FLAG, *PMILE_CRINO_PROTOCOL_SET_ATTRIBUTE_FLAG;

typedef enum _MILE_CRINO_PROTOCOL_LOCK_TYPE {
    MileCrinoProtocolLockTypeReadLock   = 0,
    MileCrinoProtocolLockTypeWriteLock  = 1,
    MileCrinoProtocolLockTypeUnlock     = 2,
} MILE_CRINO_PROTOCOL_LOCK_TYPE, *PMILE_CRINO_PROTOCOL_LOCK_TYPE;

typedef enum _MILE_CRINO_PROTOCOL_LOCK_FLAGS{
    MileCrinoProtocolLockTypeBlock  = 1,
    MileCrinoProtocolLockTypeReclaim= 2
} MILE_CRINO_PROTOCOL_LOCK_FLAGS, *PMILE_CRINO_PROTOCOL_LOCK_FLAGS;

typedef enum _MILE_CRINO_PROTOCOL_LOCK_STATUS {
    MileCrinoProtocolLockSuccess= 0,
    MileCrinoProtocolLockBlocked= 1,
    MileCrinoProtocolLockError  = 2,
    MileCrinoProtocolLockGrace  = 3,

} MILE_CRINO_PROTOCOL_LOCK_STATUS, *PMILE_CRINO_PROTOCOL_LOCK_STATUS;

/*
 * Flags in Tunlinkat (which re-uses f_tlcreate data structure but
 * with different meaning).
 */
typedef enum _MILE_CRINO_PROTOCOL_UNLINK_AT_FLAGS {
    MileProtocolAtRemoveDirectory = 0x0200
} MILE_CRINO_PROTOCOL_UNLINK_AT_FLAGS, *PMILE_CRINO_PROTOCOL_UNLINK_AT_FLAGS;

#endif // !MILE_CIRNO_PROTOCOL
