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

#include <span>
#include <string>
#include <vector>

namespace Mile::Cirno
{
    std::span<std::uint8_t> PopBytes(
        std::span<std::uint8_t>& Buffer,
        std::size_t const& Size);

    // No PushBytes because we have std::vector::insert.

    std::uint8_t PopUInt8(
        std::span<std::uint8_t>& Buffer);

    void PushUInt8(
        std::vector<std::uint8_t>& Buffer,
        std::uint8_t const& Value);

    std::uint16_t PopUInt16(
        std::span<std::uint8_t>& Buffer);

    void PushUInt16(
        std::vector<std::uint8_t>& Buffer,
        std::uint16_t const& Value);

    std::uint32_t PopUInt32(
        std::span<std::uint8_t>& Buffer);

    void PushUInt32(
        std::vector<std::uint8_t>& Buffer,
        std::uint32_t const& Value);

    std::uint64_t PopUInt64(
        std::span<std::uint8_t>& Buffer);

    void PushUInt64(
        std::vector<std::uint8_t>& Buffer,
        std::uint64_t const& Value);

    struct Header
    {
        std::uint32_t Size;
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_MESSAGE_TYPE
        std::uint16_t Tag;
    };

    const std::uint32_t HeaderSize =
        sizeof(std::uint32_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t);

    Header PopHeader(
        std::span<std::uint8_t>& Buffer);

    void PushHeader(
        std::vector<std::uint8_t>& Buffer,
        Header const& Value);

    // String

    std::string PopString(
        std::span<std::uint8_t>& Buffer);

    void PushString(
        std::vector<std::uint8_t>& Buffer,
        std::string const& Value);

