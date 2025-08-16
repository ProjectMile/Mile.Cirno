/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Cirno.Protocol.Parser.cpp
 * PURPOSE:    Implementation for Plan 9 File System Protocol Parser
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#include "Mile.Cirno.Protocol.Parser.h"

std::span<std::uint8_t> Mile::Cirno::PopBytes(
    std::span<std::uint8_t>& Buffer,
    std::size_t const& Size)
{
    std::span<std::uint8_t> Result = Buffer.subspan(0, Size);
    Buffer = Buffer.subspan(Size);
    return Result;
}

std::uint8_t Mile::Cirno::PopUInt8(
    std::span<std::uint8_t>& Buffer)
{
    std::span<std::uint8_t> Content =
        Mile::Cirno::PopBytes(Buffer, sizeof(std::uint8_t));
    return Content[0];
}

void Mile::Cirno::PushUInt8(
    std::vector<std::uint8_t>& Buffer,
    std::uint8_t const& Value)
{
    Buffer.emplace_back(Value);
}

std::uint16_t Mile::Cirno::PopUInt16(
    std::span<std::uint8_t>& Buffer)
{
    std::span<std::uint8_t> Content =
        Mile::Cirno::PopBytes(Buffer, sizeof(std::uint16_t));
    std::uint8_t* RawContent = Content.data();
    return
        (static_cast<std::uint16_t>(RawContent[0])) |
        (static_cast<std::uint16_t>(RawContent[1]) << 8);
}

void Mile::Cirno::PushUInt16(
    std::vector<std::uint8_t>& Buffer,
    std::uint16_t const& Value)
{
    std::size_t PreviousEndIndex = Buffer.size();
    Buffer.resize(PreviousEndIndex + sizeof(std::uint16_t));
    std::uint8_t* RawContent = &Buffer[PreviousEndIndex];
    RawContent[0] = static_cast<std::uint8_t>(Value);
    RawContent[1] = static_cast<std::uint8_t>(Value >> 8);
}

std::uint32_t Mile::Cirno::PopUInt32(
    std::span<std::uint8_t>& Buffer)
{
    std::span<std::uint8_t> Content =
        Mile::Cirno::PopBytes(Buffer, sizeof(std::uint32_t));
    std::uint8_t* RawContent = Content.data();
    return
        (static_cast<std::uint32_t>(RawContent[0])) |
        (static_cast<std::uint32_t>(RawContent[1]) << 8) |
        (static_cast<std::uint32_t>(RawContent[2]) << 16) |
        (static_cast<std::uint32_t>(RawContent[3]) << 24);
}

void Mile::Cirno::PushUInt32(
    std::vector<std::uint8_t>& Buffer,
    std::uint32_t const& Value)
{
    std::size_t PreviousEndIndex = Buffer.size();
    Buffer.resize(PreviousEndIndex + sizeof(std::uint32_t));
    std::uint8_t* RawContent = &Buffer[PreviousEndIndex];
    RawContent[0] = static_cast<std::uint8_t>(Value);
    RawContent[1] = static_cast<std::uint8_t>(Value >> 8);
    RawContent[2] = static_cast<std::uint8_t>(Value >> 16);
    RawContent[3] = static_cast<std::uint8_t>(Value >> 24);
}

std::uint64_t Mile::Cirno::PopUInt64(
    std::span<std::uint8_t>& Buffer)
{
    std::span<std::uint8_t> Content =
        Mile::Cirno::PopBytes(Buffer, sizeof(std::uint64_t));
    std::uint8_t* RawContent = Content.data();
    return
        (static_cast<std::uint64_t>(RawContent[0])) |
        (static_cast<std::uint64_t>(RawContent[1]) << 8) |
        (static_cast<std::uint64_t>(RawContent[2]) << 16) |
        (static_cast<std::uint64_t>(RawContent[3]) << 24) |
        (static_cast<std::uint64_t>(RawContent[4]) << 32) |
        (static_cast<std::uint64_t>(RawContent[5]) << 40) |
        (static_cast<std::uint64_t>(RawContent[6]) << 48) |
        (static_cast<std::uint64_t>(RawContent[7]) << 56);
}

