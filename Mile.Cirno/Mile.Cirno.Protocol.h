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

#ifndef MILE_CIRNO_PROTOCOL
#define MILE_CIRNO_PROTOCOL

#include <stdint.h>

#ifdef _MSC_VER
#if _MSC_VER > 1000
#pragma once
#endif
#if (_MSC_VER >= 1200)
#pragma warning(push)
#endif
#pragma warning(disable:4200) // zero length array
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
} MILE_CIRNO_MESSAGE_TYPE, *PMILE_CIRNO_MESSAGE_TYPE;

typedef struct _MILE_CIRNO_HEADER
{
    uint8_t Size[4];
    uint8_t Type[1];
    uint8_t Tag[2];
} MILE_CIRNO_HEADER, *PMILE_CIRNO_HEADER;

typedef struct _MILE_CIRNO_STRING
{
    uint8_t Length[2];
    uint8_t String[0];
} MILE_CIRNO_STRING, *PMILE_CIRNO_STRING;

typedef struct _MILE_CIRNO_QID
{
    uint8_t Type[1];
    uint8_t Version[4];
    uint8_t Path[8];
} MILE_CIRNO_QID, *PMILE_CIRNO_QID;

// %Name%<DirectoryEntry>[%Length%]

// %Name%<Stat>[%Length%]

// %Name%<WindowsDirectoryEntry>[%Length%]

typedef struct _MILE_CIRNO_LINUX_ERROR_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Code[4]; // ecode
} MILE_CIRNO_LINUX_ERROR_RESPONSE, *PMILE_CIRNO_LINUX_ERROR_RESPONSE;

typedef struct _MILE_CIRNO_STATFS_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_STATFS_REQUEST, *PMILE_CIRNO_STATFS_REQUEST;

typedef struct _MILE_CIRNO_STATFS_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileSystemType[4]; // type
    uint8_t BlockSize[4]; // bsize
    uint8_t TotalBlocks[8]; // blocks
    uint8_t FreeBlocks[8]; // bfree
    uint8_t AvailableBlocks[8]; // bavail
    uint8_t TotalFiles[8]; // files
    uint8_t FreeFileNodes[8]; // ffree
    uint8_t FileSystemId[8]; // fsid
    uint8_t MaximumFileNameLength[4]; // namelen
} MILE_CIRNO_STATFS_RESPONSE, *PMILE_CIRNO_STATFS_RESPONSE;

typedef struct _MILE_CIRNO_LINUX_OPEN_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Flags[4];
} MILE_CIRNO_LINUX_OPEN_REQUEST, *PMILE_CIRNO_LINUX_OPEN_REQUEST;

typedef struct _MILE_CIRNO_LINUX_OPEN_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
    uint8_t IoUnit[4];
} MILE_CIRNO_LINUX_OPEN_RESPONSE, *PMILE_CIRNO_LINUX_OPEN_RESPONSE;

// MILE_CIRNO_LINUX_CREATE_REQUEST

typedef struct _MILE_CIRNO_LINUX_CREATE_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
    uint8_t IoUnit[4];
} MILE_CIRNO_LINUX_CREATE_RESPONSE, *PMILE_CIRNO_LINUX_CREATE_RESPONSE;

// MILE_CIRNO_SYMLINK_REQUEST

typedef struct _MILE_CIRNO_SYMLINK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
} MILE_CIRNO_SYMLINK_RESPONSE, *PMILE_CIRNO_SYMLINK_RESPONSE;

// MILE_CIRNO_MKNOD_REQUEST

typedef struct _MILE_CIRNO_MKNOD_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
} MILE_CIRNO_MKNOD_RESPONSE, *PMILE_CIRNO_MKNOD_RESPONSE;

typedef struct _MILE_CIRNO_RENAME_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t DirectoryFid[4]; // dfid
    MILE_CIRNO_STRING Name;
} MILE_CIRNO_RENAME_REQUEST, *PMILE_CIRNO_RENAME_REQUEST;

typedef struct _MILE_CIRNO_RENAME_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_RENAME_RESPONSE, *PMILE_CIRNO_RENAME_RESPONSE;

