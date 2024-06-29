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

#include <stdint.h>

#include <array>
#include <string>

#include <Mile.Cirno.Protocol.h>

namespace Mile
{
namespace Cirno
{

struct Header
{
    uint8_t Type;
    uint16_t Tag;
    uint32_t Fid;
};

struct Qid
{
    uint8_t Type;
    uint32_t Version;
    uint64_t Path;
};

struct DirectoryEntry
{
    Qid Qid;
    uint64_t Offset;
    uint8_t Type;
    std::string Name;
};

struct Stat
{
    uint16_t Type;
    uint32_t Device;
    Qid Qid;
    uint32_t Mode;
    uint32_t AccessTime;
    uint32_t ModifiedTime;
    uint64_t Length;
    std::string Name;
    std::string Uid;
    std::string Gid;
    std::string ModifiedUid;
    std::string extension;
    uint32_t nUid;
    uint32_t nGid;
    uint32_t nModifiedUid;
};

struct WindowsDirectoryEntry
{
    DirectoryEntry Entry;
    Stat Stat;
};

struct StatFs
{
    uint32_t Type;
    uint32_t BSize;
    uint64_t Blocks;
    uint64_t BFree;
    uint64_t BAvail;
    uint64_t Files;
    uint64_t FFree;
    uint64_t FSid;
    uint32_t NameLen;
};
struct Version
{
    uint32_t Size;
    std::string Version;
};

// struct LinuxErrorRequest
// {
//     // empty
// };

struct LinuxErrorResponse
{
    uint32_t ECode;
};

struct StatFsRequest
{
    uint32_t Fid;
};

struct StatFsResponse
{
    uint32_t Type;
    uint32_t BSize;
    uint64_t Blocks;
    uint64_t BFree;
    uint64_t BAvail;
    uint64_t Files;
    uint64_t FFree;
    uint64_t Fid;
    uint32_t NameLen;
};

struct LinuxOpenRequest
{
    uint32_t Fid;
    uint32_t Flags;
};

struct LinuxOpenResponse
{
    Qid Qid;
    uint32_t IOUnit;
};

struct LinuxCreateRequest
{
    uint32_t Fid;
    std::string Name;
    uint32_t Flags;
    uint32_t Mode;
    uint32_t Gid;
};

struct LinuxCreateResponse
{
    Qid Qid;
    uint32_t IOUnit;
};

struct SymLinkRequest
{
    uint32_t Fid;
    std::string Name;
    std::string SymTarget;
    uint32_t Gid;
};
struct SymLinkResponse
{
    Qid Qid;
};
struct MkNodRequest
{
    uint32_t DirFid;
    std::string Name;
    uint32_t Mode;
    uint32_t Major;
    uint32_t Minor;
    uint32_t Gid;
};

struct MkNodResponse
{
    Qid Qid;
};

struct RenameRequest
{
    uint32_t Fid;
    uint32_t DirFid;
    std::string Name;
};

// struct RenameResponse
// {
//     // empty
// };

struct ReadLinkRequest
{
    uint32_t Fid;
};

struct ReadLinkResponse
{
    std::string Target;
};

struct GetAttrRequest
{
    uint32_t Fid;
    uint64_t RequestMask;
};

struct GetAttrResponse
{
    uint64_t Valid;
    Qid Qid;
    uint32_t Mode;
    uint32_t Uid;
    uint32_t Gid;
    uint64_t nLink;
    uint64_t RDev;
    uint64_t Size;
    uint64_t BSize;
    uint64_t Blocks;