void Mile::Cirno::PushUInt64(
    std::vector<std::uint8_t>& Buffer,
    std::uint64_t const& Value)
{
    std::size_t PreviousEndIndex = Buffer.size();
    Buffer.resize(PreviousEndIndex + sizeof(std::uint64_t));
    std::uint8_t* RawContent = &Buffer[PreviousEndIndex];
    RawContent[0] = static_cast<std::uint8_t>(Value);
    RawContent[1] = static_cast<std::uint8_t>(Value >> 8);
    RawContent[2] = static_cast<std::uint8_t>(Value >> 16);
    RawContent[3] = static_cast<std::uint8_t>(Value >> 24);
    RawContent[4] = static_cast<std::uint8_t>(Value >> 32);
    RawContent[5] = static_cast<std::uint8_t>(Value >> 40);
    RawContent[6] = static_cast<std::uint8_t>(Value >> 48);
    RawContent[7] = static_cast<std::uint8_t>(Value >> 56);
}

Mile::Cirno::Header Mile::Cirno::PopHeader(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::Header Result;
    Result.Size = Mile::Cirno::PopUInt32(Buffer) - Mile::Cirno::HeaderSize;
    Result.Type = Mile::Cirno::PopUInt8(Buffer);
    Result.Tag = Mile::Cirno::PopUInt16(Buffer);
    return Result;
}

void Mile::Cirno::PushHeader(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::Header const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.Size + Mile::Cirno::HeaderSize);
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushUInt16(Buffer, Value.Tag);
}

std::string Mile::Cirno::PopString(
    std::span<std::uint8_t>& Buffer)
{
    std::uint16_t Length = Mile::Cirno::PopUInt16(Buffer);
    std::span<std::uint8_t> Content = Mile::Cirno::PopBytes(Buffer, Length);
    return std::string(Content.begin(), Content.end());
}

void Mile::Cirno::PushString(
    std::vector<std::uint8_t>& Buffer,
    std::string const& Value)
{
    Mile::Cirno::PushUInt16(Buffer, static_cast<std::uint16_t>(Value.size()));
    Buffer.insert(Buffer.end(), Value.begin(), Value.end());
}

Mile::Cirno::Qid Mile::Cirno::PopQid(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::Qid Result;
    Result.Type = Mile::Cirno::PopUInt8(Buffer);
    Result.Version = Mile::Cirno::PopUInt32(Buffer);
    Result.Path = Mile::Cirno::PopUInt64(Buffer);
    return Result;
}

void Mile::Cirno::PushQid(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::Qid const& Value)
{
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushUInt32(Buffer, Value.Version);
    Mile::Cirno::PushUInt64(Buffer, Value.Path);
}

Mile::Cirno::DirectoryEntry Mile::Cirno::PopDirectoryEntry(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::DirectoryEntry Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.Offset = Mile::Cirno::PopUInt64(Buffer);
    Result.Type = Mile::Cirno::PopUInt8(Buffer);
    Result.Name = Mile::Cirno::PopString(Buffer);
    return Result;
}

void Mile::Cirno::PushDirectoryEntry(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::DirectoryEntry const& Value)
{
    Mile::Cirno::PushQid(Buffer, Value.UniqueId);
    Mile::Cirno::PushUInt64(Buffer, Value.Offset);
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushString(Buffer, Value.Name);
}

Mile::Cirno::Stat Mile::Cirno::PopStat(
    std::span<std::uint8_t>& Buffer)
{
    std::uint16_t Size = Mile::Cirno::PopUInt16(Buffer);
    std::span<std::uint8_t> Content = Mile::Cirno::PopBytes(Content, Size);

    Mile::Cirno::Stat Result;
    Result.Type = Mile::Cirno::PopUInt16(Buffer);
    Result.Dev = Mile::Cirno::PopUInt32(Buffer);
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.Mode = Mile::Cirno::PopUInt32(Buffer);
    Result.LastAccessTime = Mile::Cirno::PopUInt32(Buffer);
    Result.LastWriteTime = Mile::Cirno::PopUInt32(Buffer);
    Result.FileSize = Mile::Cirno::PopUInt64(Buffer);
    Result.FileName = Mile::Cirno::PopString(Buffer);
    Result.OwnerUserId = Mile::Cirno::PopString(Buffer);
    Result.GroupId = Mile::Cirno::PopString(Buffer);
    Result.LastWriteUserId = Mile::Cirno::PopString(Buffer);
    // 9P2000.u
    if (!Buffer.empty())
    {
        Result.UnixExtension = Mile::Cirno::PopString(Buffer);
        Result.NumericOwnerUserId = Mile::Cirno::PopUInt32(Buffer);
        Result.NumericGroupId = Mile::Cirno::PopUInt32(Buffer);
        Result.NumericLastWriteUserId = Mile::Cirno::PopUInt32(Buffer);
    }
    return Result;
}

