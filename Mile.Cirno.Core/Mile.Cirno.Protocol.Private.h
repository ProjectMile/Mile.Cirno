/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Protocol.Private.h
 * PURPOSE:   Definition for Plan 9 File System Protocol
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#ifndef MILE_CIRNO_PROTOCOL_PRIVATE
#define MILE_CIRNO_PROTOCOL_PRIVATE

#include <Mile.Cirno.Protocol.h>

#include <cstdint>

#include <string>
#include <vector>

namespace Mile::Cirno
{
    struct Header
    {
        std::uint32_t Size;
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_MESSAGE_TYPE
        MILE_CIRNO_TAG Tag;
    };

    // String

    struct Qid
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_QID_TYPE
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
    };

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
        MILE_CIRNO_FID FileId; // fid
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
        MILE_CIRNO_FID FileId; // fid
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    struct LinuxOpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    struct LinuxCreateRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
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
        MILE_CIRNO_FID FileId; // fid
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
        MILE_CIRNO_FID DirectoryFileId; // dfid
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
        MILE_CIRNO_FID FileId; // fid
        MILE_CIRNO_FID DirectoryFid;
        std::string Name;
    };

    // RenameResponse

    struct ReadLinkRequest
    {
        MILE_CIRNO_FID FileId; // fid
    };

    struct ReadLinkResponse
    {
        std::string Target;
    };

    struct GetAttrRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint64_t RequestMask; // request_mask
    };

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

    struct SetAttrRequest
    {
        MILE_CIRNO_FID FileId; // fid
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

    // SetAttrResponse

    struct XattrWalkRequest
    {
        MILE_CIRNO_FID FileId; // fid
        MILE_CIRNO_FID NewFid;
        std::string Name;
    };

    struct XattrWalkResponse
    {
        std::uint64_t Size;
    };

    struct XattrCreateRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::string Name;
        std::uint64_t Size;
        std::uint32_t Flags;
    };

    // XattrCreateResponse

    struct ReadDirRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    struct ReadDirResponse
    {
        std::vector<DirectoryEntry> Data;
    };

    struct FsyncRequest
    {
        MILE_CIRNO_FID FileId; // fid
    };

    // FsyncResponse

    struct LockRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_FLAGS
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    struct LockResponse
    {
        std::uint8_t Status; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_STATUS
    };

    struct GetLockRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    struct GetLockResponse
    {
        std::uint8_t Type; // MILE_CIRNO_PROTOCOL_LINUX_LOCK_TYPE
        std::uint64_t Start;
        std::uint64_t Length;
        std::uint32_t ProcessId;
        std::string ClientId;
    };

    struct LinkRequest
    {
        MILE_CIRNO_FID DirectoryFileId; // dfid
        MILE_CIRNO_FID FileId; // fid
        std::string Name;
    };

    // LinkResponse

    struct MkDirRequest
    {
        MILE_CIRNO_FID DirectoryFileId; // dfid
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
        MILE_CIRNO_FID OldDirectoryFileId; // olddirfid
        std::string OldName;
        MILE_CIRNO_FID NewDirectoryFileId; // newdirfid
        std::string NewName;
    };

    // RenameAtResponse

    struct UnlinkAtRequest
    {
        MILE_CIRNO_FID DirectoryFileId; // dirfd
        std::string Name;
        std::uint32_t Flags; // MILE_CIRNO_PROTOCOL_LINUX_OPEN_CREATE_FLAGS
    };

    // UnlinkAtResponse

    struct VersionRequest
    {
        std::uint32_t MaximumMessageSize;
        std::string ProtocolVersion;
    };

    struct VersionResponse
    {
        std::uint32_t MaximumMessageSize;
        std::string ProtocolVersion;
    };

    struct AuthRequest
    {
        MILE_CIRNO_FID AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    struct UnixAuthRequest
    {
        MILE_CIRNO_FID AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        MILE_CIRNO_NUNAME NumericUserName; // n_uname
    };

    struct AuthResponse
    {
        Qid AuthenticationUniqueId; // aqid
    };

    struct AttachRequest
    {
        MILE_CIRNO_FID FileId; // fid
        MILE_CIRNO_FID AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
    };

    struct UnixAttachRequest
    {
        MILE_CIRNO_FID FileId; // fid
        MILE_CIRNO_FID AuthenticationFileId; // afid
        std::string UserName; // uname
        std::string AccessName; // aname
        MILE_CIRNO_NUNAME NumericUserName; // n_uname
    };

    struct AttachResponse
    {
        Qid UniqueId; // qid
    };

    struct ErrorResponse
    {
        std::string Message;
    };

    struct UnixErrorResponse
    {
        std::string Message;
        std::uint32_t Code;
    };

    struct FlushRequest
    {
        MILE_CIRNO_TAG OldTag;
    };

    // FlushResponse

    struct WalkRequest
    {
        MILE_CIRNO_FID FileId; // fid
        MILE_CIRNO_FID NewFileId; // newfid
        std::vector<std::string> Names;
    };

    struct WalkResponse
    {
        std::vector<Qid> UniqueIds; // nwqid, wqid
    };

    struct OpenRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint8_t Mode; // MILE_CIRNO_PROTOCOL_OPEN_MODE
    };

    struct OpenResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    struct CreateRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
    };

    struct UnixCreateRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::string Name;
        std::uint32_t Permission; // perm, MILE_CIRNO_PROTOCOL_PERMISSION_MODE
        std::uint32_t Mode;
        std::string UnixExtension; // extension
    };

    struct CreateResponse
    {
        Qid UniqueId; // qid
        std::uint32_t IoUnit;
    };

    struct ReadRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    struct ReadResponse
    {
        std::vector<std::uint8_t> Data;
    };

    struct WriteRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint64_t Offset;
        std::vector<std::uint8_t> Data;
    };

    struct WriteResponse
    {
        std::uint32_t Count;
    };

    struct ClunkRequest
    {
        MILE_CIRNO_FID FileId; // fid
    };

    // ClunkResponse

    struct RemoveRequest
    {
        MILE_CIRNO_FID FileId; // fid
    };

    // RemoveResponse

    struct StatRequest
    {
        MILE_CIRNO_FID FileId; // fid
    };

    struct StatResponse
    {
        std::vector<Stat> Stat;
    };

    struct WriteStatRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::vector<Stat> Stat;
    };

    // WriteStatResponse

    struct AccessRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint32_t Flags;
    };

    // AccessResponse

    struct WindowsReadDirRequest
    {
        MILE_CIRNO_FID FileId; // fid
        std::uint64_t Offset;
        std::uint32_t Count;
    };

    struct WindowsReadDirResponse
    {
        std::vector<WindowsDirectoryEntry> Data; // count, data
    };

    // WindowsOpenRequest

    // WindowsOpenResponse
}

#endif // ! MILE_CIRNO_PROTOCOL_PRIVATE