    struct Qid
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_QID_TYPE
        std::uint32_t Version;
        std::uint64_t Path;
    };

    Qid PopQid(
        std::span<std::uint8_t>& Buffer);

    void PushQid(
        std::vector<std::uint8_t>& Buffer,
        Qid const& Value);

    struct DirectoryEntry
    {
        Qid UniqueId; // qid
        std::uint64_t Offset;
        std::uint8_t Type;
        std::string Name;
    };

    DirectoryEntry PopDirectoryEntry(
        std::span<std::uint8_t>& Buffer);

    void PushDirectoryEntry(
        std::vector<std::uint8_t>& Buffer,
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

    Stat PopStat(
        std::span<std::uint8_t>& Buffer);

    void PushStat(
        std::vector<std::uint8_t>& Buffer,
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

    UnixStat PopUnixStat(
        std::span<std::uint8_t>& Buffer);

    void PushUnixStat(
        std::vector<std::uint8_t>& Buffer,
        UnixStat const& Value);

    struct WindowsDirectoryEntry
    {
        DirectoryEntry Entry;
        Stat Stat;
    };

    WindowsDirectoryEntry PopWindowsDirectoryEntry(
        std::span<std::uint8_t>& Buffer);

    void PushWindowsDirectoryEntry(
        std::vector<std::uint8_t>& Buffer,
        WindowsDirectoryEntry const& Value);

    struct LinuxErrorResponse
    {
        std::uint32_t Code;
    };

    LinuxErrorResponse PopLinuxErrorResponse(
        std::span<std::uint8_t>& Buffer);

    struct StatFsRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushStatFsRequest(
        std::vector<std::uint8_t>& Buffer,
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

    StatFsResponse PopStatFsResponse(
        std::span<std::uint8_t>& Buffer);

    struct LinuxOpenRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    void PushLinuxOpenRequest(
        std::vector<std::uint8_t>& Buffer,
        LinuxOpenRequest const& Value);

    struct LinuxOpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    LinuxOpenResponse PopLinuxOpenResponse(
        std::span<std::uint8_t>& Buffer);

    struct LinuxCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
        std::uint32_t Mode;
        std::uint32_t Gid;
    };

    void PushLinuxCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        LinuxCreateRequest const& Value);

    struct LinuxCreateResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    LinuxCreateResponse PopLinuxCreateResponse(
        std::span<std::uint8_t>& Buffer);

    struct SymLinkRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::string Target;
        std::uint32_t Gid;
    };

    void PushSymLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        SymLinkRequest const& Value);

    struct SymLinkResponse
    {
        Qid UniqueId; // qid
    };

    SymLinkResponse PopSymLinkResponse(
        std::span<std::uint8_t>& Buffer);

    struct MkNodRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::string Name;
        std::uint32_t Mode;
        std::uint32_t Major;
        std::uint32_t Minor;
        std::uint32_t Gid;
    };

    void PushMkNodRequest(
        std::vector<std::uint8_t>& Buffer,
        MkNodRequest const& Value);

    struct MkNodResponse
    {
        Qid UniqueId; // qid
    };

    MkNodResponse PopMkNodResponse(
        std::span<std::uint8_t>& Buffer);

    struct RenameRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t DirectoryFid; // dfid
        std::string Name;
    };

    void PushRenameRequest(
        std::vector<std::uint8_t>& Buffer,
        RenameRequest const& Value);

    // RenameResponse

    struct ReadLinkRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushReadLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadLinkRequest const& Value);

    struct ReadLinkResponse
    {
        std::string Target;
    };

    ReadLinkResponse PopReadLinkResponse(
        std::span<std::uint8_t>& Buffer);

    struct GetAttrRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t RequestMask; // request_mask
    };

    void PushGetAttrRequest(
        std::vector<std::uint8_t>& Buffer,
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

    GetAttrResponse PopGetAttrResponse(
        std::span<std::uint8_t>& Buffer);

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

    void PushSetAttrRequest(
        std::vector<std::uint8_t>& Buffer,
        SetAttrRequest const& Value);

    // SetAttrResponse

    struct XattrWalkRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t NewFileId; // newfid
        std::string Name;
    };

    void PushXattrWalkRequest(
        std::vector<std::uint8_t>& Buffer,
        XattrWalkRequest const& Value);

    struct XattrWalkResponse
    {
        std::uint64_t Size;
    };

    XattrWalkResponse PopXattrWalkResponse(
        std::span<std::uint8_t>& Buffer);

    struct XattrCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint64_t Size;
        std::uint32_t Flags;
    };

    void PushXattrCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        XattrCreateRequest const& Value);

    // XattrCreateResponse

    struct ReadDirRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    void PushReadDirRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadDirRequest const& Value);

    struct ReadDirResponse
    {
        std::vector<DirectoryEntry> Data;
    };

    ReadDirResponse PopReadDirResponse(
        std::span<std::uint8_t>& Buffer);

    struct FsyncRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushFsyncRequest(
        std::vector<std::uint8_t>& Buffer,
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

    void PushLockRequest(
        std::vector<std::uint8_t>& Buffer,
        LockRequest const& Value);

    struct LockResponse
    {
        std::uint8_t Status; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_STATUS
    };

    LockResponse PopLockResponse(
        std::span<std::uint8_t>& Buffer);

    struct GetLockRequest
    {
        std::uint32_t FileId; // fid
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    void PushGetLockRequest(
        std::vector<std::uint8_t>& Buffer,
        GetLockRequest const& Value);

    struct GetLockResponse
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    GetLockResponse PopGetLockResponse(
        std::span<std::uint8_t>& Buffer);

    struct LinkRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::uint32_t FileId; // fid
        std::string Name;
    };

    void PushLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        LinkRequest const& Value);

    // LinkResponse

    struct MkDirRequest
    {
        std::uint32_t DirectoryFileId; // dfid
        std::string Name;
        std::uint32_t Mode;
        std::uint32_t Gid;
    };

    void PushMkDirRequest(
        std::vector<std::uint8_t>& Buffer,
        MkDirRequest const& Value);

    struct MkDirResponse
    {
        Qid UniqueId; // qid
    };

    MkDirResponse PopMkDirResponse(
        std::span<std::uint8_t>& Buffer);

    struct RenameAtRequest
    {
        std::uint32_t OldDirectoryFileId; // olddirfid
        std::string OldName;
        std::uint32_t NewDirectoryFileId; // newdirfid
        std::string NewName;
    };

    void PushRenameAtRequest(
        std::vector<std::uint8_t>& Buffer,
        RenameAtRequest const& Value);

    // RenameAtResponse

    struct UnlinkAtRequest
    {
        std::uint32_t DirectoryFileId; // dirfd
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    void PushUnlinkAtRequest(
        std::vector<std::uint8_t>& Buffer,
        UnlinkAtRequest const& Value);

    // UnlinkAtResponse

    struct VersionRequest
    {
        std::uint32_t MaximumMessageSize; // msize
        std::string ProtocolVersion; // version
    };

    void PushVersionRequest(
        std::vector<std::uint8_t>& Buffer,
        VersionRequest const& Value);

    struct VersionResponse
    {
        std::uint32_t MaximumMessageSize; // msize
        std::string ProtocolVersion; // version
    };

    VersionResponse PopVersionResponse(
        std::span<std::uint8_t>& Buffer);

    struct AuthRequest
    {
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    void PushAuthRequest(
        std::vector<std::uint8_t>& Buffer,
        AuthRequest const& Value);

    struct UnixAuthRequest
    {
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        std::uint32_t NumericUserName; // n_uname
    };

    void PushUnixAuthRequest(
        std::vector<std::uint8_t>& Buffer,
        UnixAuthRequest const& Value);

    struct AuthResponse
    {
        Qid AuthenticationUniqueId; // aqid
    };

    AuthResponse PopAuthResponse(
        std::span<std::uint8_t>& Buffer);

    struct AttachRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    void PushAttachRequest(
        std::vector<std::uint8_t>& Buffer,
        AttachRequest const& Value);

    struct UnixAttachRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        std::uint32_t NumericUserName; // n_uname
    };

    void PushUnixAttachRequest(
        std::vector<std::uint8_t>& Buffer,
        UnixAttachRequest const& Value);

    struct AttachResponse
    {
        Qid UniqueId; // qid
    };

    AttachResponse PopAttachResponse(
        std::span<std::uint8_t>& Buffer);

    struct ErrorResponse
    {
        std::string Message;
    };

    ErrorResponse PopErrorResponse(
        std::span<std::uint8_t>& Buffer);

    struct UnixErrorResponse
    {
        std::string Message;
        std::uint32_t Code;
    };

    UnixErrorResponse PopUnixErrorResponse(
        std::span<std::uint8_t>& Buffer);

    struct FlushRequest
    {
        std::uint16_t OldTag;
    };

    void PushFlushRequest(
        std::vector<std::uint8_t>& Buffer,
        FlushRequest const& Value);

    // FlushResponse

    struct WalkRequest
    {
        std::uint32_t FileId; // fid
        std::uint32_t NewFileId; // newfid
        std::vector<std::string> Names;
    };

    void PushWalkRequest(
        std::vector<std::uint8_t>& Buffer,
        WalkRequest const& Value);

    struct WalkResponse
    {
        std::vector<Qid> UniqueIds; // nwqid, wqid
    };

    WalkResponse PopWalkResponse(
        std::span<std::uint8_t>& Buffer);

    struct OpenRequest
    {
        std::uint32_t FileId; // fid
        std::uint8_t Mode; // MILE_CIRNO_PROTOCOL_OPEN_MODE
    };

    void PushOpenRequest(
        std::vector<std::uint8_t>& Buffer,
        OpenRequest const& Value);

    struct OpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    OpenResponse PopOpenResponse(
        std::span<std::uint8_t>& Buffer);

    struct CreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
    };

    void PushCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        CreateRequest const& Value);

    struct UnixCreateRequest
    {
        std::uint32_t FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
        std::string UnixExtension; // extension
    };

    void PushUnixCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        UnixCreateRequest const& Value);

    struct CreateResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    CreateResponse PopCreateResponse(
        std::span<std::uint8_t>& Buffer);

    struct ReadRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    void PushReadRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadRequest const& Value);

    struct ReadResponse
    {
        std::vector<std::uint8_t> Data; // count, data
    };

    ReadResponse PopReadResponse(
        std::span<std::uint8_t>& Buffer);

    struct WriteRequest
    {
        std::uint32_t FileId; // fid
        std::uint64_t Offset;
        std::vector<std::uint8_t> Data; // count, data
    };

    void PushWriteRequest(
        std::vector<std::uint8_t>& Buffer,
        WriteRequest const& Value);

    struct WriteResponse
    {
        std::uint32_t Count;
    };

    WriteResponse PopWriteResponse(
        std::span<std::uint8_t>& Buffer);

    struct ClunkRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushClunkRequest(
        std::vector<std::uint8_t>& Buffer,
        ClunkRequest const& Value);

    // ClunkResponse

    struct RemoveRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushRemoveRequest(
        std::vector<std::uint8_t>& Buffer,
        RemoveRequest const& Value);

    // RemoveResponse

    struct StatRequest
    {
        std::uint32_t FileId; // fid
    };

    void PushStatRequest(
        std::vector<std::uint8_t>& Buffer,
        StatRequest const& Value);

    struct StatResponse
    {
        std::vector<Stat> Stat;
    };

    StatResponse PopStatResponse(
        std::span<std::uint8_t>& Buffer);

    struct WriteStatRequest
    {
        std::uint32_t FileId; // fid
        std::vector<Stat> Stat;
    };

    void PushWriteStatRequest(
        std::vector<std::uint8_t>& Buffer,
        WriteStatRequest const& Value);

    // WriteStatResponse
}

#endif // ! MILE_CIRNO_CORE