void Mile::Cirno::PushStat(
    std::vector<std::uint8_t>& Buffer,
    Stat const& Value)
{
    std::vector<std::uint8_t> Content;

    Mile::Cirno::PushUInt16(Content, Value.Type);
    Mile::Cirno::PushUInt32(Content, Value.Dev);
    Mile::Cirno::PushQid(Content, Value.UniqueId);
    Mile::Cirno::PushUInt32(Content, Value.Mode);
    Mile::Cirno::PushUInt32(Content, Value.LastAccessTime);
    Mile::Cirno::PushUInt32(Content, Value.LastWriteTime);
    Mile::Cirno::PushUInt64(Content, Value.FileSize);
    Mile::Cirno::PushString(Content, Value.FileName);
    Mile::Cirno::PushString(Content, Value.OwnerUserId);
    Mile::Cirno::PushString(Content, Value.GroupId);
    Mile::Cirno::PushString(Content, Value.LastWriteUserId);
    // 9P2000.u
    Mile::Cirno::PushString(Content, Value.UnixExtension);
    // 9P2000.u
    Mile::Cirno::PushUInt32(Content, Value.NumericOwnerUserId);
    // 9P2000.u
    Mile::Cirno::PushUInt32(Content, Value.NumericGroupId);
    // 9P2000.u
    Mile::Cirno::PushUInt32(Content, Value.NumericLastWriteUserId);

    Mile::Cirno::PushUInt16(Buffer, static_cast<std::uint16_t>(Content.size()));
    Buffer.insert(Buffer.end(), Content.begin(), Content.end());
}

Mile::Cirno::WindowsDirectoryEntry Mile::Cirno::PopWindowsDirectoryEntry(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::WindowsDirectoryEntry Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.Offset = Mile::Cirno::PopUInt64(Buffer);
    Result.Type = Mile::Cirno::PopUInt8(Buffer);
    Result.Name = Mile::Cirno::PopString(Buffer);
    Result.Mode = Mile::Cirno::PopUInt32(Buffer);
    Result.OwnerUserId = Mile::Cirno::PopUInt32(Buffer);
    Result.GroupId = Mile::Cirno::PopUInt32(Buffer);
    Result.NumberOfHardLinks = Mile::Cirno::PopUInt64(Buffer);
    Result.DeviceId = Mile::Cirno::PopUInt64(Buffer);
    Result.FileSize = Mile::Cirno::PopUInt64(Buffer);
    Result.BlockSize = Mile::Cirno::PopUInt64(Buffer);
    Result.AllocatedBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.LastAccessTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastAccessTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastWriteTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastWriteTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.ChangeTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.ChangeTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    return Result;
}

void Mile::Cirno::PushWindowsDirectoryEntry(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::WindowsDirectoryEntry const& Value)
{
    Mile::Cirno::PushQid(Buffer, Value.UniqueId);
    Mile::Cirno::PushUInt64(Buffer, Value.Offset);
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.OwnerUserId);
    Mile::Cirno::PushUInt32(Buffer, Value.GroupId);
    Mile::Cirno::PushUInt64(Buffer, Value.NumberOfHardLinks);
    Mile::Cirno::PushUInt64(Buffer, Value.DeviceId);
    Mile::Cirno::PushUInt64(Buffer, Value.FileSize);
    Mile::Cirno::PushUInt64(Buffer, Value.BlockSize);
    Mile::Cirno::PushUInt64(Buffer, Value.AllocatedBlocks);
    Mile::Cirno::PushUInt64(Buffer, Value.LastAccessTimeSeconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastAccessTimeNanoseconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastWriteTimeSeconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastWriteTimeNanoseconds);
    Mile::Cirno::PushUInt64(Buffer, Value.ChangeTimeSeconds);
    Mile::Cirno::PushUInt64(Buffer, Value.ChangeTimeNanoseconds);
}

Mile::Cirno::LinuxErrorResponse Mile::Cirno::PopLinuxErrorResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::LinuxErrorResponse Result;
    Result.Code = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushStatFsRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::StatFsRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

