/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.cpp
 * PURPOSE:   Implementation for Mile.Cirno Core API
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#include "Mile.Cirno.Core.h"

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
    return Result;
}

void Mile::Cirno::PushStat(
    std::vector<std::uint8_t>& Buffer,
    Stat const& Value)
{
    Mile::Cirno::PushUInt16(Buffer, Value.Type);
    Mile::Cirno::PushUInt32(Buffer, Value.Dev);
    Mile::Cirno::PushQid(Buffer, Value.UniqueId);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.LastAccessTime);
    Mile::Cirno::PushUInt32(Buffer, Value.LastWriteTime);
    Mile::Cirno::PushUInt64(Buffer, Value.FileSize);
    Mile::Cirno::PushString(Buffer, Value.FileName);
    Mile::Cirno::PushString(Buffer, Value.OwnerUserId);
    Mile::Cirno::PushString(Buffer, Value.GroupId);
    Mile::Cirno::PushString(Buffer, Value.LastWriteUserId);
}

Mile::Cirno::UnixStat Mile::Cirno::PopUnixStat(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::UnixStat Result;
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
    Result.NumericOwnerUserId = Mile::Cirno::PopUInt32(Buffer);
    Result.NumericGroupId = Mile::Cirno::PopUInt32(Buffer);
    Result.NumericLastWriteUserId = Mile::Cirno::PopUInt32(Buffer);
    return Result;
}

void Mile::Cirno::PushUnixStat(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::UnixStat const& Value)
{
    Mile::Cirno::PushUInt16(Buffer, Value.Type);
    Mile::Cirno::PushUInt32(Buffer, Value.Dev);
    Mile::Cirno::PushQid(Buffer, Value.UniqueId);
    Mile::Cirno::PushUInt32(Buffer, Value.Mode);
    Mile::Cirno::PushUInt32(Buffer, Value.LastAccessTime);
    Mile::Cirno::PushUInt32(Buffer, Value.LastWriteTime);
    Mile::Cirno::PushUInt64(Buffer, Value.FileSize);
    Mile::Cirno::PushString(Buffer, Value.FileName);
    Mile::Cirno::PushString(Buffer, Value.OwnerUserId);
    Mile::Cirno::PushString(Buffer, Value.GroupId);
    Mile::Cirno::PushString(Buffer, Value.LastWriteUserId);
    Mile::Cirno::PushUInt32(Buffer, Value.NumericOwnerUserId);
    Mile::Cirno::PushUInt32(Buffer, Value.NumericGroupId);
    Mile::Cirno::PushUInt32(Buffer, Value.NumericLastWriteUserId);
}

Mile::Cirno::WindowsDirectoryEntry Mile::Cirno::PopWindowsDirectoryEntry(
    std::span<std::uint8_t>& Buffer)
{
    Mile::Cirno::WindowsDirectoryEntry Result;
    Result.Entry = Mile::Cirno::PopDirectoryEntry(Buffer);
    Result.Stat = Mile::Cirno::PopStat(Buffer);
}

void Mile::Cirno::PushWindowsDirectoryEntry(
    std::vector<std::uint8_t>& Buffer,
    Mile::Cirno::WindowsDirectoryEntry const& Value)
{
    Mile::Cirno::PushDirectoryEntry(Buffer, Value.Entry);
    Mile::Cirno::PushStat(Buffer, Value.Stat);
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
    Result.TotalBlocks = Mile::Cirno::PopUInt16(Buffer);
    Result.FreeBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.AvailableBlocks = Mile::Cirno::PopUInt64(Buffer);
    Result.TotalFiles = Mile::Cirno::PopUInt64(Buffer);
    Result.FreeFileNodes = Mile::Cirno::PopUInt64(Buffer);
    Result.FileSystemId = Mile::Cirno::PopUInt64(Buffer);
    Result.MaximumFileNameLength = Mile::Cirno::PopUInt32(Buffer);
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

void PushMkNodRequest(
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
