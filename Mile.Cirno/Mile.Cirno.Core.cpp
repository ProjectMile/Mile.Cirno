/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Cirno.Core.cpp
 * PURPOSE:    Implementation for Mile.Cirno Core Infrastructures
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#define _WINSOCKAPI_
#define WIN32_NO_STATUS
#include <Windows.h>
#include <WinSock2.h>
#include <hvsocket.h>
#include <WS2tcpip.h>

#include "Mile.Cirno.Core.h"

#include "Mile.Cirno.Protocol.Parser.h"

#include <Mile.Helpers.CppBase.h>

#include <stdexcept>

#include "Aptx.Posix.Error.h"

[[noreturn]] void Mile::Cirno::ThrowException(
    std::string_view Checkpoint,
    std::int32_t const& Code)
{
    throw std::runtime_error(Mile::FormatString(
        "[Mile.Cirno] %s Failed. (Code = %d)",
        Checkpoint.data(),
        Code));
}

bool Mile::Cirno::Client::SocketRecv(
    _Out_opt_ LPVOID Buffer,
    _In_ DWORD NumberOfBytesToRecv,
    _Out_opt_ LPDWORD NumberOfBytesRecvd,
    _Inout_ LPDWORD Flags)
{
    WSABUF WSABuffer = {};
    WSABuffer.len = static_cast<ULONG>(NumberOfBytesToRecv);
    WSABuffer.buf = const_cast<char*>(reinterpret_cast<const char*>(Buffer));
    return SOCKET_ERROR != ::WSARecv(
        this->m_Socket,
        &WSABuffer,
        1,
        NumberOfBytesRecvd,
        Flags,
        nullptr,
        nullptr);
}

bool Mile::Cirno::Client::SocketSend(
    _In_opt_ LPCVOID Buffer,
    _In_ DWORD NumberOfBytesToSend,
    _Out_opt_ LPDWORD NumberOfBytesSent,
    _In_ DWORD Flags)
{
    WSABUF WSABuffer = {};
    WSABuffer.len = static_cast<ULONG>(NumberOfBytesToSend);
    WSABuffer.buf = const_cast<char*>(reinterpret_cast<const char*>(Buffer));
    return SOCKET_ERROR != ::WSASend(
        this->m_Socket,
        &WSABuffer,
        1,
        NumberOfBytesSent,
        Flags,
        nullptr,
        nullptr);
}

Mile::Cirno::Client::~Client()
{
    if (INVALID_SOCKET != this->m_Socket)
    {
        ::closesocket(this->m_Socket);
        this->m_Socket = INVALID_SOCKET;
    }
}

std::uint32_t Mile::Cirno::Client::AllocateFileId()
{
    std::lock_guard<std::mutex> Guard(this->m_FileIdAllocationMutex);

    if (this->m_ReusableFileIds.empty())
    {
        if (MILE_CIRNO_NOFID == this->m_FileIdUnallocatedStart)
        {
            return MILE_CIRNO_NOFID;
        }
        else
        {
            return this->m_FileIdUnallocatedStart++;
        }
    }

    std::uint32_t Result = *this->m_ReusableFileIds.begin();
    this->m_ReusableFileIds.erase(Result);
    return Result;
}

void Mile::Cirno::Client::FreeFileId(
    std::uint32_t const& FileId)
{
    if (MILE_CIRNO_NOFID == FileId)
    {
        return;
    }

    std::lock_guard<std::mutex> Guard(this->m_FileIdAllocationMutex);

    this->m_ReusableFileIds.insert(FileId);

    while (!this->m_ReusableFileIds.empty() &&
        *this->m_ReusableFileIds.rbegin() == this->m_FileIdUnallocatedStart - 1)
    {
        this->m_ReusableFileIds.erase(--this->m_FileIdUnallocatedStart);
    }
}