typedef struct _MILE_CIRNO_READLINK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_READLINK_REQUEST, *PMILE_CIRNO_READLINK_REQUEST;

typedef struct _MILE_CIRNO_READLINK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_STRING Target;
} MILE_CIRNO_READLINK_RESPONSE, *PMILE_CIRNO_READLINK_RESPONSE;

typedef struct _MILE_CIRNO_GETATTR_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t RequestMask[8]; // request_mask
} MILE_CIRNO_GETATTR_REQUEST, *PMILE_CIRNO_GETATTR_REQUEST;

typedef struct _MILE_CIRNO_GETATTR_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Valid[8];
    MILE_CIRNO_QID UniqueId; // qid
    uint8_t Mode[4];
    uint8_t OwnerUserId[4]; // uid
    uint8_t GroupId[4]; // gid
    uint8_t NumberOfHardLinks[8]; // nlink
    uint8_t DeviceId[8]; // rdev
    uint8_t FileSize[8]; // size
    uint8_t BlockSize[8]; // blksize
    uint8_t AllocatedBlocks[8]; // blocks
    uint8_t LastAccessTimeSeconds[8]; // atime_sec
    uint8_t LastAccessTimeNanoseconds[8]; // atime_nsec
    uint8_t LastWriteTimeSeconds[8]; // mtime_sec
    uint8_t LastWriteTimeNanoseconds[8]; // mtime_nsec
    uint8_t ChangeTimeSeconds[8]; // ctime_sec
    uint8_t ChangeTimeNanoseconds[8]; // ctime_nsec
    uint8_t BirthTimeSeconds[8]; // btime_sec
    uint8_t BirthTimeNanoseconds[8]; // btime_nsec
    uint8_t Generation[8]; // gen
    uint8_t DataVersion[8]; // data_version
} MILE_CIRNO_GETATTR_RESPONSE, *PMILE_CIRNO_GETATTR_RESPONSE;

typedef struct _MILE_CIRNO_SETATTR_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Valid[4];
    uint8_t Mode[4];
    uint8_t OwnerUserId[4]; // uid
    uint8_t GroupId[4]; // gid
    uint8_t FileSize[8]; // size
    uint8_t LastAccessTimeSeconds[8]; // atime_sec
    uint8_t LastAccessTimeNanoseconds[8]; // atime_nsec
    uint8_t LastWriteTimeSeconds[8]; // mtime_sec
    uint8_t LastWriteTimeNanoseconds[8]; // mtime_nsec
} MILE_CIRNO_SETATTR_REQUEST, *PMILE_CIRNO_SETATTR_REQUEST;

typedef struct _MILE_CIRNO_SETATTR_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_SETATTR_RESPONSE, *PMILE_CIRNO_SETATTR_RESPONSE;

typedef struct _MILE_CIRNO_XATTRWALK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t NewFileId[4]; // newfid
    MILE_CIRNO_STRING Name;
} MILE_CIRNO_XATTRWALK_REQUEST, *PMILE_CIRNO_XATTRWALK_REQUEST;

typedef struct _MILE_CIRNO_XATTRWALK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Size[8];
} MILE_CIRNO_XATTRWALK_RESPONSE, *PMILE_CIRNO_XATTRWALK_RESPONSE;

// MILE_CIRNO_XATTRCREATE_REQUEST

typedef struct _MILE_CIRNO_XATTRCREATE_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_XATTRCREATE_RESPONSE, *PMILE_CIRNO_XATTRCREATE_RESPONSE;

typedef struct _MILE_CIRNO_READDIR_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Offset[8];
    uint8_t Count[4];
} MILE_CIRNO_READDIR_REQUEST, *PMILE_CIRNO_READDIR_REQUEST;

// MILE_CIRNO_READDIR_RESPONSE

typedef struct _MILE_CIRNO_FSYNC_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_FSYNC_REQUEST, *PMILE_CIRNO_FSYNC_REQUEST;

typedef struct _MILE_CIRNO_FSYNC_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_FSYNC_RESPONSE, *PMILE_CIRNO_FSYNC_RESPONSE;