Mile::Cirno::StatFsResponse Mile::Cirno::PopStatFsResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::StatFsResponse Result;
    Result.FileSystemType = Mile::Cirno::PopUInt32(Buffer);
    Result.BlockSize = Mile::Cirno::PopUInt32(Buffer);
    Result.TotalBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.FreeBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.AvailableBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.TotalFiles = Mile::Cirno::PopUInt64(Buffer);
    Result.FreeFileNodes = Mile::Cirno::PopUInt64(Buffer);
    Result.FileSystemId = Mile::Cirno::PopUInt64(Buffer);
    Result.MaximumFileNameLength = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushLinuxOpenRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::LinuxOpenRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.Flags);
}

Mile::Cirno::LinuxOpenResponse Mile::Cirno::PopLinuxOpenResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::LinuxOpenResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.IoUnit = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushLinuxCreateRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::LinuxCreateRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Flags);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.Gid);
}

Mile::Cirno::LinuxCreateResponse Mile::Cirno::PopLinuxCreateResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::LinuxCreateResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.IoUnit = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushSymLinkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::SymLinkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushString(Buffer, Value.Target);
    Mile::Cirno::PushUInt32(Buffer, Value.Gid);
}

Mile::Cirno::SymLinkResponse Mile::Cirno::PopSymLinkResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::SymLinkResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    return Result;
}

void Mile::Cirno::PushMkNodRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::MkNodRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.DirectoryFileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.Major);
    Mile::Cirno::PushUInt32(Buffer, Value.Minor);
    Mile::Cirno::PushUInt32(Buffer, Value.Gid);
}

Mile::Cirno::MkNodResponse Mile::Cirno::PopMkNodResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::MkNodResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    return Result;
}

void Mile::Cirno::PushRenameRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::RenameRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.DirectoryFid);
    Mile::Cirno::PushString(Buffer, Value.Name);
}

void Mile::Cirno::PushReadLinkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::ReadLinkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

Mile::Cirno::ReadLinkResponse Mile::Cirno::PopReadLinkResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::ReadLinkResponse Result;
    Result.Target = Mile::Cirno::PopString(Buffer);
    return Result;
}

void Mile::Cirno::PushGetAttrRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::GetAttrRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt64(Buffer, Value.RequestMask);
}

Mile::Cirno::GetAttrResponse Mile::Cirno::PopGetAttrResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::GetAttrResponse Result;
    Result.Valid = Mile::Cirno::PopUInt64(Buffer);
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.Mode = Mile::Cirno::PopUInt32(Buffer);
    Result.OwnerUserId = Mile::Cirno::PopUInt32(Buffer);
    Result.GroupId = Mile::Cirno::PopUInt32(Buffer);
    Result.NumberOfHardLinks = Mile::Cirno::PopUInt64(Buffer);
    Result.DeviceId = Mile::Cirno::PopUInt64(Buffer);
    Result.FileSize = Mile::Cirno::PopUInt64(Buffer);
    Result.BlockSize = Mile::Cirno::PopUInt64(Buffer);
    Result.AllocatedBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.LastAccessTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastAccessTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastWriteTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.LastWriteTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.ChangeTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.ChangeTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.BirthTimeSeconds = Mile::Cirno::PopUInt64(Buffer);
    Result.BirthTimeNanoseconds = Mile::Cirno::PopUInt64(Buffer);
    Result.Generation = Mile::Cirno::PopUInt64(Buffer);
    Result.DataVersion = Mile::Cirno::PopUInt64(Buffer);
    return Result;
}

void Mile::Cirno::PushSetAttrRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::SetAttrRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.Valid);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.OwnerUserId);
    Mile::Cirno::PushUInt32(Buffer, Value.GroupId);
    Mile::Cirno::PushUInt64(Buffer, Value.FileSize);
    Mile::Cirno::PushUInt64(Buffer, Value.LastAccessTimeSeconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastAccessTimeNanoseconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastWriteTimeSeconds);
    Mile::Cirno::PushUInt64(Buffer, Value.LastWriteTimeNanoseconds);
}

void Mile::Cirno::PushXattrWalkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::XattrWalkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.NewFileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
}

Mile::Cirno::XattrWalkResponse Mile::Cirno::PopXattrWalkResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::XattrWalkResponse Result;
    Result.Size = PopUInt64(Buffer);
    return Result;
}

