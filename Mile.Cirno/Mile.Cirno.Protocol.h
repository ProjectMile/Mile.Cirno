/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Protocol.h
 * PURPOSE:   Definition for Plan 9 File System Protocol
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

// References
// - https://9p.io/sys/man/5/INDEX.html
// - https://github.com/9fans/plan9port/blob/master/include/fcall.h
// - https://ericvh.github.io/9p-rfc/rfc9p2000.html
// - https://ericvh.github.io/9p-rfc/rfc9p2000.u.html
// - https://github.com/freebsd/freebsd-src/blob/main/contrib/lib9p/fcall.h
// - https://github.com/pfpacket/rust-9p/blob/master/src/fcall.rs
// - https://github.com/piscisaureus/rust-9p/blob/master/src/fcall.rs
// - https://github.com/microsoft/WSL/blob/master/src/linux/plan9/p9defs.h
// - https://github.com/microsoft/WSL/blob/master/src/linux/plan9/p9data.h

// Use traditional C/C++ language features only because this header file is
// designed for more universal interoperability scenarios.

#ifndef MILE_CIRNO_PROTOCOL
#define MILE_CIRNO_PROTOCOL

#ifdef __cplusplus
#include <cstdint>
#include <string>
#include <vector>
#else
#include <stdint.h>
#endif

/**
 * @brief The definition of Plan 9 File System Protocol Message Type.
 */
typedef enum _MILE_CIRNO_MESSAGE_TYPE
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
    //   header<Header>[1] qid<Qid>[1] iounit[4]
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
    //   header<Header>[1] qid<Qid>[1] iounit[4]
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

    // header<Header>[1] fid[4] flags[4]
    MileCirnoAccessRequestMessage = 128,
    // header<Header>[1]
    MileCirnoAccessResponseMessage,
    // header<Header>[1] fid[4] offset[8] count[4]
    MileCirnoWindowsReadDirRequestMessage = 130,
    // header<Header>[1] count[4] data<WindowsDirectoryEntry>[count]
    MileCirnoWindowsReadDirResponseMessage,
    // header<Header>[1] fid[4] newfid[4] flags[4] wflags[4] mode[4] gid[4]
    // attr_mask[8] nwname[2] wname<String>[nwname]
    MileCirnoWindowsOpenRequestMessage = 132,
    // header<Header>[1] status[1] walked[2] qid<Qid>[1]
    // symlink_target<String>[1] iounit[4] mode[4] uid[4] gid[4] nlink[8]
    // rdev[8] size[8] blksize[8] blocks[8] atime_sec[8] atime_nsec[8]
    // mtime_sec[8] mtime_nsec[8] ctime_sec[8] ctime_nsec[8] btime_sec[8]
    // btime_nsec[8] gen[8] data_version[8]
    MileCirnoWindowsOpenResponseMessage,
} MILE_CIRNO_MESSAGE_TYPE, *PMILE_CIRNO_MESSAGE_TYPE;

#ifdef __cplusplus
#define MILE_CIRNO_NOTAG ((std::uint16_t)~0)
#define MILE_CIRNO_NOFID ((std::uint32_t)~0)
#define MILE_CIRNO_NONUNAME ((std::uint32_t)~0)
#else
#define MILE_CIRNO_NOTAG ((uint16_t)~0)
#define MILE_CIRNO_NOFID ((uint32_t)~0)
#define MILE_CIRNO_NONUNAME ((uint32_t)~0)
#endif
#define MILE_CIRNO_FSTYPE 0x01021997
#define MILE_CIRNO_MAXWELEM 16

/*
 * @brief Type code bits in (the first byte of) a qid.
 */
typedef enum _MILE_CIRNO_QID_TYPE
{
    MileCirnoQidTypeFile = 0x00,
    MileCirnoQidTypeHardLink = 0x01,
    MileCirnoQidTypeSymbolicLink = 0x02,
    MileCirnoQidTypeTemporary = 0x04,
    MileCirnoQidTypeAuthentication = 0x08,
    MileCirnoQidTypeMountPoint = 0x10,
    MileCirnoQidTypeExclusive = 0x20,
    MileCirnoQidTypeAppend = 0x40,
    MileCirnoQidTypeDirectory = 0x80,   
} MILE_CIRNO_QID_TYPE, *PMILE_CIRNO_QID_TYPE;