typedef struct _MILE_CIRNO_LOCK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Type[1];
    uint8_t Flags[4];
    uint8_t Start[8];
    uint8_t Length[8];
    uint8_t ProcessId[4]; // proc_id
    MILE_CIRNO_STRING ClientId; // client_id
} MILE_CIRNO_LOCK_REQUEST, *PMILE_CIRNO_LOCK_REQUEST;

typedef struct _MILE_CIRNO_LOCK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Status[1];
} MILE_CIRNO_LOCK_RESPONSE, *PMILE_CIRNO_LOCK_RESPONSE;

typedef struct _MILE_CIRNO_GETLOCK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Type[1];
    uint8_t Start[8];
    uint8_t Length[8];
    uint8_t ProcessId[4]; // proc_id
    MILE_CIRNO_STRING ClientId; // client_id
} MILE_CIRNO_GETLOCK_REQUEST, *PMILE_CIRNO_GETLOCK_REQUEST;

typedef struct _MILE_CIRNO_GETLOCK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Type[1];
    uint8_t Start[8];
    uint8_t Length[8];
    uint8_t ProcessId[4]; // proc_id
    MILE_CIRNO_STRING ClientId; // client_id
} MILE_CIRNO_GETLOCK_RESPONSE, *PMILE_CIRNO_GETLOCK_RESPONSE;

typedef struct _MILE_CIRNO_LINK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t DirectoryFileId[4]; // dfid
    uint8_t FileId[4]; // fid
    MILE_CIRNO_STRING Name;
} MILE_CIRNO_LINK_REQUEST, *PMILE_CIRNO_LINK_REQUEST;

typedef struct _MILE_CIRNO_LINK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_LINK_RESPONSE, *PMILE_CIRNO_LINK_RESPONSE;

// MILE_CIRNO_MKDIR_REQUEST

typedef struct _MILE_CIRNO_MKDIR_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
} MILE_CIRNO_MKDIR_RESPONSE, *PMILE_CIRNO_MKDIR_RESPONSE;

// MILE_CIRNO_RENAMEAT_REQUEST

typedef struct _MILE_CIRNO_RENAMEAT_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_RENAMEAT_RESPONSE, *PMILE_CIRNO_RENAMEAT_RESPONSE;

// MILE_CIRNO_UNLINKAT_REQUEST

typedef struct _MILE_CIRNO_UNLINKAT_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_UNLINKAT_RESPONSE, *PMILE_CIRNO_UNLINKAT_RESPONSE;

typedef struct _MILE_CIRNO_VERSION_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t MaximumMessageSize[4]; // msize
    MILE_CIRNO_STRING ProtocolVersion; // version
} MILE_CIRNO_VERSION_REQUEST, *PMILE_CIRNO_VERSION_REQUEST;

typedef struct _MILE_CIRNO_VERSION_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t MaximumMessageSize[4]; // msize
    MILE_CIRNO_STRING ProtocolVersion; // version
} MILE_CIRNO_VERSION_RESPONSE, *PMILE_CIRNO_VERSION_RESPONSE;

// MILE_CIRNO_AUTH_REQUEST

typedef struct _MILE_CIRNO_AUTH_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID AuthenticationUniqueId; // aqid
} MILE_CIRNO_AUTH_RESPONSE, *PMILE_CIRNO_AUTH_RESPONSE;

// MILE_CIRNO_ATTACH_REQUEST

typedef struct _MILE_CIRNO_ATTACH_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
} MILE_CIRNO_ATTACH_RESPONSE, *PMILE_CIRNO_ATTACH_RESPONSE;

// MILE_CIRNO_ERROR_RESPONSE

typedef struct _MILE_CIRNO_FLUSH_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t OldTag[2];
} MILE_CIRNO_FLUSH_REQUEST, *PMILE_CIRNO_FLUSH_REQUEST;

typedef struct _MILE_CIRNO_FLUSH_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_FLUSH_RESPONSE, *PMILE_CIRNO_FLUSH_RESPONSE;

// MILE_CIRNO_WALK_REQUEST

typedef struct _MILE_CIRNO_WALK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t UniqueIdCount[2]; // nwqid
    MILE_CIRNO_QID UniqueIdList[0]; // wqid
} MILE_CIRNO_WALK_RESPONSE, *PMILE_CIRNO_WALK_RESPONSE;

