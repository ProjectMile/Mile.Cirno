/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Protocol.Parser.h
 * PURPOSE:   Definition for Plan 9 File System Protocol Parser
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#ifndef MILE_CIRNO_PROTOCOL_PARSER
#define MILE_CIRNO_PROTOCOL_PARSER

#include "Mile.Cirno.Protocol.h"

#include <span>

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

    Qid PopQid(
        std::span<std::uint8_t>& Buffer);

    void PushQid(
        std::vector<std::uint8_t>& Buffer,
        Qid const& Value);

    DirectoryEntry PopDirectoryEntry(
        std::span<std::uint8_t>& Buffer);

    void PushDirectoryEntry(
        std::vector<std::uint8_t>& Buffer,
        DirectoryEntry const& Value);

    Stat PopStat(
        std::span<std::uint8_t>& Buffer);

    void PushStat(
        std::vector<std::uint8_t>& Buffer,
        Stat const& Value);

    UnixStat PopUnixStat(
        std::span<std::uint8_t>& Buffer);

    void PushUnixStat(
        std::vector<std::uint8_t>& Buffer,
        UnixStat const& Value);

    WindowsDirectoryEntry PopWindowsDirectoryEntry(
        std::span<std::uint8_t>& Buffer);

    void PushWindowsDirectoryEntry(
        std::vector<std::uint8_t>& Buffer,
        WindowsDirectoryEntry const& Value);

    LinuxErrorResponse PopLinuxErrorResponse(
        std::span<std::uint8_t>& Buffer);

    void PushStatFsRequest(
        std::vector<std::uint8_t>& Buffer,
        StatFsRequest const& Value);

    StatFsResponse PopStatFsResponse(
        std::span<std::uint8_t>& Buffer);

    void PushLinuxOpenRequest(
        std::vector<std::uint8_t>& Buffer,
        LinuxOpenRequest const& Value);

    LinuxOpenResponse PopLinuxOpenResponse(
        std::span<std::uint8_t>& Buffer);

    void PushLinuxCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        LinuxCreateRequest const& Value);

    LinuxCreateResponse PopLinuxCreateResponse(
        std::span<std::uint8_t>& Buffer);

    void PushSymLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        SymLinkRequest const& Value);

    SymLinkResponse PopSymLinkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushMkNodRequest(
        std::vector<std::uint8_t>& Buffer,
        MkNodRequest const& Value);

    MkNodResponse PopMkNodResponse(
        std::span<std::uint8_t>& Buffer);

    void PushRenameRequest(
        std::vector<std::uint8_t>& Buffer,
        RenameRequest const& Value);

    void PushReadLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadLinkRequest const& Value);

    ReadLinkResponse PopReadLinkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushGetAttrRequest(
        std::vector<std::uint8_t>& Buffer,
        GetAttrRequest const& Value);

    GetAttrResponse PopGetAttrResponse(
        std::span<std::uint8_t>& Buffer);

    void PushSetAttrRequest(
        std::vector<std::uint8_t>& Buffer,
        SetAttrRequest const& Value);

    void PushXattrWalkRequest(
        std::vector<std::uint8_t>& Buffer,
        XattrWalkRequest const& Value);

    XattrWalkResponse PopXattrWalkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushXattrCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        XattrCreateRequest const& Value);

    void PushReadDirRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadDirRequest const& Value);

    ReadDirResponse PopReadDirResponse(
        std::span<std::uint8_t>& Buffer);

    void PushFsyncRequest(
        std::vector<std::uint8_t>& Buffer,
        FsyncRequest const& Value);

    void PushLockRequest(
        std::vector<std::uint8_t>& Buffer,
        LockRequest const& Value);

    LockResponse PopLockResponse(
        std::span<std::uint8_t>& Buffer);

    void PushGetLockRequest(
        std::vector<std::uint8_t>& Buffer,
        GetLockRequest const& Value);

    GetLockResponse PopGetLockResponse(
        std::span<std::uint8_t>& Buffer);

    void PushLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        LinkRequest const& Value);

    void PushMkDirRequest(
        std::vector<std::uint8_t>& Buffer,
        MkDirRequest const& Value);

    MkDirResponse PopMkDirResponse(
        std::span<std::uint8_t>& Buffer);

    void PushRenameAtRequest(
        std::vector<std::uint8_t>& Buffer,
        RenameAtRequest const& Value);
    
    void PushUnlinkAtRequest(
        std::vector<std::uint8_t>& Buffer,
        UnlinkAtRequest const& Value);
    
    void PushVersionRequest(
        std::vector<std::uint8_t>& Buffer,
        VersionRequest const& Value);

    VersionResponse PopVersionResponse(
        std::span<std::uint8_t>& Buffer);

    void PushAuthRequest(
        std::vector<std::uint8_t>& Buffer,
        AuthRequest const& Value);

    AuthResponse PopAuthResponse(
        std::span<std::uint8_t>& Buffer);

    void PushAttachRequest(
        std::vector<std::uint8_t>& Buffer,
        AttachRequest const& Value);

    AttachResponse PopAttachResponse(
        std::span<std::uint8_t>& Buffer); 

    ErrorResponse PopErrorResponse(
        std::span<std::uint8_t>& Buffer);

    void PushFlushRequest(
        std::vector<std::uint8_t>& Buffer,
        FlushRequest const& Value);

    void PushWalkRequest(
        std::vector<std::uint8_t>& Buffer,
        WalkRequest const& Value);

    WalkResponse PopWalkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushOpenRequest(
        std::vector<std::uint8_t>& Buffer,
        OpenRequest const& Value);

    OpenResponse PopOpenResponse(
        std::span<std::uint8_t>& Buffer); 

    void PushCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        CreateRequest const& Value);

    CreateResponse PopCreateResponse(
        std::span<std::uint8_t>& Buffer);

    void PushReadRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadRequest const& Value);

    ReadResponse PopReadResponse(
        std::span<std::uint8_t>& Buffer);

    void PushWriteRequest(
        std::vector<std::uint8_t>& Buffer,
        WriteRequest const& Value);

    WriteResponse PopWriteResponse(
        std::span<std::uint8_t>& Buffer);

    void PushClunkRequest(
        std::vector<std::uint8_t>& Buffer,
        ClunkRequest const& Value);

    void PushRemoveRequest(
        std::vector<std::uint8_t>& Buffer,
        RemoveRequest const& Value);

    void PushStatRequest(
        std::vector<std::uint8_t>& Buffer,
        StatRequest const& Value);

    StatResponse PopStatResponse(
        std::span<std::uint8_t>& Buffer);

    void PushWriteStatRequest(
        std::vector<std::uint8_t>& Buffer,
        WriteStatRequest const& Value);
}

#endif // !MILE_CIRNO_PROTOCOL_PARSER