/*
 * @brief The perm field flags used in MileCirnoCreateRequestMessage.
 */
typedef enum _MILE_CIRNO_PERMISSION_MODE
{
    MileCirnoPermissionModeDirectory = 0x80000000,
    MileCirnoPermissionModeAppend = 0x40000000,
    MileCirnoPermissionModeExclusive = 0x20000000,
    MileCirnoPermissionModeMount = 0x10000000,
    MileCirnoPermissionModeAuthenticationFile = 0x08000000,
    MileCirnoPermissionModeTemporaryFile = 0x04000000,
    MileCirnoPermissionModeSymbolicLink = 0x02000000,
    MileCirnoPermissionModeRead = 0x00000004,
    MileCirnoPermissionModeWrite = 0x00000002,
    MileCirnoPermissionModeExecute = 0x00000001,

    /* 9P2000.u */

    MileCirnoPermissionModeDevice = 0x00800000,
    MileCirnoPermissionModeNamedPipe = 0x00200000,
    MileCirnoPermissionModeSocket = 0x00100000,
    MileCirnoPermissionModeSetUid = 0x00080000,
    MileCirnoPermissionModeSetGid = 0x00040000,
} MILE_CIRNO_PERMISSION_MODE, *PMILE_CIRNO_PERMISSION_MODE;

/**
 * @brief The flags used in MileCirnoOpenRequestMessage and
 *        MileCirnoCreateRequestMessage.
 */
typedef enum _MILE_CIRNO_OPEN_MODE
{
    MileCirnoOpenModeRead = 0,
    MileCirnoOpenModeWrite = 1,
    MileCirnoOpenModeReadAndWrite = 2,
    MileCirnoOpenModeExecute = 3,
    MileCirnoOpenModeAccessModeMask = 3,
    MileCirnoOpenModeTruncate = 16,
    MileCirnoOpenModeCloseOnExecute = 32,
    MileCirnoOpenModeRemoveOnClose = 64,
    MileCirnoOpenModeDirectAccess = 128,
} MILE_CIRNO_OPEN_MODE, *PMILE_CIRNO_OPEN_MODE;

/**
 * @brief The flags used in MileCirnoLinuxOpenRequestMessage and
 *        MileCirnoLinuxCreateRequestMessage.
 */
typedef enum _MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS
{
    MileCirnoLinuxOpenCreateFlagReadOnly = 000000000U,
    MileCirnoLinuxOpenCreateFlagWriteOnly = 000000001U,
    MileCirnoLinuxOpenCreateFlagReadWrite = 000000002U,
    MileCirnoLinuxOpenCreateFlagNoAccess = 000000003U,
    MileCirnoLinuxOpenCreateFlagAccessMask = 000000003U,
    MileCirnoLinuxOpenCreateFlagCreate = 000000100U,
    MileCirnoLinuxOpenCreateFlagCreateOnlyWhenNotExist = 000000200U,
    MileCirnoLinuxOpenCreateFlagNoControllingTerminal = 000000400U,
    MileCirnoLinuxOpenCreateFlagTruncate = 000001000U,
    MileCirnoLinuxOpenCreateFlagAppend = 000002000U,
    MileCirnoLinuxOpenCreateFlagNonBlock = 000004000U,
    MileCirnoLinuxOpenCreateFlagDataSynchronize = 000010000U,
    MileCirnoLinuxOpenCreateFlagFileAsynchronize = 000020000U,
    MileCirnoLinuxOpenCreateFlagDirect = 000040000U,
    MileCirnoLinuxOpenCreateFlagLargeFile = 000100000U,
    MileCirnoLinuxOpenCreateFlagDirectory = 000200000U,
    MileCirnoLinuxOpenCreateFlagNoFollowSymbolicLink = 000400000U,
    MileCirnoLinuxOpenCreateFlagNoLastAccessTime = 001000000U,
    MileCirnoLinuxOpenCreateFlagCloseOnExecute = 002000000U,
    MileCirnoLinuxOpenCreateFlagSynchronize = 004000000U,
    MileCirnoLinuxOpenCreateFlagPath = 010000000U,
    MileCirnoLinuxOpenCreateFlagTemporaryFile = 020000000U,
} MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS, *PMILE_CIRNO_LINUX_OPEN_CREATE_FLAGS;

/**
 * @brief The request_mask field flags used in MileCirnoGetAttrRequestMessage
 *        and the valid flags used in MileCirnoGetAttrResponseMessage.
 */