std::uint32_t Mile::Cirno::Client::Request(
    MILE_CIRNO_MESSAGE_TYPE const& RequestType,
    std::vector<std::uint8_t> const& RequestContent,
    MILE_CIRNO_MESSAGE_TYPE const& ResponseType,
    std::vector<std::uint8_t>& ResponseContent)
{
    std::lock_guard<std::mutex> Guard(this->m_RequestMutex);

    std::uint16_t Tag = MileCirnoVersionRequestMessage != RequestType
        ? 1
        : MILE_CIRNO_NOTAG;

    Mile::Cirno::Header RequestHeader = {};
    RequestHeader.Size = static_cast<std::uint32_t>(RequestContent.size());
    RequestHeader.Type = static_cast<std::uint8_t>(RequestType);
    RequestHeader.Tag = Tag;
    std::vector<std::uint8_t> RequestHeaderBuffer;
    Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);
    {
        DWORD NumberOfBytesSent = 0;
        if (!this->SocketSend(
            &RequestHeaderBuffer[0],
            static_cast<DWORD>(RequestHeaderBuffer.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
        if (!this->SocketSend(
            &RequestContent[0],
            static_cast<DWORD>(RequestContent.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
    }

    std::vector<std::uint8_t> ResponseBuffer;

    Mile::Cirno::Header ResponseHeader = {};
    {
        ResponseBuffer.resize(Mile::Cirno::HeaderSize);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            Mile::Cirno::HeaderSize,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (Mile::Cirno::HeaderSize != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
        std::span<std::uint8_t> Span = std::span<std::uint8_t>(ResponseBuffer);
        ResponseHeader = Mile::Cirno::PopHeader(Span);
        if (Tag != ResponseHeader.Tag)
        {
            return APTX_EIO;
        }
    }

    if (ResponseHeader.Size)
    {
        ResponseBuffer.resize(ResponseHeader.Size);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            ResponseHeader.Size,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (ResponseHeader.Size != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
    }

    std::span<std::uint8_t> ResponseContentSpan =
        std::span<std::uint8_t>(ResponseBuffer);
    if (ResponseType == ResponseHeader.Type)
    {
        ResponseContent.assign(
            ResponseContentSpan.begin(),
            ResponseContentSpan.end());
    }
    else if (MileCirnoErrorResponseMessage == ResponseHeader.Type)
    {
        std::uint32_t ErrorCode =
            Mile::Cirno::PopErrorResponse(ResponseContentSpan).Code;
        if (ErrorCode > APTX_ERANGE || 11 == ErrorCode)
        {
            // Because there is no convention implementation for non-Linux
            // environment, returns APTX_EIO if the error code is not contained
            // in Version 7 Unix Error Codes which are supported by all major
            // POSIX-compliant environments.
            return APTX_EIO;
        }
        return ErrorCode;
    }
    else if (MileCirnoLinuxErrorResponseMessage == ResponseHeader.Type)
    {
        return Mile::Cirno::PopLinuxErrorResponse(ResponseContentSpan).Code;
    }
    else
    {
        return APTX_EIO;
    }

    return 0;
}

std::uint32_t Mile::Cirno::Client::Version(
    Mile::Cirno::VersionRequest const& Request,
    Mile::Cirno::VersionResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushVersionRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoVersionRequestMessage,
        RequestBuffer,
        MileCirnoVersionResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopVersionResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Attach(
    Mile::Cirno::AttachRequest const& Request,
    Mile::Cirno::AttachResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushAttachRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoAttachRequestMessage,
        RequestBuffer,
        MileCirnoAttachResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopAttachResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Walk(
    Mile::Cirno::WalkRequest const& Request,
    Mile::Cirno::WalkResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushWalkRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoWalkRequestMessage,
        RequestBuffer,
        MileCirnoWalkResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopWalkResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Clunk(
    Mile::Cirno::ClunkRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushClunkRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    return this->Request(
        MileCirnoClunkRequestMessage,
        RequestBuffer,
        MileCirnoClunkResponseMessage,
        ResponseBuffer);
}

std::uint32_t Mile::Cirno::Client::LinuxOpen(
    Mile::Cirno::LinuxOpenRequest const& Request,
    Mile::Cirno::LinuxOpenResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushLinuxOpenRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoLinuxOpenRequestMessage,
        RequestBuffer,
        MileCirnoLinuxOpenResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopLinuxOpenResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::ReadDirectory(
    Mile::Cirno::ReadDirectoryRequest const& Request,
    Mile::Cirno::ReadDirectoryResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushReadDirectoryRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoReadDirectoryRequestMessage,
        RequestBuffer,
        MileCirnoReadDirectoryResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopReadDirectoryResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::GetAttributes(
    Mile::Cirno::GetAttributesRequest const& Request,
    Mile::Cirno::GetAttributesResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushGetAttributesRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoGetAttributesRequestMessage,
        RequestBuffer,
        MileCirnoGetAttributesResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopGetAttributesResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::FileSystemStatus(
    Mile::Cirno::FileSystemStatusRequest const& Request,
    Mile::Cirno::FileSystemStatusResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushFileSystemStatusRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoFileSystemStatusRequestMessage,
        RequestBuffer,
        MileCirnoFileSystemStatusResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopFileSystemStatusResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Read(
    Mile::Cirno::ReadRequest const& Request,
    Mile::Cirno::ReadResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushReadRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoReadRequestMessage,
        RequestBuffer,
        MileCirnoReadResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopReadResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Remove(
    Mile::Cirno::RemoveRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushRemoveRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    return this->Request(
        MileCirnoRemoveRequestMessage,
        RequestBuffer,
        MileCirnoRemoveResponseMessage,
        ResponseBuffer);
}

std::uint32_t Mile::Cirno::Client::SetAttributes(
    Mile::Cirno::SetAttributesRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushSetAttributesRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    return this->Request(
        MileCirnoSetAttributesRequestMessage,
        RequestBuffer,
        MileCirnoSetAttributesResponseMessage,
        ResponseBuffer);
}

std::uint32_t Mile::Cirno::Client::FlushFile(
    Mile::Cirno::FlushFileRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushFlushFileRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    return this->Request(
        MileCirnoFlushFileRequestMessage,
        RequestBuffer,
        MileCirnoFlushFileResponseMessage,
        ResponseBuffer);
}

std::uint32_t Mile::Cirno::Client::RenameAt(
    Mile::Cirno::RenameAtRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushRenameAtRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    return this->Request(
        MileCirnoRenameAtRequestMessage,
        RequestBuffer,
        MileCirnoRenameAtResponseMessage,
        ResponseBuffer);
}

std::uint32_t Mile::Cirno::Client::Write(
    Mile::Cirno::WriteRequest const& Request,
    Mile::Cirno::WriteResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushWriteRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoWriteRequestMessage,
        RequestBuffer,
        MileCirnoWriteResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopWriteResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::MakeDirectory(
    Mile::Cirno::MakeDirectoryRequest const& Request,
    Mile::Cirno::MakeDirectoryResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushMakeDirectoryRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoMakeDirectoryRequestMessage,
        RequestBuffer,
        MileCirnoMakeDirectoryResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopMakeDirectoryResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::LinuxCreate(
    Mile::Cirno::LinuxCreateRequest const& Request,
    Mile::Cirno::LinuxCreateResponse& Response)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushLinuxCreateRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    std::uint32_t ErrorCode = this->Request(
        MileCirnoLinuxCreateRequestMessage,
        RequestBuffer,
        MileCirnoLinuxCreateResponseMessage,
        ResponseBuffer);
    if (0 == ErrorCode)
    {
        std::span<std::uint8_t> ResponseSpan =
            std::span<std::uint8_t>(ResponseBuffer);
        Response = Mile::Cirno::PopLinuxCreateResponse(ResponseSpan);
    }
    return ErrorCode;
}

std::uint32_t Mile::Cirno::Client::Read(
    std::uint32_t const& FileId,
    std::uint64_t const& Offset,
    void* Buffer,
    std::uint32_t const& NumberOfBytesToRead,
    std::uint32_t& NumberOfBytesRead)
{
    std::lock_guard<std::mutex> Guard(this->m_RequestMutex);

    Mile::Cirno::ReadRequest Request = {};
    Request.FileId = FileId;
    Request.Offset = Offset;
    Request.Count = NumberOfBytesToRead;
    std::vector<std::uint8_t> RequestContent;
    Mile::Cirno::PushReadRequest(RequestContent, Request);

    std::uint16_t Tag = 1;

    Mile::Cirno::Header RequestHeader = {};
    RequestHeader.Size = static_cast<std::uint32_t>(RequestContent.size());
    RequestHeader.Type = MileCirnoReadRequestMessage;
    RequestHeader.Tag = Tag;
    std::vector<std::uint8_t> RequestHeaderBuffer;
    Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);
    {
        DWORD NumberOfBytesSent = 0;
        if (!this->SocketSend(
            &RequestHeaderBuffer[0],
            static_cast<DWORD>(RequestHeaderBuffer.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
        if (!this->SocketSend(
            &RequestContent[0],
            static_cast<DWORD>(RequestContent.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
    }

    std::vector<std::uint8_t> ResponseBuffer;

    Mile::Cirno::Header ResponseHeader = {};
    {
        ResponseBuffer.resize(Mile::Cirno::HeaderSize);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            Mile::Cirno::HeaderSize,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (Mile::Cirno::HeaderSize != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
        std::span<std::uint8_t> Span = std::span<std::uint8_t>(ResponseBuffer);
        ResponseHeader = Mile::Cirno::PopHeader(Span);
        if (Tag != ResponseHeader.Tag)
        {
            return APTX_EIO;
        }
    }

    if (MileCirnoReadResponseMessage == ResponseHeader.Type)
    {
        {
            DWORD NumberOfBytesRecvd = 0;
            DWORD Flags = MSG_WAITALL;
            if (!this->SocketRecv(
                &NumberOfBytesRead,
                sizeof(std::uint32_t),
                &NumberOfBytesRecvd,
                &Flags))
            {
                return APTX_EIO;
            }
            if (sizeof(std::uint32_t) != NumberOfBytesRecvd)
            {
                return APTX_EIO;
            }
        }
        if (NumberOfBytesRead)
        {
            DWORD NumberOfBytesRecvd = 0;
            DWORD Flags = MSG_WAITALL;
            if (!this->SocketRecv(
                Buffer,
                NumberOfBytesRead,
                &NumberOfBytesRecvd,
                &Flags))
            {
                return APTX_EIO;
            }
            if (NumberOfBytesRead != NumberOfBytesRecvd)
            {
                return APTX_EIO;
            }
        }
        return 0;
    }

    if (ResponseHeader.Size)
    {
        ResponseBuffer.resize(ResponseHeader.Size);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            ResponseHeader.Size,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (ResponseHeader.Size != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
    }

    std::span<std::uint8_t> ResponseContentSpan =
        std::span<std::uint8_t>(ResponseBuffer);
    if (MileCirnoErrorResponseMessage == ResponseHeader.Type)
    {
        std::uint32_t ErrorCode =
            Mile::Cirno::PopErrorResponse(ResponseContentSpan).Code;
        if (ErrorCode > APTX_ERANGE || 11 == ErrorCode)
        {
            // Because there is no convention implementation for non-Linux
            // environment, returns APTX_EIO if the error code is not contained
            // in Version 7 Unix Error Codes which are supported by all major
            // POSIX-compliant environments.
            return APTX_EIO;
        }
        return ErrorCode;
    }
    else if (MileCirnoLinuxErrorResponseMessage == ResponseHeader.Type)
    {
        return Mile::Cirno::PopLinuxErrorResponse(ResponseContentSpan).Code;
    }

    return APTX_EIO;
}

std::uint32_t Mile::Cirno::Client::Write(
    std::uint32_t const& FileId,
    std::uint64_t const& Offset,
    const void* Buffer,
    std::uint32_t const& NumberOfBytesToWrite,
    std::uint32_t& NumberOfBytesWritten)
{
    std::lock_guard<std::mutex> Guard(this->m_RequestMutex);

    std::uint16_t Tag = 1;

    Mile::Cirno::Header RequestHeader = {};
    RequestHeader.Size = Mile::Cirno::WriteRequestHeaderSize;
    RequestHeader.Size -= Mile::Cirno::HeaderSize;
    RequestHeader.Size += NumberOfBytesToWrite;
    RequestHeader.Type = MileCirnoWriteRequestMessage;
    RequestHeader.Tag = Tag;
    std::vector<std::uint8_t> RequestHeaderBuffer;
    Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);
    Mile::Cirno::PushUInt32(RequestHeaderBuffer, FileId);
    Mile::Cirno::PushUInt64(RequestHeaderBuffer, Offset);
    Mile::Cirno::PushUInt32(RequestHeaderBuffer, NumberOfBytesToWrite);
    {
        DWORD NumberOfBytesSent = 0;
        if (!this->SocketSend(
            &RequestHeaderBuffer[0],
            static_cast<DWORD>(RequestHeaderBuffer.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
        if (!this->SocketSend(
            Buffer,
            NumberOfBytesToWrite,
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
    }

    std::vector<std::uint8_t> ResponseBuffer;

    Mile::Cirno::Header ResponseHeader = {};
    {
        ResponseBuffer.resize(Mile::Cirno::HeaderSize);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            Mile::Cirno::HeaderSize,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (Mile::Cirno::HeaderSize != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
        std::span<std::uint8_t> Span = std::span<std::uint8_t>(ResponseBuffer);
        ResponseHeader = Mile::Cirno::PopHeader(Span);
        if (Tag != ResponseHeader.Tag)
        {
            return APTX_EIO;
        }
    }

    if (ResponseHeader.Size)
    {
        ResponseBuffer.resize(ResponseHeader.Size);
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = MSG_WAITALL;
        if (!this->SocketRecv(
            &ResponseBuffer[0],
            ResponseHeader.Size,
            &NumberOfBytesRecvd,
            &Flags))
        {
            return APTX_EIO;
        }
        if (ResponseHeader.Size != NumberOfBytesRecvd)
        {
            return APTX_EIO;
        }
    }

    std::span<std::uint8_t> ResponseContentSpan =
        std::span<std::uint8_t>(ResponseBuffer);
    if (MileCirnoWriteResponseMessage == ResponseHeader.Type)
    {
        NumberOfBytesWritten = 
            Mile::Cirno::PopWriteResponse(ResponseContentSpan).Count;
    }
    else if (MileCirnoErrorResponseMessage == ResponseHeader.Type)
    {
        std::uint32_t ErrorCode =
            Mile::Cirno::PopErrorResponse(ResponseContentSpan).Code;
        if (ErrorCode > APTX_ERANGE || 11 == ErrorCode)
        {
            // Because there is no convention implementation for non-Linux
            // environment, returns APTX_EIO if the error code is not contained
            // in Version 7 Unix Error Codes which are supported by all major
            // POSIX-compliant environments.
            return APTX_EIO;
        }
        return ErrorCode;
    }
    else if (MileCirnoLinuxErrorResponseMessage == ResponseHeader.Type)
    {
        return Mile::Cirno::PopLinuxErrorResponse(ResponseContentSpan).Code;
    }
    else
    {
        return APTX_EIO;
    }

    return 0;
}

Mile::Cirno::Client* Mile::Cirno::Client::ConnectWithTcpSocket(
    std::string const& Host,
    std::string const& Port)
{
    Mile::Cirno::Client* Object = new Mile::Cirno::Client();
    if (!Object)
    {
        Mile::Cirno::ThrowException(
            "new Mile::Cirno::Client",
            ::GetLastError());
    }

    std::string Checkpoint = "getaddrinfo";
    int Error = 0;

    addrinfo AddressHints = {};
    AddressHints.ai_family = AF_INET;
    AddressHints.ai_socktype = SOCK_STREAM;
    AddressHints.ai_protocol = IPPROTO_TCP;
    addrinfo* AddressInfo = nullptr;
    Error = ::getaddrinfo(
        Host.c_str(),
        Port.c_str(),
        &AddressHints,
        &AddressInfo);
    if (0 == Error)
    {
        for (addrinfo* Current = AddressInfo;
            nullptr != Current;
            Current = Current->ai_next)
        {
            SOCKET Socket = ::WSASocketW(
                Current->ai_family,
                Current->ai_socktype,
                Current->ai_protocol,
                nullptr,
                0,
                0);
            if (INVALID_SOCKET == Socket)
            {
                Checkpoint = "WSASocketW";
                Error = ::WSAGetLastError();
                continue;
            }

            if (SOCKET_ERROR != ::WSAConnect(
                Socket,
                Current->ai_addr,
                static_cast<int>(Current->ai_addrlen),
                nullptr,
                nullptr,
                nullptr,
                nullptr))
            {
                Object->m_Socket = Socket;
                break;
            }

            Checkpoint = "WSAConnect";
            Error = ::WSAGetLastError();
            ::closesocket(Socket);
        }

        ::freeaddrinfo(AddressInfo);
    }

    if (INVALID_SOCKET == Object->m_Socket && 0 != Error)
    {
        Mile::Cirno::ThrowException(
            Checkpoint,
            Error);
    }

    return Object;
}

Mile::Cirno::Client* Mile::Cirno::Client::ConnectWithHyperVSocket(
    std::uint32_t const& Port)
{
    Mile::Cirno::Client* Object = new Mile::Cirno::Client();
    if (!Object)
    {
        Mile::Cirno::ThrowException(
            "new Mile::Cirno::Client",
            ::GetLastError());
    }

    SOCKET Socket = ::WSASocketW(
        AF_HYPERV,
        SOCK_STREAM,
        HV_PROTOCOL_RAW,
        nullptr,
        0,
        0);
    if (INVALID_SOCKET == Socket)
    {
        Mile::Cirno::ThrowException(
            "WSASocketW",
            ::WSAGetLastError());
    }

    SOCKADDR_HV SocketAddress = {};
    SocketAddress.Family = AF_HYPERV;
    std::memcpy(
        &SocketAddress.VmId,
        &HV_GUID_PARENT,
        sizeof(GUID));
    std::memcpy(
        &SocketAddress.ServiceId,
        &HV_GUID_VSOCK_TEMPLATE,
        sizeof(GUID));
    SocketAddress.ServiceId.Data1 = Port;

    if (SOCKET_ERROR == ::WSAConnect(
        Socket,
        reinterpret_cast<sockaddr*>(&SocketAddress),
        sizeof(SocketAddress),
        nullptr,
        nullptr,
        nullptr,
        nullptr))
    {
        Mile::Cirno::ThrowException(
            "WSAConnect",
            ::WSAGetLastError());
    }

    Object->m_Socket = Socket;

    return Object;
}
