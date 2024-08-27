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

#include <type_traits>

#include "Mile.Cirno.Core.h"

namespace Mile::Cirno
{
    inline void CopyFromTemp(std::vector<std::uint8_t>& Dest, std::vector<std::uint8_t> const& Temp)
    {
        Dest.insert(Dest.end(), Temp.begin(), Temp.end());
    }

    std::vector<std::uint8_t> FromUInt8(std::uint8_t const& Value)
    {
        return { Value };
    }

    std::vector<std::uint8_t> FromUInt16(std::uint16_t const& Value)
    {
        std::uint8_t u1 = (std::uint8_t)(Value & 0xff);
        std::uint8_t u2 = (std::uint8_t)(Value >> 8);
        return { u1, u2 };
    }

    std::vector<std::uint8_t> FromUInt32(std::uint32_t const& Value)
    {
        std::uint8_t u1 = (std::uint8_t)(Value & 0xff);
        std::uint8_t u2 = (std::uint8_t)((Value & 0xff00) >> 8);
        std::uint8_t u3 = (std::uint8_t)((Value & 0xff0000) >> 16);
        std::uint8_t u4 = (std::uint8_t)((Value & 0xff000000) >> 24);
        return { u1, u2, u3, u4 };
    }

    std::vector<std::uint8_t> FromUInt64(std::uint64_t const& Value)
    {
        std::uint8_t u1 = (std::uint8_t)(Value & 0xff);
        std::uint8_t u2 = (std::uint8_t)((Value & 0xff00) >> 8);
        std::uint8_t u3 = (std::uint8_t)((Value & 0xff0000) >> 16);
        std::uint8_t u4 = (std::uint8_t)((Value & 0xff000000) >> 24);
        std::uint8_t u5 = (std::uint8_t)((Value & 0xff00000000) >> 32);
        std::uint8_t u6 = (std::uint8_t)((Value & 0xff0000000000) >> 40);
        std::uint8_t u7 = (std::uint8_t)((Value & 0xff000000000000) >> 48);
        std::uint8_t u8 = (std::uint8_t)((Value & 0xff00000000000000) >> 56);
        return { u1, u2, u3, u4, u5, u6, u7, u8 };
    }