typedef enum _MILE_CIRNO_LINUX_GETATTR_FLAGS
{
    MileCirnoLinuxGetAttrFlagMode = 0x00000001,
    MileCirnoLinuxGetAttrFlagNumberOfHardLinks = 0x00000002,
    MileCirnoLinuxGetAttrFlagOwnerUserId = 0x00000004,
    MileCirnoLinuxGetAttrFlagGroupId = 0x00000008,
    MileCirnoLinuxGetAttrFlagDeviceId = 0x00000010,
    MileCirnoLinuxGetAttrFlagLastAccessTime = 0x00000020,
    MileCirnoLinuxGetAttrFlagLastWriteTime = 0x00000040,
    MileCirnoLinuxGetAttrFlagChangeTime = 0x00000080,
    MileCirnoLinuxGetAttrFlagInodeNumber = 0x00000100,
    MileCirnoLinuxGetAttrFlagSize = 0x00000200,
    MileCirnoLinuxGetAttrFlagAllocatedBlocks = 0x00000400,
    // Everything up to and including MileCirnoLinuxGetAttrFlagBlocks is
    // MileCirnoLinuxGetAttrFlagBasic.
    MileCirnoLinuxGetAttrFlagBasic =
        MileCirnoLinuxGetAttrFlagMode |
        MileCirnoLinuxGetAttrFlagNumberOfHardLinks |
        MileCirnoLinuxGetAttrFlagOwnerUserId |
        MileCirnoLinuxGetAttrFlagGroupId |
        MileCirnoLinuxGetAttrFlagDeviceId |
        MileCirnoLinuxGetAttrFlagLastAccessTime |
        MileCirnoLinuxGetAttrFlagLastWriteTime |
        MileCirnoLinuxGetAttrFlagChangeTime |
        MileCirnoLinuxGetAttrFlagInodeNumber |
        MileCirnoLinuxGetAttrFlagSize |
        MileCirnoLinuxGetAttrFlagAllocatedBlocks,
    MileCirnoLinuxGetAttrFlagBirthTime = 0x00000800,
    MileCirnoLinuxGetAttrFlagGenration = 0x00001000,
    MileCirnoLinuxGetAttrFlagDataVersion = 0x00002000,
    MileCirnoLinuxGetAttrFlagAll =
        MileCirnoLinuxGetAttrFlagBasic |
        MileCirnoLinuxGetAttrFlagBirthTime |
        MileCirnoLinuxGetAttrFlagGenration |
        MileCirnoLinuxGetAttrFlagDataVersion,
} MILE_CIRNO_LINUX_GETATTR_FLAGS, *PMILE_CIRNO_LINUX_GETATTR_FLAGS;

/**
 * @brief The valid field flags used in MileCirnoSetAttrRequestMessage.
 */
typedef enum _MILE_CIRNO_LINUX_SETATTR_FLAGS
{
    MileCirnoLinuxSetAttrFlagMode = 0x00000001,
    MileCirnoLinuxSetAttrFlagOwnerUserId = 0x00000002,
    MileCirnoLinuxSetAttrFlagGroupId = 0x00000004,
    MileCirnoLinuxSetAttrFlagSize = 0x00000008,
    MileCirnoLinuxSetAttrFlagLastAccessTime = 0x00000010,
    MileCirnoLinuxSetAttrFlagLastWriteTime = 0x00000020,
    MileCirnoLinuxSetAttrFlagChangeTime = 0x00000040,
    MileCirnoLinuxSetAttrFlagLastAccessTimeSet = 0x00000080,
    MileCirnoLinuxSetAttrFlagLastWriteTimeSet = 0x00000100,
} MILE_CIRNO_LINUX_SETATTR_FLAGS, *PMILE_CIRNO_LINUX_SETATTR_FLAGS;

typedef enum _MILE_CIRNO_LINUX_LOCK_TYPE
{
    MileCirnoLinuxLockTypeRead = 0,
    MileCirnoLinuxLockTypeWrite = 1,
    MileCirnoLinuxLockTypeUnlock = 2,
} MILE_CIRNO_LINUX_LOCK_TYPE, *PMILE_CIRNO_LINUX_LOCK_TYPE;

