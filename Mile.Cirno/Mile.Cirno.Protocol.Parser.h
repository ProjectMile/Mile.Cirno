/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Cirno.Protocol.Parser.h
 * PURPOSE:    Definition for Plan 9 File System Protocol Parser
 *
 * LICENSE:    The MIT License
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

    Status PopStatus(
        std::span<std::uint8_t>& Buffer);

    void PushStatus(
        std::vector<std::uint8_t>& Buffer,
        Status const& Value);

    WindowsDirectoryEntry PopWindowsDirectoryEntry(
        std::span<std::uint8_t>& Buffer);

    void PushWindowsDirectoryEntry(
        std::vector<std::uint8_t>& Buffer,
        WindowsDirectoryEntry const& Value);

    LinuxErrorResponse PopLinuxErrorResponse(
        std::span<std::uint8_t>& Buffer);

    void PushFileSystemStatusRequest(
        std::vector<std::uint8_t>& Buffer,
        FileSystemStatusRequest const& Value);

    FileSystemStatusResponse PopFileSystemStatusResponse(
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

    void PushMakeSymbolicLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        MakeSymbolicLinkRequest const& Value);

    MakeSymbolicLinkResponse PopMakeSymbolicLinkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushMakeDeviceNodeRequest(
        std::vector<std::uint8_t>& Buffer,
        MakeDeviceNodeRequest const& Value);

    MakeDeviceNodeResponse PopMakeDeviceNodeResponse(
        std::span<std::uint8_t>& Buffer);

    void PushRenameRequest(
        std::vector<std::uint8_t>& Buffer,
        RenameRequest const& Value);

    void PushReadLinkRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadLinkRequest const& Value);

    ReadLinkResponse PopReadLinkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushGetAttributesRequest(
        std::vector<std::uint8_t>& Buffer,
        GetAttributesRequest const& Value);

    GetAttributesResponse PopGetAttributesResponse(
        std::span<std::uint8_t>& Buffer);

    void PushSetAttributesRequest(
        std::vector<std::uint8_t>& Buffer,
        SetAttributesRequest const& Value);

    void PushExtendedAttributesWalkRequest(
        std::vector<std::uint8_t>& Buffer,
        ExtendedAttributesWalkRequest const& Value);

    ExtendedAttributesWalkResponse PopExtendedAttributesWalkResponse(
        std::span<std::uint8_t>& Buffer);

    void PushExtendedAttributesCreateRequest(
        std::vector<std::uint8_t>& Buffer,
        ExtendedAttributesCreateRequest const& Value);

    void PushReadDirectoryRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadDirectoryRequest const& Value);

    ReadDirectoryResponse PopReadDirectoryResponse(
        std::span<std::uint8_t>& Buffer);

    void PushFlushFileRequest(
        std::vector<std::uint8_t>& Buffer,
        FlushFileRequest const& Value);

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

    void PushMakeDirectoryRequest(
        std::vector<std::uint8_t>& Buffer,
        MakeDirectoryRequest const& Value);

    MakeDirectoryResponse PopMakeDirectoryResponse(
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

    void PushAuthenticationRequest(
        std::vector<std::uint8_t>& Buffer,
        AuthenticationRequest const& Value);

    AuthenticationResponse PopAuthenticationResponse(
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

    void PushReadStatusRequest(
        std::vector<std::uint8_t>& Buffer,
        ReadStatusRequest const& Value);

    ReadStatusResponse PopReadStatusResponse(
        std::span<std::uint8_t>& Buffer);

    void PushWriteStatusRequest(
        std::vector<std::uint8_t>& Buffer,
        WriteStatusRequest const& Value);

    void PushAccessRequest(
        std::vector<std::uint8_t>& Buffer,
        AccessRequest const& Value);

    void PushWindowsReadDirectoryRequest(
        std::vector<std::uint8_t>& Buffer,
        WindowsReadDirectoryRequest const& Value);

    WindowsReadDirectoryResponse PopWindowsReadDirectoryResponse(
        std::span<std::uint8_t>& Buffer);

    void PushWindowsOpenRequest(
        std::vector<std::uint8_t>& Buffer,
        WindowsOpenRequest const& Value);

    WindowsOpenResponse PopWindowsOpenResponse(
        std::span<std::uint8_t>& Buffer);
}

#endif // !MILE_CIRNO_PROTOCOL_PARSER