void Mile::Cirno::PushXattrCreateRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::XattrCreateRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt64(Buffer, Value.Size);
    Mile::Cirno::PushUInt32(Buffer, Value.Flags);
}

void Mile::Cirno::PushReadDirRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::ReadDirRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt64(Buffer, Value.Offset);
    Mile::Cirno::PushUInt32(Buffer, Value.Count);
}

Mile::Cirno::ReadDirResponse Mile::Cirno::PopReadDirResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::ReadDirResponse Result;
    // Discard the unused Length field.
    Mile::Cirno::PopUInt32(Buffer);
    while (!Buffer.empty())
    {
        Result.Data.push_back(Mile::Cirno::PopDirectoryEntry(Buffer));
    }
    return Result;
}

void Mile::Cirno::PushFsyncRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::FsyncRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

void Mile::Cirno::PushLockRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::LockRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushUInt32(Buffer, Value.Flags);
    Mile::Cirno::PushUInt64(Buffer, Value.Start);
    Mile::Cirno::PushUInt64(Buffer, Value.Length);
    Mile::Cirno::PushUInt32(Buffer, Value.ProcessId);
    Mile::Cirno::PushString(Buffer, Value.ClientId);
}

Mile::Cirno::LockResponse Mile::Cirno::PopLockResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::LockResponse Result;
    Result.Status = PopUInt8(Buffer);
    return Result;
}

void Mile::Cirno::PushGetLockRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::GetLockRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt8(Buffer, Value.Type);
    Mile::Cirno::PushUInt64(Buffer, Value.Start);
    Mile::Cirno::PushUInt64(Buffer, Value.Length);
    Mile::Cirno::PushUInt32(Buffer, Value.ProcessId);
    Mile::Cirno::PushString(Buffer, Value.ClientId);
}

Mile::Cirno::GetLockResponse Mile::Cirno::PopGetLockResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::GetLockResponse Result;
    Result.Type = Mile::Cirno::PopUInt8(Buffer);
    Result.Start = Mile::Cirno::PopUInt64(Buffer);
    Result.Length = Mile::Cirno::PopUInt64(Buffer);
    Result.ProcessId = Mile::Cirno::PopUInt32(Buffer);
    Result.ClientId = Mile::Cirno::PopString(Buffer);
    return Result;
}

void Mile::Cirno::PushLinkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::LinkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.DirectoryFileId);
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
}

void Mile::Cirno::PushMkDirRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::MkDirRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.DirectoryFileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.Gid);
}

Mile::Cirno::MkDirResponse Mile::Cirno::PopMkDirResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::MkDirResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    return Result;
}

void Mile::Cirno::PushRenameAtRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::RenameAtRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.OldDirectoryFileId);
    Mile::Cirno::PushString(Buffer, Value.OldName);
    Mile::Cirno::PushUInt32(Buffer, Value.NewDirectoryFileId);
    Mile::Cirno::PushString(Buffer, Value.NewName);
}

void Mile::Cirno::PushUnlinkAtRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::UnlinkAtRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.DirectoryFileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Flags);
}

void Mile::Cirno::PushVersionRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::VersionRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.MaximumMessageSize);
    Mile::Cirno::PushString(Buffer, Value.ProtocolVersion);
}

Mile::Cirno::VersionResponse Mile::Cirno::PopVersionResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::VersionResponse Result;
    Result.MaximumMessageSize = Mile::Cirno::PopUInt32(Buffer);
    Result.ProtocolVersion = Mile::Cirno::PopString(Buffer);
    return Result;
}

void Mile::Cirno::PushAuthRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::AuthRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.AuthenticationFileId);
    Mile::Cirno::PushString(Buffer, Value.UserName);
    Mile::Cirno::PushString(Buffer, Value.AccessName);
    // 9P2000.L
    Mile::Cirno::PushUInt32(Buffer, Value.NumericUserName);
}

Mile::Cirno::AuthResponse Mile::Cirno::PopAuthResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::AuthResponse Result;
    Result.AuthenticationUniqueId = Mile::Cirno::PopQid(Buffer);
    return Result;
}

void Mile::Cirno::PushAttachRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::AttachRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.AuthenticationFileId);
    Mile::Cirno::PushString(Buffer, Value.UserName);
    Mile::Cirno::PushString(Buffer, Value.AccessName);
    // 9P2000.L and 9P2000.W
    Mile::Cirno::PushUInt32(Buffer, Value.NumericUserName);
}