typedef enum _MILE_CIRNO_LINUX_LOCK_FLAGS
{
    MileCirnoLinuxLockFlagBlock = 1,
    MileCirnoLinuxLockFlagReclaim = 2,
} MILE_CIRNO_LINUX_LOCK_FLAGS, *PMILE_CIRNO_LINUX_LOCK_FLAGS;

typedef enum _MILE_CIRNO_LINUX_LOCK_STATUS
{
    MileCirnoLinuxLockStatusSuccess = 0,
    MileCirnoLinuxLockStatusBlocked = 1,
    MileCirnoLinuxLockStatusError = 2,
    MileCirnoLinuxLockStatusGrace = 3,
} MILE_CIRNO_LINUX_LOCK_STATUS, *PMILE_CIRNO_LINUX_LOCK_STATUS;

/**
 * @brief The flags used in MileCirnoUnlinkAtRequestMessage.
 */
typedef enum _MILE_CIRNO_LINUX_UNLINKAT_FLAGS
{
    MileCirnoLinuxUnlinkAtFlagRemoveDirectory = 0x00000200,
} MILE_CIRNO_LINUX_UNLINKAT_FLAGS, *PMILE_CIRNO_LINUX_UNLINKAT_FLAGS;

/**
 * @brief The flags used in MileCirnoAccessRequestMessage.
 */
typedef enum _MILE_CIRNO_WINDOWS_ACCESS_FLAGS
{
    MileCirnoWindowsAccessFlagOk = 0,
    MileCirnoWindowsAccessFlagExecute = 1,
    MileCirnoWindowsAccessFlagWrite = 2,
    MileCirnoWindowsAccessFlagRead = 4,
    MileCirnoWindowsAccessFlagDelete = 8,
    MileCirnoWindowsAccessFlagAll =
        MileCirnoWindowsAccessFlagExecute |
        MileCirnoWindowsAccessFlagWrite |
        MileCirnoWindowsAccessFlagRead |
        MileCirnoWindowsAccessFlagDelete,
} MILE_CIRNO_WINDOWS_ACCESS_FLAGS, *PMILE_CIRNO_WINDOWS_ACCESS_FLAGS;

/**
 * @brief The wflags used in MileCirnoWindowsOpenRequestMessage.
 */
typedef enum _MILE_CIRNO_WINDOWS_OPEN_FLAGS
{
    MileCirnoWindowsOpenFlagNone = 0,
    MileCirnoWindowsOpenFlagDeleteAccess = 1,
    MileCirnoWindowsOpenFlagNonDirectoryFile = 2,
    MileCirnoWindowsOpenFlagOpenSymbolicLink = 4,
} MILE_CIRNO_WINDOWS_OPEN_FLAGS, *PMILE_CIRNO_WINDOWS_OPEN_FLAGS;

/**
 * @brief The status used in MileCirnoWindowsOpenResponseMessage.
 */
typedef enum _MILE_CIRNO_WINDOWS_OPEN_STATUS
{
    MileCirnoWindowsOpenStatusOpened = 0,
    MileCirnoWindowsOpenStatusCreated = 1,
    MileCirnoWindowsOpenStatusParentNotFound = 2,
    MileCirnoWindowsOpenStatusNotFound = 3,
    MileCirnoWindowsOpenStatusStopped = 4,
} MILE_CIRNO_WINDOWS_OPEN_STATUS, *PMILE_CIRNO_WINDOWS_OPEN_STATUS;

#ifdef __cplusplus
namespace Mile
{
    namespace Cirno
    {
        struct Header
        {
            std::uint32_t Size;
            std::uint8_t Type; // MILE_CIRNO_MESSAGE_TYPE
            std::uint16_t Tag;
        };

        const std::uint32_t HeaderSize =
            sizeof(std::uint32_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t);

        struct Qid
        {
            std::uint8_t Type; // MILE_CIRNO_QID_TYPE
            std::uint32_t Version;
            std::uint64_t Path;
        };

        struct DirectoryEntry
        {
            Qid UniqueId; // qid
            std::uint64_t Offset;
            std::uint8_t Type;
            std::string Name;
        };

        struct Stat
        {
            std::uint16_t Type;
            std::uint32_t Dev;
            Qid UniqueId; // qid
            std::uint32_t Mode;
            std::uint32_t LastAccessTime; // atime
            std::uint32_t LastWriteTime; // mtime
            std::uint64_t FileSize; // length
            std::string FileName; // name
            std::string OwnerUserId; // uid
            std::string GroupId; // gid
            std::string LastWriteUserId; // muid
            std::string UnixExtension; // extension (9P2000.u)
            std::uint32_t NumericOwnerUserId; // n_uid (9P2000.u)
            std::uint32_t NumericGroupId; // n_gid (9P2000.u)
            std::uint32_t NumericLastWriteUserId; // n_muid (9P2000.u)
        };