    std::vector<std::uint8_t> FromHeader(
        Header const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.Size));
        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromUInt16(Value.Tag));

        return Return;
    }

    std::vector<std::uint8_t> FromString(
        std::string const& Value)
    {
        std::vector<std::uint8_t> Return = { Value.begin(), Value.end() };
        return Return;
    }

    std::vector<std::uint8_t> FromQid(
        Qid const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromUInt32(Value.Version));
        CopyFromTemp(Return, FromUInt64(Value.Path));

        return Return;
    }

    std::vector<std::uint8_t> FromDirectoryEntry(
        DirectoryEntry const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt64(Value.Offset));
        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromString(Value.Name));

        return Return;
    }

    std::vector<std::uint8_t> FromStat(
        Stat const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt16(Value.Type));
        CopyFromTemp(Return, FromUInt32(Value.Dev));
        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.LastAccessTime));
        CopyFromTemp(Return, FromUInt32(Value.LastWriteTime));
        CopyFromTemp(Return, FromUInt64(Value.FileSize));
        CopyFromTemp(Return, FromString(Value.FileName));
        CopyFromTemp(Return, FromString(Value.OwnerUserId));
        CopyFromTemp(Return, FromString(Value.GroupId));
        CopyFromTemp(Return, FromString(Value.LastWriteUserId));

        return Return;
    }

    std::vector<std::uint8_t> FromUnixStat(
        UnixStat const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt16(Value.Type));
        CopyFromTemp(Return, FromUInt32(Value.Dev));
        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.LastAccessTime));
        CopyFromTemp(Return, FromUInt32(Value.LastWriteTime));
        CopyFromTemp(Return, FromUInt64(Value.FileSize));
        CopyFromTemp(Return, FromString(Value.FileName));
        CopyFromTemp(Return, FromString(Value.OwnerUserId));
        CopyFromTemp(Return, FromString(Value.GroupId));
        CopyFromTemp(Return, FromString(Value.LastWriteUserId));
        CopyFromTemp(Return, FromString(Value.UnixExtension));
        CopyFromTemp(Return, FromUInt32(Value.NumericOwnerUserId));
        CopyFromTemp(Return, FromUInt32(Value.NumericGroupId));
        CopyFromTemp(Return, FromUInt32(Value.NumericLastWriteUserId));

        return Return;
    }

    std::vector<std::uint8_t> FromWindowsDirectoryEntry(
        WindowsDirectoryEntry const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromDirectoryEntry(Value.Entry));
        CopyFromTemp(Return, FromStat(Value.Stat));

        return Return;
    }

    std::vector<std::uint8_t> FromLinuxErrorResponse(
        LinuxErrorResponse const& Value)
    {
        return FromUInt32(Value.Code);
    }

    std::vector<std::uint8_t> FromStatFsRequest(
        StatFsRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromStatFsResponse(
        StatFsResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileSystemType));
        CopyFromTemp(Return, FromUInt32(Value.BlockSize));
        CopyFromTemp(Return, FromUInt64(Value.TotalBlocks));
        CopyFromTemp(Return, FromUInt64(Value.FreeBlocks));
        CopyFromTemp(Return, FromUInt64(Value.AvailableBlocks));
        CopyFromTemp(Return, FromUInt64(Value.TotalFiles));
        CopyFromTemp(Return, FromUInt64(Value.FreeFileNodes));
        CopyFromTemp(Return, FromUInt64(Value.FileSystemId));
        CopyFromTemp(Return, FromUInt32(Value.MaximumFileNameLength));

        return Return;
    }

    std::vector<std::uint8_t> FromLinuxOpenRequest(
        LinuxOpenRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.Flags));

        return Return;
    }

    std::vector<std::uint8_t> FromLinuxOpenResponse(
        LinuxOpenResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.IoUnit));

        return Return;
    }

    std::vector<std::uint8_t> FromLinuxCreateRequest(
        LinuxCreateRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Flags));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.Gid));

        return Return;
    }

    std::vector<std::uint8_t> FromLinuxCreateResponse(
        LinuxCreateResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.IoUnit));

        return Return;
    }

    std::vector<std::uint8_t> FromSymLinkRequest(
        SymLinkRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromString(Value.Target));
        CopyFromTemp(Return, FromUInt32(Value.Gid));

        return Return;
    }

    std::vector<std::uint8_t> FromSymLinkResponse(
        SymLinkResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromMkNodRequest(
        MkNodRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.Major));
        CopyFromTemp(Return, FromUInt32(Value.Minor));
        CopyFromTemp(Return, FromUInt32(Value.Gid));

        return Return;
    }

    std::vector<std::uint8_t> FromMkNodResponse(
        MkNodResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromRenameRequest(
        RenameRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.DirectoryFid));
        CopyFromTemp(Return, FromString(Value.Name));

        return Return;
    }

    std::vector<std::uint8_t> FromReadLinkRequest(
        ReadLinkRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromReadLinkResponse(
        ReadLinkResponse const& Value)
    {
        return FromString(Value.Target);
    }

    std::vector<std::uint8_t> FromGetAttrRequest(
        GetAttrRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt64(Value.RequestMask));

        return Return;
    }

    std::vector<std::uint8_t> FromGetAttrResponse(
        GetAttrResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt64(Value.Valid));
        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.OwnerUserId));
        CopyFromTemp(Return, FromUInt32(Value.GroupId));
        CopyFromTemp(Return, FromUInt64(Value.NumberOfHardLinks));
        CopyFromTemp(Return, FromUInt64(Value.DeviceId));
        CopyFromTemp(Return, FromUInt64(Value.FileSize));
        CopyFromTemp(Return, FromUInt64(Value.BlockSize));
        CopyFromTemp(Return, FromUInt64(Value.AllocatedBlocks));
        CopyFromTemp(Return, FromUInt64(Value.LastAccessTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.LastAccessTimeNanoseconds));
        CopyFromTemp(Return, FromUInt64(Value.LastWriteTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.LastWriteTimeNanoseconds));
        CopyFromTemp(Return, FromUInt64(Value.ChangeTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.ChangeTimeNanoseconds));
        CopyFromTemp(Return, FromUInt64(Value.BirthTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.BirthTimeNanoseconds));
        CopyFromTemp(Return, FromUInt64(Value.Generation));
        CopyFromTemp(Return, FromUInt64(Value.DataVersion));

        return Return;
    }

    std::vector<std::uint8_t> FromSetAttrRequest(
        SetAttrRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.Valid));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.OwnerUserId));
        CopyFromTemp(Return, FromUInt32(Value.GroupId));
        CopyFromTemp(Return, FromUInt64(Value.FileSize));
        CopyFromTemp(Return, FromUInt64(Value.LastAccessTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.LastAccessTimeNanoseconds));
        CopyFromTemp(Return, FromUInt64(Value.LastWriteTimeSeconds));
        CopyFromTemp(Return, FromUInt64(Value.LastWriteTimeNanoseconds));

        return Return;
    }

    std::vector<std::uint8_t> FromXattrWalkRequest(
        XattrWalkRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.NewFileId));
        CopyFromTemp(Return, FromString(Value.Name));

        return Return;
    }

    std::vector<std::uint8_t> FromXattrWalkResponse(
        XattrWalkResponse const& Value)
    {
        return FromUInt64(Value.Size);
    }

    std::vector<std::uint8_t> FromXattrCreateRequest(
        XattrCreateRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt64(Value.Size));
        CopyFromTemp(Return, FromUInt32(Value.Flags));

        return Return;
    }

    std::vector<std::uint8_t> FromReadDirRequest(
        ReadDirRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt64(Value.Offset));
        CopyFromTemp(Return, FromUInt32(Value.Count));

        return Return;
    }

    std::vector<std::uint8_t> FromReadDirResponse(
        ReadDirResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        for (auto& data : Value.Data)
        {
            CopyFromTemp(Return, FromDirectoryEntry(data));
        }

        return Return;
    }

    std::vector<std::uint8_t> FromFsyncRequest(
        FsyncRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromLockRequest(
        LockRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromUInt32(Value.Flags));
        CopyFromTemp(Return, FromUInt64(Value.Start));
        CopyFromTemp(Return, FromUInt64(Value.Length));
        CopyFromTemp(Return, FromUInt32(Value.ProcessId));
        CopyFromTemp(Return, FromString(Value.ClientId));

        return Return;
    }

    std::vector<std::uint8_t> FromLockResponse(
        LockResponse const& Value)
    {
        return FromUInt8(Value.Status);
    }

    std::vector<std::uint8_t> FromGetLockRequest(
        GetLockRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromUInt64(Value.Start));
        CopyFromTemp(Return, FromUInt64(Value.Length));
        CopyFromTemp(Return, FromUInt32(Value.ProcessId));
        CopyFromTemp(Return, FromString(Value.ClientId));

        return Return;
    }

    std::vector<std::uint8_t> FromGetLockResponse(
        GetLockResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt8(Value.Type));
        CopyFromTemp(Return, FromUInt64(Value.Start));
        CopyFromTemp(Return, FromUInt64(Value.Length));
        CopyFromTemp(Return, FromUInt32(Value.ProcessId));
        CopyFromTemp(Return, FromString(Value.ClientId));

        return Return;
    }

    std::vector<std::uint8_t> FromLinkRequest(
        LinkRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));

        return Return;
    }

    std::vector<std::uint8_t> FromMkDirRequest(
        MkDirRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromUInt32(Value.Gid));

        return Return;
    }

    std::vector<std::uint8_t> FromMkDirResponse(
        MkDirResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromRenameAtRequest(
        RenameAtRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.OldDirectoryFileId));
        CopyFromTemp(Return, FromString(Value.OldName));
        CopyFromTemp(Return, FromUInt32(Value.NewDirectoryFileId));
        CopyFromTemp(Return, FromString(Value.NewName));

        return Return;
    }

    std::vector<std::uint8_t> FromUnlinkAtRequest(
        UnlinkAtRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Flags));

        return Return;
    }

    std::vector<std::uint8_t> FromVersionRequest(
        VersionRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.MaximumMessageSize));
        CopyFromTemp(Return, FromString(Value.ProtocolVersion));

        return Return;
    }

    std::vector<std::uint8_t> FromVersionResponse(
        VersionResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.MaximumMessageSize));
        CopyFromTemp(Return, FromString(Value.ProtocolVersion));

        return Return;
    }

    std::vector<std::uint8_t> FromAuthRequest(
        AuthRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(Return, FromString(Value.UserName));
        CopyFromTemp(Return, FromString(Value.AccessName));

        return Return;
    }

    std::vector<std::uint8_t> FromUnixAuthRequest(
        UnixAuthRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(Return, FromString(Value.UserName));
        CopyFromTemp(Return, FromString(Value.AccessName));
        CopyFromTemp(Return, FromUInt32(Value.NumericUserName));

        return Return;
    }

    std::vector<std::uint8_t> FromAuthResponse(
        AuthResponse const& Value)
    {
        return FromQid(Value.AuthenticationUniqueId);
    }

    std::vector<std::uint8_t> FromAttachRequest(
        AttachRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(Return, FromString(Value.UserName));
        CopyFromTemp(Return, FromString(Value.AccessName));

        return Return;
    }

    std::vector<std::uint8_t> FromUnixAttachRequest(
        UnixAttachRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(Return, FromString(Value.UserName));
        CopyFromTemp(Return, FromString(Value.AccessName));
        CopyFromTemp(Return, FromUInt32(Value.NumericUserName));

        return Return;
    }

    std::vector<std::uint8_t> FromAttachResponse(
        AttachResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromErrorResponse(
        ErrorResponse const& Value)
    {
        return FromString(Value.Message);
    }

    std::vector<std::uint8_t> FromUnixErrorResponse(
        UnixErrorResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromString(Value.Message));
        CopyFromTemp(Return, FromUInt32(Value.Code));

        return Return;
    }

    std::vector<std::uint8_t> FromFlushRequest(
        FlushRequest const& Value)
    {
        return FromUInt16(Value.OldTag);
    }

    std::vector<std::uint8_t> FromWalkRequest(
        WalkRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt32(Value.NewFileId));
        for (auto& s : Value.Names)
        {
            CopyFromTemp(Return, FromString(s));
        }

        return Return;
    }

    std::vector<std::uint8_t> FromWalkResponse(
        WalkResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        for (auto& qid : Value.UniqueIds) {
            CopyFromTemp(Return, FromQid(qid));
        }

        return Return;
    }

    std::vector<std::uint8_t> FromOpenRequest(
        OpenRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt8(Value.Mode));

        return Return;
    }

    std::vector<std::uint8_t> FromOpenResponse(
        OpenResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromQid(Value.UniqueId));
        CopyFromTemp(Return, FromUInt32(Value.IoUnit));

        return Return;
    }

    std::vector<std::uint8_t> FromCreateRequest(
        CreateRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Permission));
        CopyFromTemp(Return, FromUInt32(Value.Mode));

        return Return;
    }

    std::vector<std::uint8_t> FromUnixCreateRequest(
        UnixCreateRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromString(Value.Name));
        CopyFromTemp(Return, FromUInt32(Value.Permission));
        CopyFromTemp(Return, FromUInt32(Value.Mode));
        CopyFromTemp(Return, FromString(Value.UnixExtension));

        return Return;
    }

    std::vector<std::uint8_t> FromCreateResponse(
        CreateResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromReadRequest(
        ReadRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt64(Value.Offset));
        CopyFromTemp(Return, FromUInt32(Value.Count));

        return Return;
    }

    std::vector<std::uint8_t> FromReadResponse(
        ReadResponse const& Value)
    {
        std::vector<std::uint8_t> Return = {Value.Data};

        return Return;
    }

    std::vector<std::uint8_t> FromWriteRequest(
        WriteRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        CopyFromTemp(Return, FromUInt64(Value.Offset));
        CopyFromTemp(Return, Value.Data);

        return Return;
    }

    std::vector<std::uint8_t> FromWriteResponse(
        WriteResponse const& Value)
    {
        return FromUInt32(Value.Count);
    }

    std::vector<std::uint8_t> FromClunkRequest(
        ClunkRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromRemoveRequest(
        RemoveRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromStatRequest(
        StatRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromStatResponse(
        StatResponse const& Value)
    {
        std::vector<std::uint8_t> Return;

        for (auto& stat : Value.Stat)
        {
            CopyFromTemp(Return, FromStat(stat));
        }

        return Return;
    }

    std::vector<std::uint8_t> FromWriteStatRequest(
        WriteStatRequest const& Value)
    {
        std::vector<std::uint8_t> Return;

        CopyFromTemp(Return, FromUInt32(Value.FileId));
        for (auto& stat : Value.Stat)
        {
            CopyFromTemp(Return, FromStat(stat));
        }

        return Return;
    }

}