Mile::Cirno::AttachResponse Mile::Cirno::PopAttachResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::AttachResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    return Result;
}

Mile::Cirno::ErrorResponse Mile::Cirno::PopErrorResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::ErrorResponse Result;
    Result.Message = Mile::Cirno::PopString(Buffer);
    // 9P2000.u
    if (sizeof(std::uint32_t) == Buffer.size())
    {
        Result.Code = Mile::Cirno::PopUInt32(Buffer);
    }
    else
    {
        // Plan 9 File System Protocol's ERRUNDEF
        Result.Code = static_cast<std::uint32_t>(-1);
    }
    return Result;
}

void Mile::Cirno::PushFlushRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::FlushRequest const& Value)
{
    Mile::Cirno::PushUInt16(Buffer, Value.OldTag);
}

void Mile::Cirno::PushWalkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::WalkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt32(Buffer, Value.NewFileId);
    Mile::Cirno::PushUInt16(
        Buffer,
        static_cast<std::uint16_t>(Value.Names.size()));
    for (auto const& Name : Value.Names)
    {
        Mile::Cirno::PushString(Buffer, Name);
    }
}

Mile::Cirno::WalkResponse Mile::Cirno::PopWalkResponse(
        std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::WalkResponse Result;
    std::uint16_t Length = Mile::Cirno::PopUInt16(Buffer);
    for (std::uint16_t i = 0; i < Length; ++i)
    {
        Result.UniqueIds.emplace_back(Mile::Cirno::PopQid(Buffer));
    }
    return Result;
}

void Mile::Cirno::PushOpenRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::OpenRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt8(Buffer, Value.Mode);
}

Mile::Cirno::OpenResponse Mile::Cirno::PopOpenResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::OpenResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.IoUnit = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushCreateRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::CreateRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushString(Buffer, Value.Name);
    Mile::Cirno::PushUInt32(Buffer, Value.Permission);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    // 9P2000.u
    Mile::Cirno::PushString(Buffer, Value.UnixExtension);
}

Mile::Cirno::CreateResponse Mile::Cirno::PopCreateResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::CreateResponse Result;
    Result.UniqueId = Mile::Cirno::PopQid(Buffer);
    Result.IoUnit = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushReadRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::ReadRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt64(Buffer, Value.Offset);
    Mile::Cirno::PushUInt32(Buffer, Value.Count);
}

Mile::Cirno::ReadResponse Mile::Cirno::PopReadResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::ReadResponse Result;
    std::uint32_t Length = Mile::Cirno::PopUInt32(Buffer);
    auto Swap = Mile::Cirno::PopBytes(Buffer, Length);
    Result.Data.assign(Swap.begin(), Swap.end());
    return Result;
}

void Mile::Cirno::PushWriteRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::WriteRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    Mile::Cirno::PushUInt64(Buffer, Value.Offset);
    Mile::Cirno::PushUInt32(Buffer, static_cast<std::uint32_t>(Value.Data.size()));
    Buffer.insert(Buffer.end(), Value.Data.begin(), Value.Data.end());
}

Mile::Cirno::WriteResponse Mile::Cirno::PopWriteResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::WriteResponse Result;
    Result.Count = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushClunkRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::ClunkRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

void Mile::Cirno::PushRemoveRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::RemoveRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

void Mile::Cirno::PushStatRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::StatRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
}

Mile::Cirno::StatResponse Mile::Cirno::PopStatResponse(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::StatResponse Result;
    std::uint16_t Length = Mile::Cirno::PopUInt16(Buffer);
    for (std::uint16_t i = 0; i < Length; ++i)
    {
        Result.Stat.push_back(Mile::Cirno::PopStat(Buffer));
    }
    return Result;
}

void Mile::Cirno::PushWriteStatRequest(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::WriteStatRequest const& Value)
{
    Mile::Cirno::PushUInt32(Buffer, Value.FileId);
    std::uint16_t Length = static_cast<std::uint16_t>(Value.Stat.size());
    Mile::Cirno::PushUInt16(Buffer, Length);
    for (auto const& Item: Value.Stat)
    {
        Mile::Cirno::PushStat(Buffer, Item);
    }
}