        struct WindowsDirectoryEntry
        {
            DirectoryEntry Entry;
            Stat Stat;
        };

        struct LinuxErrorResponse
        {
            std::uint32_t Code;
        };

        struct StatFsRequest
        {
            std::uint32_t FileId; // fid
        };

        struct StatFsResponse
        {
            std::uint32_t FileSystemType; // type
            std::uint32_t BlockSize; // bsize
            std::uint64_t TotalBlocks; // blocks
            std::uint64_t FreeBlocks; // bfree
            std::uint64_t AvailableBlocks; // bavail
            std::uint64_t TotalFiles; // files
            std::uint64_t FreeFileNodes; // ffree
            std::uint64_t FileSystemId; // fsid
            std::uint32_t MaximumFileNameLength; // namelen
        };

        struct LinuxOpenRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t Flags; // MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS
        };

        struct LinuxOpenResponse
        {
            Qid UniqueId; // qid
            std::uint32_t IoUnit;
        };

        struct LinuxCreateRequest
        {
            std::uint32_t FileId; // fid
            std::string Name;
            std::uint32_t Flags; // MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS
            std::uint32_t Mode;
            std::uint32_t Gid;
        };

        struct LinuxCreateResponse
        {
            Qid UniqueId; // qid
            std::uint32_t IoUnit;
        };

        struct SymLinkRequest
        {
            std::uint32_t FileId; // fid
            std::string Name;
            std::string Target;
            std::uint32_t Gid;
        };

        struct SymLinkResponse
        {
            Qid UniqueId; // qid
        };

        struct MkNodRequest
        {
            std::uint32_t DirectoryFileId; // dfid
            std::string Name;
            std::uint32_t Mode;
            std::uint32_t Major;
            std::uint32_t Minor;
            std::uint32_t Gid;
        };

        struct MkNodResponse
        {
            Qid UniqueId; // qid
        };