typedef struct _MILE_CIRNO_OPEN_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Mode[1];
} MILE_CIRNO_OPEN_REQUEST, *PMILE_CIRNO_OPEN_REQUEST;

typedef struct _MILE_CIRNO_OPEN_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
    uint8_t IoUnit[4];
} MILE_CIRNO_OPEN_RESPONSE, *PMILE_CIRNO_OPEN_RESPONSE;

// MILE_CIRNO_CREATE_REQUEST

typedef struct _MILE_CIRNO_CREATE_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    MILE_CIRNO_QID UniqueId; // qid
    uint8_t IoUnit[4];
} MILE_CIRNO_CREATE_RESPONSE, *PMILE_CIRNO_CREATE_RESPONSE;

typedef struct _MILE_CIRNO_READ_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Offset[8];
    uint8_t Count[4];
} MILE_CIRNO_READ_REQUEST, *PMILE_CIRNO_READ_REQUEST;

typedef struct _MILE_CIRNO_READ_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Count[4];
    uint8_t Data[0];
} MILE_CIRNO_READ_RESPONSE, *PMILE_CIRNO_READ_RESPONSE;

typedef struct _MILE_CIRNO_WRITE_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
    uint8_t Offset[8];
    uint8_t Count[4];
    uint8_t Data[0];
} MILE_CIRNO_WRITE_REQUEST, *PMILE_CIRNO_WRITE_REQUEST;

typedef struct _MILE_CIRNO_WRITE_RESPONSE
{
    MILE_CIRNO_HEADER Header;
    uint8_t Count[4];
} MILE_CIRNO_WRITE_RESPONSE, *PMILE_CIRNO_WRITE_RESPONSE;

typedef struct _MILE_CIRNO_CLUNK_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_CLUNK_REQUEST, *PMILE_CIRNO_CLUNK_REQUEST;

typedef struct _MILE_CIRNO_CLUNK_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_CLUNK_RESPONSE, *PMILE_CIRNO_CLUNK_RESPONSE;

typedef struct _MILE_CIRNO_REMOVE_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_REMOVE_REQUEST, *PMILE_CIRNO_REMOVE_REQUEST;

typedef struct _MILE_CIRNO_REMOVE_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_REMOVE_RESPONSE, *PMILE_CIRNO_REMOVE_RESPONSE;

typedef struct _MILE_CIRNO_STAT_REQUEST
{
    MILE_CIRNO_HEADER Header;
    uint8_t FileId[4]; // fid
} MILE_CIRNO_STAT_REQUEST, *PMILE_CIRNO_STAT_REQUEST;

// MILE_CIRNO_STAT_RESPONSE

// MILE_CIRNO_WRITESTAT_REQUEST

typedef struct _MILE_CIRNO_WRITESTAT_RESPONSE
{
    MILE_CIRNO_HEADER Header;
} MILE_CIRNO_WRITESTAT_RESPONSE, *PMILE_CIRNO_WRITESTAT_RESPONSE;

// MILE_CIRNO_ACCESS_REQUEST (Undocumented)

// MILE_CIRNO_ACCESS_RESPONSE (Undocumented)

// MILE_CIRNO_WINDOWS_READDIR_REQUEST (Undocumented)

// MILE_CIRNO_WINDOWS_READDIR_RESPONSE (Undocumented)

// MILE_CIRNO_WINDOWS_OPEN_REQUEST (Undocumented)

// MILE_CIRNO_WINDOWS_OPEN_RESPONSE (Undocumented)

#define MILE_CIRNO_NOTAG ((uint16_t)~0)
#define MILE_CIRNO_NOFID ((uint32_t)~0)
#define MILE_CIRNO_NONUNAME ((uint32_t)~0)
#define MILE_CIRNO_FSTYPE 0x01021997
#define MILE_CIRNO_MAXWELEM 16

/*
 * @brief Type code bits in (the first byte of) a qid.
 */
