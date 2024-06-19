/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Protocol.h
 * PURPOSE:   Definition for Plan 9 File System Protocol
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_CIRNO_PROTOCOL
#define MILE_CIRNO_PROTOCOL

#include <stdint.h>

/**
 * @brief The definition of Plan 9 File System Protocol Message Type.
 */
typedef enum _MILE_CIRNO_PROTOCOL_MESSAGE_TYPE
{
    // 9P2000.L

    MileCirnoStatFsRequestMessage = 8,
    MileCirnoStatFsResponseMessage,
    MileCirnoLinuxOpenRequestMessage = 12,
    MileCirnoLinuxOpenResponseMessage,
    MileCirnoLinuxCreateRequestMessage = 14,
    MileCirnoLinuxCreateResponseMessage,
    MileCirnoSymLinkRequestMessage = 16,
    MileCirnoSymLinkResponseMessage,
    MileCirnoMkNodRequestMessage = 18,
    MileCirnoMkNodResponseMessage,
    MileCirnoRenameRequestMessage = 20,
    MileCirnoRenameResponseMessage,
    MileCirnoReadLinkRequestMessage = 22,
    MileCirnoReadLinkResponseMessage,
    MileCirnoGetAttrRequestMessage = 24,
    MileCirnoGetAttrResponseMessage,
    MileCirnoSetAttrRequestMessage = 26,
    MileCirnoSetAttrResponseMessage,
    MileCirnoXattrWalkRequestMessage = 30,
    MileCirnoXattrWalkResponseMessage,
    MileCirnoXattrCreateRequestMessage = 32,
    MileCirnoXattrCreateResponseMessage,
    MileCirnoReadDirRequestMessage = 40,
    MileCirnoReadDirResponseMessage,
    MileCirnoFsyncRequestMessage = 50,
    MileCirnoFsyncResponseMessage,
    MileCirnoLockRequestMessage = 52,
    MileCirnoLockResponseMessage,
    MileCirnoGetLockRequestMessage = 54,
    MileCirnoGetLockResponseMessage,
    MileCirnoLinkRequestMessage = 70,
    MileCirnoLinkResponseMessage,
    MileCirnoMkDirRequestMessage = 72,
    MileCirnoMkDirResponseMessage,
    MileCirnoRenameAtRequestMessage = 74,
    MileCirnoRenameAtResponseMessage,
    MileCirnoUnlinkAtRequestMessage = 76,
    MileCirnoUnlinkAtResponseMessage,

    // 9P2000

    MileCirnoVersionRequestMessage = 100,
    MileCirnoVersionResponseMessage,

    MileCirnoAttachRequestMessage = 104,
    MileCirnoAttachResponseMessage,

    MileCirnoFlushRequestMessage = 108,
    MileCirnoFlushResponseMessage,
    MileCirnoWalkRequestMessage = 110,
    MileCirnoWalkResponseMessage,

    MileCirnoReadRequestMessage = 116,
    MileCirnoReadResponseMessage,
    MileCirnoWriteRequestMessage = 118,
    MileCirnoWriteResponseMessage,
    MileCirnoClunkRequestMessage = 120,
    MileCirnoClunkResponseMessage,
    MileCirnoRemoveRequestMessage = 122,
    MileCirnoRemoveResponseMessage,

    // 9P2000.W

    MileCirnoAccessRequestMessage = 128,
    MileCirnoAccessResponseMessage,
    MileCirnoWindowsReadDirRequestMessage = 130,
    MileCirnoWindowsReadDirResponseMessage,
    MileCirnoWindowsOpenRequestMessage = 132,
    MileCirnoWindowsOpenResponseMessage,

} MILE_CIRNO_PROTOCOL_MESSAGE_TYPE, *PMILE_CIRNO_PROTOCOL_MESSAGE_TYPE;

#endif // !MILE_CIRNO_PROTOCOL