        struct RenameRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t DirectoryFid; // dfid
            std::string Name;
        };

        // RenameResponse

        struct ReadLinkRequest
        {
            std::uint32_t FileId; // fid
        };

        struct ReadLinkResponse
        {
            std::string Target;
        };

        struct GetAttrRequest
        {
            std::uint32_t FileId; // fid
            std::uint64_t RequestMask; // request_mask
        };

        struct GetAttrResponse
        {
            std::uint64_t Valid; // MILE_CIRNO_LINUX_GETATTR_FLAGS
            Qid UniqueId; // qid
            std::uint32_t Mode;
            std::uint32_t OwnerUserId; // uid
            std::uint32_t GroupId; // gid
            std::uint64_t NumberOfHardLinks; // nlink
            std::uint64_t DeviceId; // rdev
            std::uint64_t FileSize; // size
            std::uint64_t BlockSize; // blksize
            std::uint64_t AllocatedBlocks; // blocks
            std::uint64_t LastAccessTimeSeconds; // atime_sec
            std::uint64_t LastAccessTimeNanoseconds; // atime_nsec
            std::uint64_t LastWriteTimeSeconds; // mtime_sec
            std::uint64_t LastWriteTimeNanoseconds; // mtime_nsec
            std::uint64_t ChangeTimeSeconds; // ctime_sec
            std::uint64_t ChangeTimeNanoseconds; // ctime_nsec
            std::uint64_t BirthTimeSeconds; // btime_sec
            std::uint64_t BirthTimeNanoseconds; // btime_nsec
            std::uint64_t Generation; // gen
            std::uint64_t DataVersion; // data_version
        };

        struct SetAttrRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t Valid; // MILE_CIRNO_LINUX_SETATTR_FLAGS
            std::uint32_t Mode;
            std::uint32_t OwnerUserId; // uid
            std::uint32_t GroupId; // gid
            std::uint64_t FileSize; // size
            std::uint64_t LastAccessTimeSeconds; // atime_sec
            std::uint64_t LastAccessTimeNanoseconds; // atime_nsec
            std::uint64_t LastWriteTimeSeconds; // mtime_sec
            std::uint64_t LastWriteTimeNanoseconds; // mtime_nsec
        };

        // SetAttrResponse

        struct XattrWalkRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t NewFileId; // newfid
            std::string Name;
        };

        struct XattrWalkResponse
        {
            std::uint64_t Size;
        };

        struct XattrCreateRequest
        {
            std::uint32_t FileId; // fid
            std::string Name;
            std::uint64_t Size;
            std::uint32_t Flags;
        };

        // XattrCreateResponse

        struct ReadDirRequest
        {
            std::uint32_t FileId; // fid
            std::uint64_t Offset;
            std::uint32_t Count;
        };

        struct ReadDirResponse
        {
            std::vector<DirectoryEntry> Data;
        };

        const std::uint32_t ReadDirResponseHeaderSize =
            HeaderSize + sizeof(std::uint32_t);

        struct FsyncRequest
        {
            std::uint32_t FileId; // fid
        };

        // FsyncResponse

        struct LockRequest
        {
            std::uint32_t FileId; // fid
            std::uint8_t Type; // MILE_CIRNO_LINUX_LOCK_TYPE
            std::uint32_t Flags; // MILE_CIRNO_LINUX_LOCK_FLAGS
            std::uint64_t Start;
            std::uint64_t Length;
            std::uint32_t ProcessId; // proc_id
            std::string ClientId; // client_id
        };

        struct LockResponse
        {
            std::uint8_t Status; // MILE_CIRNO_LINUX_LOCK_STATUS
        };

        struct GetLockRequest
        {
            std::uint32_t FileId; // fid
            std::uint8_t Type; // MILE_CIRNO_LINUX_LOCK_TYPE
            std::uint64_t Start;
            std::uint64_t Length;
            std::uint32_t ProcessId;
            std::string ClientId;
        };

        struct GetLockResponse
        {
            std::uint8_t Type; // MILE_CIRNO_LINUX_LOCK_TYPE
            std::uint64_t Start;
            std::uint64_t Length;
            std::uint32_t ProcessId;
            std::string ClientId;
        };

        struct LinkRequest
        {
            std::uint32_t DirectoryFileId; // dfid
            std::uint32_t FileId; // fid
            std::string Name;
        };

        // LinkResponse

        struct MkDirRequest
        {
            std::uint32_t DirectoryFileId; // dfid
            std::string Name;
            std::uint32_t Mode;
            std::uint32_t Gid;
        };

        struct MkDirResponse
        {
            Qid UniqueId; // qid
        };

        struct RenameAtRequest
        {
            std::uint32_t OldDirectoryFileId; // olddirfid
            std::string OldName;
            std::uint32_t NewDirectoryFileId; // newdirfid
            std::string NewName;
        };

        // RenameAtResponse

        struct UnlinkAtRequest
        {
            std::uint32_t DirectoryFileId; // dirfd
            std::string Name;
            std::uint32_t Flags; // MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS
        };

        // UnlinkAtResponse

        struct VersionRequest
        {
            std::uint32_t MaximumMessageSize; // msize
            std::string ProtocolVersion; // version
        };

        const std::uint32_t DefaultMaximumMessageSize = 1 << 16;

        const std::string DefaultProtocolVersion = "9P2000.L";

        struct VersionResponse
        {
            std::uint32_t MaximumMessageSize; // msize
            std::string ProtocolVersion; // version
        };

        struct AuthRequest
        {
            std::uint32_t AuthenticationFileId; // afid
            std::string UserName; // uname
            std::string AccessName; // aname
            std::uint32_t NumericUserName; // n_uname (9P2000.L)
        };

        struct AuthResponse
        {
            Qid AuthenticationUniqueId; // aqid
        };

        struct AttachRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t AuthenticationFileId; // afid
            std::string UserName; // uname
            std::string AccessName; // aname
            std::uint32_t NumericUserName; // n_uname (9P2000.L and 9P2000.W)
        };

        struct AttachResponse
        {
            Qid UniqueId; // qid
        };

        struct ErrorResponse
        {
            std::string Message;
            std::uint32_t Code; // (9P2000.u)
        };

        struct FlushRequest
        {
            std::uint16_t OldTag;
        };

        // FlushResponse

        struct WalkRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t NewFileId; // newfid
            std::vector<std::string> Names; // wname, nwname
        };

        struct WalkResponse
        {
            std::vector<Qid> UniqueIds; // nwqid, wqid
        };

        struct OpenRequest
        {
            std::uint32_t FileId; // fid
            std::uint8_t Mode; // MILE_CIRNO_OPEN_MODE
        };

        struct OpenResponse
        {
            Qid UniqueId; // qid
            std::uint32_t IoUnit;
        };

        struct CreateRequest
        {
            std::uint32_t FileId; // fid
            std::string Name;
            std::uint32_t Permission; // perm, MILE_CIRNO_PERMISSION_MODE
            std::uint32_t Mode;
            std::string UnixExtension; // extension (9P2000.u)
        };

        struct CreateResponse
        {
            Qid UniqueId; // qid
            std::uint32_t IoUnit;
        };

        struct ReadRequest
        {
            std::uint32_t FileId; // fid
            std::uint64_t Offset;
            std::uint32_t Count;
        };

        struct ReadResponse
        {
            std::vector<std::uint8_t> Data; // count, data
        };

        const std::uint32_t ReadResponseHeaderSize =
            HeaderSize + sizeof(std::uint32_t);

        struct WriteRequest
        {
            std::uint32_t FileId; // fid
            std::uint64_t Offset;
            std::vector<std::uint8_t> Data; // count, data
        };

        struct WriteResponse
        {
            std::uint32_t Count;
        };

        struct ClunkRequest
        {
            std::uint32_t FileId; // fid
        };

        // ClunkResponse

        struct RemoveRequest
        {
            std::uint32_t FileId; // fid
        };

        // RemoveResponse

        struct StatRequest
        {
            std::uint32_t FileId; // fid
        };

        struct StatResponse
        {
            std::vector<Stat> Stat;
        };

        struct WriteStatRequest
        {
            std::uint32_t FileId; // fid
            std::vector<Stat> Stat;
        };

        // WriteStatResponse

        // AccessRequest

        struct AccessRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t Flags; // MILE_CIRNO_WINDOWS_ACCESS_FLAGS
        };

        // AccessResponse

        // WindowsReadDirRequest

        struct WindowsReadDirRequest
        {
            std::uint32_t FileId; // fid
            std::uint64_t Offset;
            std::uint32_t Count;
        };

        // WindowsReadDirResponse

        struct WindowsReadDirResponse
        {
            std::vector<WindowsDirectoryEntry> Data; // count, data
        };

        // WindowsOpenRequest

        struct WindowsOpenRequest
        {
            std::uint32_t FileId; // fid
            std::uint32_t NewFileId; // newfid
            std::uint32_t Flags; // MILE_CIRNO_LINUX_OPEN_CREATE_FLAGS
            std::uint32_t WindowsFlags; // MILE_CIRNO_WINDOWS_OPEN_FLAGS
            std::uint32_t Mode;
            std::uint32_t Gid;
            std::uint64_t AttributeMask; // attr_mask
            std::vector<std::string> Names; // wname, nwname
        };

        // WindowsOpenResponse

        struct WindowsOpenResponse
        {
            std::uint8_t Status; // MILE_CIRNO_WINDOWS_OPEN_STATUS
            std::uint16_t Walked; // walked
            Qid UniqueId; // qid
            std::string SymlinkTarget; // symlink_target
            std::uint32_t IoUnit;
            std::uint32_t Mode;
            std::uint32_t UserId; // uid
            std::uint32_t GroupId; // gid
            std::uint64_t NumberOfHardLinks; // nlink
            std::uint64_t DeviceId; // rdev
            std::uint64_t FileSize; // size
            std::uint64_t BlockSize; // blksize
            std::uint64_t Blocks; // blocks
            std::uint64_t LastAccessTimeSeconds; // atime_sec
            std::uint64_t LastAccessTimeNanoseconds; // atime_nsec
            std::uint64_t LastWriteTimeSeconds; // mtime_sec
            std::uint64_t LastWriteTimeNanoseconds; // mtime_nsec
            std::uint64_t ChangeTimeSeconds; // ctime_sec
            std::uint64_t ChangeTimeNanoseconds; // ctime_nsec
            std::uint64_t BirthTimeSeconds; // btime_sec
            std::uint64_t BirthTimeNanoseconds; // btime_nsec
            std::uint64_t Generation; // gen
            std::uint64_t DataVersion; // data_version
        };
    }
}
#endif // __cplusplus

#endif // !MILE_CIRNO_PROTOCOL
