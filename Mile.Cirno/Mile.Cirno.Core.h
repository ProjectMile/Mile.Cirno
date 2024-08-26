/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.h
 * PURPOSE:   Definition for Mile.Cirno Core API
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#ifndef MILE_CIRNO_CORE
#define MILE_CIRNO_CORE

#include "Mile.Cirno.Protocol.h"

#include <cstdint>

#include <string>
#include <vector>

namespace Mile::Cirno
{
    struct Header
    {
        std::uint32_t Size;
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_MESSAGE_TYPE
        std::uint16_t Tag;
    };

    std::vector<uint8_t> FromHeader(
        Header const& Value);

    // String

    std::vector<uint8_t> FromString(
        std::string const& Value);

    struct Qid
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_QID_TYPE
        std::uint32_t Version;
        std::uint64_t Path;
    };

    std::vector<uint8_t> FromQid(
        Qid const& Value);

    struct DirectoryEntry
    {
        Qid UniqueId; // qid
        std::uint64_t Offset;
        std::uint8_t Type;
        std::string Name;
    };

    std::vector<uint8_t> FromDirectoryEntry(
        DirectoryEntry const& Value);

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
    };

    std::vector<uint8_t> FromStat(
        Stat const& Value);

    struct UnixStat
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
        std::string UnixExtension; // extension
        std::uint32_t NumericOwnerUserId; // n_uid
        std::uint32_t NumericGroupId; // n_gid
        std::uint32_t NumericLastWriteUserId; // n_muid
    };

    std::vector<uint8_t> FromUnixStat(
        UnixStat const& Value);

    struct WindowsDirectoryEntry
    {
        DirectoryEntry Entry;
        Stat Stat;
    };

    std::vector<uint8_t> FromWindowsDirectoryEntry(
        WindowsDirectoryEntry const& Value);

    struct LinuxErrorResponse
    {
        std::uint32_t Code;
    };

    std::vector<uint8_t> FromLinuxErrorResponse(
        LinuxErrorResponse const& Value);

    struct StatFsRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromStatFsRequest(
        StatFsRequest const& Value);

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

    std::vector<uint8_t> FromStatFsResponse(
        StatFsResponse const& Value);

    struct LinuxOpenRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    std::vector<uint8_t> FromLinuxOpenRequest(
        LinuxOpenRequest const& Value);

    struct LinuxOpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    std::vector<uint8_t> FromLinuxOpenRequest(
        LinuxOpenRequest const& Value);

    struct LinuxCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
        std::uint32_t Mode;
        std::uint32_t Gid;
    };

    std::vector<uint8_t> FromLinuxCreateRequest(
        LinuxCreateRequest const& Value);

    struct LinuxCreateResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    std::vector<uint8_t> FromLinuxCreateResponse(
        LinuxCreateResponse const& Value);

    struct SymLinkRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::string Target;
        std::uint32_t Gid;
    };

    std::vector<uint8_t> FromSymLinkRequest(
        SymLinkRequest const& Value);

    struct SymLinkResponse
    {
        Qid UniqueId; // qid
    };

    std::vector<uint8_t> FromSymLinkResponse(
        SymLinkResponse const& Value);

    struct MkNodRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::string Name;
        std::uint32_t Mode;
        std::uint32_t Major;
        std::uint32_t Minor;
        std::uint32_t Gid;
    };

    std::vector<uint8_t> FromMkNodRequest(
        MkNodRequest const& Value);

    struct MkNodResponse
    {
        Qid UniqueId; // qid
    };

    std::vector<uint8_t> FromMkNodResponse(
        MkNodResponse const& Value);

    struct RenameRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t DirectoryFid; // dfid
        std::string Name;
    };

    std::vector<uint8_t> FromRenameRequest(
        RenameRequest const& Value);

    // RenameResponse

    struct ReadLinkRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromReadLinkRequest(
        ReadLinkRequest const& Value);

    struct ReadLinkResponse
    {
        std::string Target;
    };

    std::vector<uint8_t> FromReadLinkResponse(
        ReadLinkResponse const& Value);

    struct GetAttrRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t RequestMask; // request_mask
    };

    std::vector<uint8_t> FromGetAttrRequest(
        GetAttrRequest const& Value);

    struct GetAttrResponse
    {
        std::uint64_t Valid; // MILE_CIRNO_PROTOCOL_LINUX_GETATTR_FLAGS
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

    std::vector<uint8_t> FromGetAttrResponse(
        GetAttrResponse const& Value);

    struct SetAttrRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t Valid; // MILE_CIRNO_PROTOCOL_LINUX_SETATTR_FLAGS
        std::uint32_t Mode;
        std::uint32_t OwnerUserId; // uid
        std::uint32_t GroupId; // gid
        std::uint64_t FileSize; // size
        std::uint64_t LastAccessTimeSeconds; // atime_sec
        std::uint64_t LastAccessTimeNanoseconds; // atime_nsec
        std::uint64_t LastWriteTimeSeconds; // mtime_sec
        std::uint64_t LastWriteTimeNanoseconds; // mtime_nsec
    };

    std::vector<uint8_t> FromSetAttrRequest(
        SetAttrRequest const& Value);

    // SetAttrResponse

    struct XattrWalkRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t NewFileId; // newfid
        std::string Name;
    };

    std::vector<uint8_t> FromXattrWalkRequest(
        XattrWalkRequest const& Value);

    struct XattrWalkResponse
    {
        std::uint64_t Size;
    };

    std::vector<uint8_t> FromXattrWalkResponse(
        XattrWalkResponse const& Value);

    struct XattrCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint64_t Size;
        std::uint32_t Flags;
    };

    std::vector<uint8_t> FromXattrCreateRequest(
        XattrCreateRequest const& Value);

    // XattrCreateResponse

    struct ReadDirRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    std::vector<uint8_t> FromReadDirRequest(
        ReadDirRequest const& Value);

    struct ReadDirResponse
    {
        std::vector<DirectoryEntry> Data;
    };

    std::vector<uint8_t> FromReadDirResponse(
        ReadDirResponse const& Value);

    struct FsyncRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromFsyncRequest(
        FsyncRequest const& Value);

    // FsyncResponse

    struct LockRequest
    {
        std::uint32_t FileId; // fid
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_FLAGS
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId; // proc_id
        std::string ClientId; // client_id
    };

    std::vector<uint8_t> FromLockRequest(
        LockRequest const& Value);

    struct LockResponse
    {
        std::uint8_t Status; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_STATUS
    };

    std::vector<uint8_t> FromLockResponse(
        LockResponse const& Value);

    struct GetLockRequest
    {
        std::uint32_t FileId; // fid
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    std::vector<uint8_t> FromGetLockRequest(
        GetLockRequest const& Value);

    struct GetLockResponse
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    std::vector<uint8_t> FromGetLockResponse(
        GetLockResponse const& Value);

    struct LinkRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::uint32_t FileId; // fid
        std::string Name;
    };

    std::vector<uint8_t> FromLinkRequest(
        LinkRequest const& Value);

    // LinkResponse

    struct MkDirRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::string Name;
        std::uint32_t Mode;
        std::uint32_t Gid;
    };

    std::vector<uint8_t> FromMkDirRequest(
        MkDirRequest const& Value);

    struct MkDirResponse
    {
        Qid UniqueId; // qid
    };

    std::vector<uint8_t> FromMkDirResponse(
        MkDirResponse const& Value);

    struct RenameAtRequest
    {
        std::uint32_t OldDirectoryFileId; // olddirfid
        std::string OldName;
        std::uint32_t NewDirectoryFileId; // newdirfid
        std::string NewName;
    };

    std::vector<uint8_t> FromRenameAtRequest(
        RenameAtRequest const& Value);

    // RenameAtResponse

    struct UnlinkAtRequest
    {
        std::uint32_t DirectoryFileId; // dirfd
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    std::vector<uint8_t> FromUnlinkAtRequest(
        UnlinkAtRequest const& Value);

    // UnlinkAtResponse

    struct VersionRequest
    {
        std::uint32_t MaximumMessageSize; // msize
        std::string ProtocolVersion; // version
    };

    std::vector<uint8_t> FromVersionRequest(
        VersionRequest const& Value);

    struct VersionResponse
    {
        std::uint32_t MaximumMessageSize; // msize
        std::string ProtocolVersion; // version
    };

    std::vector<uint8_t> FromVersionResponse(
        VersionResponse const& Value);

    struct AuthRequest
    {
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    std::vector<uint8_t> FromAuthRequest(
        AuthRequest const& Value);

    struct UnixAuthRequest
    {
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        std::uint32_t NumericUserName; // n_uname
    };

    std::vector<uint8_t> FromUnixAuthRequest(
        UnixAuthRequest const& Value);

    struct AuthResponse
    {
        Qid AuthenticationUniqueId; // aqid
    };

    std::vector<uint8_t> FromAuthResponse(
        AuthResponse const& Value);

    struct AttachRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    std::vector<uint8_t> FromAttachRequest(
        AttachRequest const& Value);

    struct UnixAttachRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        std::uint32_t NumericUserName; // n_uname
    };

    std::vector<uint8_t> FromUnixAttachRequest(
        UnixAttachRequest const& Value);

    struct AttachResponse
    {
        Qid UniqueId; // qid
    };

    std::vector<uint8_t> FromAttachResponse(
        AttachResponse const& Value);

    struct ErrorResponse
    {
        std::string Message;
    };

    std::vector<uint8_t> FromErrorResponse(
        ErrorResponse const& Value);

    struct UnixErrorResponse
    {
        std::string Message;
        std::uint32_t Code;
    };

    std::vector<uint8_t> FromUnixErrorResponse(
        UnixErrorResponse const& Value);

    struct FlushRequest
    {
        std::uint16_t OldTag;
    };

    std::vector<uint8_t> FromFlushRequest(
        FlushRequest const& Value);

    // FlushResponse

    struct WalkRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t NewFileId; // newfid
        std::vector<std::string> Names;
    };

    std::vector<uint8_t> FromWalkRequest(
        WalkRequest const& Value);

    struct WalkResponse
    {
        std::vector<Qid> UniqueIds; // nwqid, wqid
    };

    std::vector<uint8_t> FromWalkResponse(
        WalkResponse const& Value);

    struct OpenRequest
    {
        std::uint32_t FileId; // fid
        std::uint8_t Mode; // MILE_CIRNO_PROTOCOL_OPEN_MODE
    };

    std::vector<uint8_t> FromOpenRequest(
        OpenRequest const& Value);

    struct OpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    std::vector<uint8_t> FromOpenResponse(
        OpenResponse const& Value);

    struct CreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
    };

    std::vector<uint8_t> FromCreateRequest(
        CreateRequest const& Value);

    struct UnixCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
        std::string UnixExtension; // extension
    };

    std::vector<uint8_t> FromUnixCreateRequest(
        UnixCreateRequest const& Value);

    struct CreateResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    std::vector<uint8_t> FromCreateResponse(
        CreateResponse const& Value);

    struct ReadRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    std::vector<uint8_t> FromReadRequest(
        ReadRequest const& Value);

    struct ReadResponse
    {
        std::vector<std::uint8_t> Data; // count, data
    };

    std::vector<uint8_t> FromReadResponse(
        ReadResponse const& Value);

    struct WriteRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::vector<std::uint8_t> Data; // count, data
    };

    std::vector<uint8_t> FromWriteRequest(
        WriteRequest const& Value);

    struct WriteResponse
    {
        std::uint32_t Count;
    };

    std::vector<uint8_t> FromWriteResponse(
        WriteResponse const& Value);

    struct ClunkRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromClunkRequest(
        ClunkRequest const& Value);

    // ClunkResponse

    struct RemoveRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromRemoveRequest(
        RemoveRequest const& Value);

    // RemoveResponse

    struct StatRequest
    {
        std::uint32_t FileId; // fid
    };

    std::vector<uint8_t> FromStatRequest(
        StatRequest const& Value);

    struct StatResponse
    {
        std::vector<Stat> Stat;
    };

    std::vector<uint8_t> FromStatResponse(
        StatResponse const& Value);

    struct WriteStatRequest
    {
        std::uint32_t FileId; // fid
        std::vector<Stat> Stat;
    };

    std::vector<uint8_t> FromWriteStatRequest(
        WriteStatRequest const& Value);
    
    // WriteStatResponse
}

#endif // ! MILE_CIRNO_CORE