typedef enum _MILE_CIRNO_QID_TYPE
{
    MileCirnoQidTypeDirectory = 0x80,
    MileCirnoQidTypeAppendOnlyFiles = 0x40,
    MileCirnoQidTypeExclusiveUseFiles = 0x20,
    MileCirnoQidTypeMountedChannel = 0x10,
    MileCirnoQidTypeAuthenticationFile = 0x08,
    MileCirnoQidTypeTemporaryFile = 0x04,
    MileCirnoQidTypeSymbolicLink = 0x02,
    MileCirnoQidTypePlainFile = 0x00,
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
    MileCirnoLinuxOpenCreateFlagCreate = 000000100U,
    MileCirnoLinuxOpenCreateFlagExclude = 000000200U,
    MileCirnoLinuxOpenCreateFlagNoControllingTerminalType = 000000400U,
    MileCirnoLinuxOpenCreateFlagTruncate = 000001000U,
    MileCirnoLinuxOpenCreateFlagAppend = 000002000U,
    MileCirnoLinuxOpenCreateFlagNonBlock = 000004000U,
    MileCirnoLinuxOpenCreateFlagDataSynchronize = 000010000U,
    MileCirnoLinuxOpenCreateFlagFileAsynchronize = 000020000U,
    MileCirnoLinuxOpenCreateFlagDirect = 000040000U,
    MileCirnoLinuxOpenCreateFlagLargeFile = 000100000U,
    MileCirnoLinuxOpenCreateFlagDirectory = 000200000U,
    MileCirnoLinuxOpenCreateFlagNoFollow = 000400000U,
    MileCirnoLinuxOpenCreateFlagNoAccessTime = 001000000U,
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
    MileCirnoLinuxGetAttrFlagNLink = 0x00000002,
    MileCirnoLinuxGetAttrFlagUid = 0x00000004,
    MileCirnoLinuxGetAttrFlagGid = 0x00000008,
    MileCirnoLinuxGetAttrFlagRawDevice = 0x00000010,
    MileCirnoLinuxGetAttrFlagAccessTime = 0x00000020,
    MileCirnoLinuxGetAttrFlagModifiedTime = 0x00000040,
    MileCirnoLinuxGetAttrFlagChangeTime = 0x00000080,
    MileCirnoLinuxGetAttrFlagInodeNumber = 0x00000100,
    MileCirnoLinuxGetAttrFlagSize = 0x00000200,
    MileCirnoLinuxGetAttrFlagBlocks = 0x00000400,
    // Everything up to and including MileCirnoLinuxGetAttrFlagBlocks is
    // MileCirnoLinuxGetAttrFlagBasic.
    MileCirnoLinuxGetAttrFlagBasic =
        MileCirnoLinuxGetAttrFlagMode |
        MileCirnoLinuxGetAttrFlagNLink |
        MileCirnoLinuxGetAttrFlagUid |
        MileCirnoLinuxGetAttrFlagGid |
        MileCirnoLinuxGetAttrFlagRawDevice |
        MileCirnoLinuxGetAttrFlagAccessTime |
        MileCirnoLinuxGetAttrFlagModifiedTime |
        MileCirnoLinuxGetAttrFlagChangeTime |
        MileCirnoLinuxGetAttrFlagInodeNumber |
        MileCirnoLinuxGetAttrFlagSize |
        MileCirnoLinuxGetAttrFlagBlocks,
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
    MileCirnoLinuxSetAttrFlagUid = 0x00000002,
    MileCirnoLinuxSetAttrFlagGid = 0x00000004,
    MileCirnoLinuxSetAttrFlagSize = 0x00000008,
    MileCirnoLinuxSetAttrFlagAccessTime = 0x00000010,
    MileCirnoLinuxSetAttrFlagModifiedTime = 0x00000020,
    MileCirnoLinuxSetAttrFlagChangeTime = 0x00000040,
    MileCirnoLinuxSetAttrFlagAccessTimeSet = 0x00000080,
    MileCirnoLinuxSetAttrFlagModifiedTimeSet = 0x00000100,
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

#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#pragma warning(pop)
#else
#pragma warning(default:4200) // zero length array
#endif
#endif

#endif // !MILE_CIRNO_PROTOCOL