    uint64_t AccessTimeSec;
    uint64_t AccessTimeNSec;
    uint64_t ModifiedTimeSec;
    uint64_t ModifiedTimeNSec;
    uint64_t ChangeTimeSec;
    uint64_t ChangeTimeNSec;
    uint64_t BirthTimeSec;
    uint64_t BirthTimeNSec;
    uint64_t Generation;
    uint64_t DataVersion;
};

struct SetAttrRequest
{
    uint32_t Fid;
    uint32_t Valid;
    uint32_t Mode;
    uint32_t Uid;
    uint32_t Gid;
    uint64_t Size;
    // valid & LinuxSetAttrFlagAccessTimeSet
    uint64_t AtimeSec;
    // else
    uint64_t AtimeNSec;
    // valid & LinuxSetAttrFlagModifiedTimeSet
    uint64_t MtimeSec;
    // else
    uint64_t MtimeNSec;
};

// struct SetAttrResponse
// {
//     // empty
// };

struct XAttrWalkRequest
{
    uint32_t Fid;
    uint32_t NewFid;
    std::string Name;
};

struct XAttrWalkResponse
{
    uint64_t Size;
};

struct XAttrCreateRequest
{
    uint32_t Fid;
    std::string Name;
    uint64_t AttrSize;
    uint32_t Flags;
};

// struct XAttrCreateResponse
// {
//     // empty
// };

struct ReadDirRequest
{
    uint32_t Fid;
    uint64_t Offset;
    uint32_t Count;
};

struct ReadDirResonse
{
    uint32_t Count;
    std::array<DirectoryEntry, MILE_CIRNO_MAXWELEM> Data;
};

struct FsyncRequest
{
    uint32_t Fid;
};

// struct FsyncResponse
// {
// };

struct LockRequest
{
    uint32_t Fid;
    uint8_t Type;
    uint32_t Flags;
    uint64_t Start;
    uint64_t Length;
    uint32_t ProcId;
    std::string CliendId;
};

struct LockResponse
{
    uint8_t Status;
};

struct GetLockRequest
{
    uint32_t Fid;
    uint8_t Type;
    uint64_t Start;
    uint64_t Length;
    uint32_t ProcId;
    std::string CliendId;
};

struct GetLockResponse
{
    uint8_t Type;
    uint64_t Start;
    uint64_t Length;
    uint32_t ProcId;
    std::string CliendId;
};

struct LinkRequest
{
    uint32_t Fid;
    uint32_t DirFid;
    std::string Name;
};

// struct LinkResponse
// {
// };

struct MkdirRequest
{
    uint32_t DirFid;
    std::string Name;
    uint32_t Mode;
    uint32_t Gid;
};

struct MkdirResponse
{
    Qid Qid;
};

struct RenameAtRequest
{
    uint32_t OldDirFid;
    std::string OldName;
    uint32_t NewDirFid;
    std::string NewName;
};

// struct RenameAtResponse
// {
//     // empty
// };

struct UnlinkAtRequest
{
    uint32_t DirFd;
    std::string Name;
    uint32_t Flags;
};

// struct UnlinkAtResponse
// {
//     // empty
// };

struct VersionRequest
{
    uint32_t Size;
    std::string Version;
};

struct VersionResponse
{
    uint32_t Size;
    std::string Version;
};

struct AuthRequest
{
    uint32_t AFid;
    std::string UName;
    std::string AName;
    uint32_t nUName;
};

struct AuthResponse
{
    Qid Qid;
};

struct AttachRequest
{
    uint32_t Fid;
    uint32_t AFid;
    std::string UName;
    std::string AName;
    uint32_t nUName;
};

struct AttachResponse
{
    Qid Qid;
};

// struct ErrorRequest
// {
//     // illegal
// };

struct ErrorResponse
{
    // 9P2000.L
    std::string EName;
    uint32_t Errno;
};

struct FlushRequest
{
    uint16_t OldTag;
};

// struct FlushResponse
// {
//     // empty
// };

struct WalkRequest
{
    uint32_t Fid;
    uint32_t NewFid;
    uint16_t nWName;
    std::string WName;
};

struct WalkResponse
{
    uint16_t nWQid;
    std::array<Qid, MILE_CIRNO_MAXWELEM> WQid;
};

// struct OpenRequest
// {
//     // in 9P2000.L not used.
// };

// struct OpenResponse
// {
//     // in 9P2000.L not used.
// };

// struct CreateRequest
// {
//     // in 9P2000.L not used.
// };

// struct CreateResponse
// {
//     // in 9P2000.L not used.

// };

struct ReadRequest
{
    uint32_t Fid;
    uint64_t Offset;
    uint32_t Count;
};

struct ReadResponse
{
    uint32_t Count;
    // todo: data[Count]
};

struct WriteRequest
{
    uint32_t Fid;
    uint64_t Offset;
    uint32_t Count;
    // todo: data[count]
};

struct WriteResponse
{
    uint32_t Count;
};

struct ClunkRequest
{
    uint32_t Fid;
};

// struct ClunkResponse
// {
//     // empty
// };

struct RemoveRequest
{
    uint32_t Fid;
};

// struct RemoveResponse
// {
//     // empty
// };

// struct StatRequest
// {
//     // 9P2000.L not used.
// };

// struct StatResponse
// {
//     // 9P2000.L not used.
// };

// struct StatResponse
// {
//     // 9P2000.L not used.
// };

// struct WriteStatRequest
// {
//     // 9P2000.L not used.
// };

// struct WriteStatResponse
// {
//     // 9P2000.L not used.
// };

struct AccessRequest
{
    uint32_t Fid;
    uint32_t Flags;
};

// struct AccessResponse
// {
//     // empty
// };

// todo: 9P2000.W extension

} // namespace Cirno

} // namespace Mile

#endif // ! MILE_CIRNO_PROTOCOL_PRIVATE
