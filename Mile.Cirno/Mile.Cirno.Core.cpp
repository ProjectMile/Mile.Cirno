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
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.Size));
        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromUInt16(Value.Tag));

        return vReturn;
    }

    std::vector<std::uint8_t> FromString(
        std::string const& Value)
    {
        std::vector<std::uint8_t> vReturn = { Value.begin(), Value.end() };
        return vReturn;
    }

    std::vector<std::uint8_t> FromQid(
        Qid const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromUInt32(Value.Version));
        CopyFromTemp(vReturn, FromUInt64(Value.Path));

        return vReturn;
    }

    std::vector<std::uint8_t> FromDirectoryEntry(
        DirectoryEntry const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt64(Value.Offset));
        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromString(Value.Name));

        return vReturn;
    }

    std::vector<std::uint8_t> FromStat(
        Stat const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt16(Value.Type));
        CopyFromTemp(vReturn, FromUInt32(Value.Dev));
        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.LastAccessTime));
        CopyFromTemp(vReturn, FromUInt32(Value.LastWriteTime));
        CopyFromTemp(vReturn, FromUInt64(Value.FileSize));
        CopyFromTemp(vReturn, FromString(Value.FileName));
        CopyFromTemp(vReturn, FromString(Value.OwnerUserId));
        CopyFromTemp(vReturn, FromString(Value.GroupId));
        CopyFromTemp(vReturn, FromString(Value.LastWriteUserId));

        return vReturn;
    }

    std::vector<std::uint8_t> FromUnixStat(
        UnixStat const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt16(Value.Type));
        CopyFromTemp(vReturn, FromUInt32(Value.Dev));
        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.LastAccessTime));
        CopyFromTemp(vReturn, FromUInt32(Value.LastWriteTime));
        CopyFromTemp(vReturn, FromUInt64(Value.FileSize));
        CopyFromTemp(vReturn, FromString(Value.FileName));
        CopyFromTemp(vReturn, FromString(Value.OwnerUserId));
        CopyFromTemp(vReturn, FromString(Value.GroupId));
        CopyFromTemp(vReturn, FromString(Value.LastWriteUserId));
        CopyFromTemp(vReturn, FromString(Value.UnixExtension));
        CopyFromTemp(vReturn, FromUInt32(Value.NumericOwnerUserId));
        CopyFromTemp(vReturn, FromUInt32(Value.NumericGroupId));
        CopyFromTemp(vReturn, FromUInt32(Value.NumericLastWriteUserId));

        return vReturn;
    }

    std::vector<std::uint8_t> FromWindowsDirectoryEntry(
        WindowsDirectoryEntry const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromDirectoryEntry(Value.Entry));
        CopyFromTemp(vReturn, FromStat(Value.Stat));

        return vReturn;
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
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileSystemType));
        CopyFromTemp(vReturn, FromUInt32(Value.BlockSize));
        CopyFromTemp(vReturn, FromUInt64(Value.TotalBlocks));
        CopyFromTemp(vReturn, FromUInt64(Value.FreeBlocks));
        CopyFromTemp(vReturn, FromUInt64(Value.AvailableBlocks));
        CopyFromTemp(vReturn, FromUInt64(Value.TotalFiles));
        CopyFromTemp(vReturn, FromUInt64(Value.FreeFileNodes));
        CopyFromTemp(vReturn, FromUInt64(Value.FileSystemId));
        CopyFromTemp(vReturn, FromUInt32(Value.MaximumFileNameLength));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLinuxOpenRequest(
        LinuxOpenRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.Flags));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLinuxOpenResponse(
        LinuxOpenResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.IoUnit));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLinuxCreateRequest(
        LinuxCreateRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Flags));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.Gid));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLinuxCreateResponse(
        LinuxCreateResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.IoUnit));

        return vReturn;
    }

    std::vector<std::uint8_t> FromSymLinkRequest(
        SymLinkRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromString(Value.Target));
        CopyFromTemp(vReturn, FromUInt32(Value.Gid));

        return vReturn;
    }

    std::vector<std::uint8_t> FromSymLinkResponse(
        SymLinkResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromMkNodRequest(
        MkNodRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.Major));
        CopyFromTemp(vReturn, FromUInt32(Value.Minor));
        CopyFromTemp(vReturn, FromUInt32(Value.Gid));

        return vReturn;
    }

    std::vector<std::uint8_t> FromMkNodResponse(
        MkNodResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromRenameRequest(
        RenameRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.DirectoryFid));
        CopyFromTemp(vReturn, FromString(Value.Name));

        return vReturn;
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
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt64(Value.RequestMask));

        return vReturn;
    }

    std::vector<std::uint8_t> FromGetAttrResponse(
        GetAttrResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt64(Value.Valid));
        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.OwnerUserId));
        CopyFromTemp(vReturn, FromUInt32(Value.GroupId));
        CopyFromTemp(vReturn, FromUInt64(Value.NumberOfHardLinks));
        CopyFromTemp(vReturn, FromUInt64(Value.DeviceId));
        CopyFromTemp(vReturn, FromUInt64(Value.FileSize));
        CopyFromTemp(vReturn, FromUInt64(Value.BlockSize));
        CopyFromTemp(vReturn, FromUInt64(Value.AllocatedBlocks));
        CopyFromTemp(vReturn, FromUInt64(Value.LastAccessTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastAccessTimeNanoseconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastWriteTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastWriteTimeNanoseconds));
        CopyFromTemp(vReturn, FromUInt64(Value.ChangeTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.ChangeTimeNanoseconds));
        CopyFromTemp(vReturn, FromUInt64(Value.BirthTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.BirthTimeNanoseconds));
        CopyFromTemp(vReturn, FromUInt64(Value.Generation));
        CopyFromTemp(vReturn, FromUInt64(Value.DataVersion));

        return vReturn;
    }

    std::vector<std::uint8_t> FromSetAttrRequest(
        SetAttrRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.Valid));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.OwnerUserId));
        CopyFromTemp(vReturn, FromUInt32(Value.GroupId));
        CopyFromTemp(vReturn, FromUInt64(Value.FileSize));
        CopyFromTemp(vReturn, FromUInt64(Value.LastAccessTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastAccessTimeNanoseconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastWriteTimeSeconds));
        CopyFromTemp(vReturn, FromUInt64(Value.LastWriteTimeNanoseconds));

        return vReturn;
    }

    std::vector<std::uint8_t> FromXattrWalkRequest(
        XattrWalkRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.NewFileId));
        CopyFromTemp(vReturn, FromString(Value.Name));

        return vReturn;
    }

    std::vector<std::uint8_t> FromXattrWalkResponse(
        XattrWalkResponse const& Value)
    {
        return FromUInt64(Value.Size);
    }

    std::vector<std::uint8_t> FromXattrCreateRequest(
        XattrCreateRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt64(Value.Size));
        CopyFromTemp(vReturn, FromUInt32(Value.Flags));

        return vReturn;
    }

    std::vector<std::uint8_t> FromReadDirRequest(
        ReadDirRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt64(Value.Offset));
        CopyFromTemp(vReturn, FromUInt32(Value.Count));

        return vReturn;
    }

    std::vector<std::uint8_t> FromReadDirResponse(
        ReadDirResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        for (auto& data : Value.Data)
        {
            CopyFromTemp(vReturn, FromDirectoryEntry(data));
        }

        return vReturn;
    }

    std::vector<std::uint8_t> FromFsyncRequest(
        FsyncRequest const& Value)
    {
        return FromUInt32(Value.FileId);
    }

    std::vector<std::uint8_t> FromLockRequest(
        LockRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromUInt32(Value.Flags));
        CopyFromTemp(vReturn, FromUInt64(Value.Start));
        CopyFromTemp(vReturn, FromUInt64(Value.Length));
        CopyFromTemp(vReturn, FromUInt32(Value.ProcessId));
        CopyFromTemp(vReturn, FromString(Value.ClientId));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLockResponse(
        LockResponse const& Value)
    {
        return FromUInt8(Value.Status);
    }

    std::vector<std::uint8_t> FromGetLockRequest(
        GetLockRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromUInt64(Value.Start));
        CopyFromTemp(vReturn, FromUInt64(Value.Length));
        CopyFromTemp(vReturn, FromUInt32(Value.ProcessId));
        CopyFromTemp(vReturn, FromString(Value.ClientId));

        return vReturn;
    }

    std::vector<std::uint8_t> FromGetLockResponse(
        GetLockResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt8(Value.Type));
        CopyFromTemp(vReturn, FromUInt64(Value.Start));
        CopyFromTemp(vReturn, FromUInt64(Value.Length));
        CopyFromTemp(vReturn, FromUInt32(Value.ProcessId));
        CopyFromTemp(vReturn, FromString(Value.ClientId));

        return vReturn;
    }

    std::vector<std::uint8_t> FromLinkRequest(
        LinkRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));

        return vReturn;
    }

    std::vector<std::uint8_t> FromMkDirRequest(
        MkDirRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromUInt32(Value.Gid));

        return vReturn;
    }

    std::vector<std::uint8_t> FromMkDirResponse(
        MkDirResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromRenameAtRequest(
        RenameAtRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.OldDirectoryFileId));
        CopyFromTemp(vReturn, FromString(Value.OldName));
        CopyFromTemp(vReturn, FromUInt32(Value.NewDirectoryFileId));
        CopyFromTemp(vReturn, FromString(Value.NewName));

        return vReturn;
    }

    std::vector<std::uint8_t> FromUnlinkAtRequest(
        UnlinkAtRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.DirectoryFileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Flags));

        return vReturn;
    }

    std::vector<std::uint8_t> FromVersionRequest(
        VersionRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.MaximumMessageSize));
        CopyFromTemp(vReturn, FromString(Value.ProtocolVersion));

        return vReturn;
    }

    std::vector<std::uint8_t> FromVersionResponse(
        VersionResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.MaximumMessageSize));
        CopyFromTemp(vReturn, FromString(Value.ProtocolVersion));

        return vReturn;
    }

    std::vector<std::uint8_t> FromAuthRequest(
        AuthRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(vReturn, FromString(Value.UserName));
        CopyFromTemp(vReturn, FromString(Value.AccessName));

        return vReturn;
    }

    std::vector<std::uint8_t> FromUnixAuthRequest(
        UnixAuthRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(vReturn, FromString(Value.UserName));
        CopyFromTemp(vReturn, FromString(Value.AccessName));
        CopyFromTemp(vReturn, FromUInt32(Value.NumericUserName));

        return vReturn;
    }

    std::vector<std::uint8_t> FromAuthResponse(
        AuthResponse const& Value)
    {
        return FromQid(Value.AuthenticationUniqueId);
    }

    std::vector<std::uint8_t> FromAttachRequest(
        AttachRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(vReturn, FromString(Value.UserName));
        CopyFromTemp(vReturn, FromString(Value.AccessName));

        return vReturn;
    }

    std::vector<std::uint8_t> FromUnixAttachRequest(
        UnixAttachRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.AuthenticationFileId));
        CopyFromTemp(vReturn, FromString(Value.UserName));
        CopyFromTemp(vReturn, FromString(Value.AccessName));
        CopyFromTemp(vReturn, FromUInt32(Value.NumericUserName));

        return vReturn;
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
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromString(Value.Message));
        CopyFromTemp(vReturn, FromUInt32(Value.Code));

        return vReturn;
    }

    std::vector<std::uint8_t> FromFlushRequest(
        FlushRequest const& Value)
    {
        return FromUInt16(Value.OldTag);
    }

    std::vector<std::uint8_t> FromWalkRequest(
        WalkRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt32(Value.NewFileId));
        for (auto& s : Value.Names)
        {
            CopyFromTemp(vReturn, FromString(s));
        }

        return vReturn;
    }

    std::vector<std::uint8_t> FromWalkResponse(
        WalkResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        for (auto& qid : Value.UniqueIds) {
            CopyFromTemp(vReturn, FromQid(qid));
        }

        return vReturn;
    }

    std::vector<std::uint8_t> FromOpenRequest(
        OpenRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt8(Value.Mode));

        return vReturn;
    }

    std::vector<std::uint8_t> FromOpenResponse(
        OpenResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromQid(Value.UniqueId));
        CopyFromTemp(vReturn, FromUInt32(Value.IoUnit));

        return vReturn;
    }

    std::vector<std::uint8_t> FromCreateRequest(
        CreateRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Permission));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));

        return vReturn;
    }

    std::vector<std::uint8_t> FromUnixCreateRequest(
        UnixCreateRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromString(Value.Name));
        CopyFromTemp(vReturn, FromUInt32(Value.Permission));
        CopyFromTemp(vReturn, FromUInt32(Value.Mode));
        CopyFromTemp(vReturn, FromString(Value.UnixExtension));

        return vReturn;
    }

    std::vector<std::uint8_t> FromCreateResponse(
        CreateResponse const& Value)
    {
        return FromQid(Value.UniqueId);
    }

    std::vector<std::uint8_t> FromReadRequest(
        ReadRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt64(Value.Offset));
        CopyFromTemp(vReturn, FromUInt32(Value.Count));

        return vReturn;
    }

    std::vector<std::uint8_t> FromReadResponse(
        ReadResponse const& Value)
    {
        std::vector<std::uint8_t> vReturn = {Value.Data};

        return vReturn;
    }

    std::vector<std::uint8_t> FromWriteRequest(
        WriteRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        CopyFromTemp(vReturn, FromUInt64(Value.Offset));
        CopyFromTemp(vReturn, Value.Data);

        return vReturn;
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
        std::vector<std::uint8_t> vReturn;

        for (auto& stat : Value.Stat)
        {
            CopyFromTemp(vReturn, FromStat(stat));
        }

        return vReturn;
    }

    std::vector<std::uint8_t> FromWriteStatRequest(
        WriteStatRequest const& Value)
    {
        std::vector<std::uint8_t> vReturn;

        CopyFromTemp(vReturn, FromUInt32(Value.FileId));
        for (auto& stat : Value.Stat)
        {
            CopyFromTemp(vReturn, FromStat(stat));
        }

        return vReturn;
    }

}
