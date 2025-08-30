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

std::uint16_t Mile::Cirno::Client::AllocateTag()
{
    std::lock_guard<std::mutex> Guard(this->m_TagAllocationMutex);

    if (this->m_ReusableTags.empty())
    {
        if (MILE_CIRNO_NOTAG == this->m_TagUnallocatedStart)
        {
            return MILE_CIRNO_NOTAG;
        }
        else
        {
            return this->m_TagUnallocatedStart++;
        }
    }

    std::uint16_t Result = *this->m_ReusableTags.begin();
    this->m_ReusableTags.erase(Result);
    return Result;
}

void Mile::Cirno::Client::FreeTag(
    std::uint16_t const& Tag)
{
    if (MILE_CIRNO_NOTAG == Tag)
    {
        return;
    }

    std::lock_guard<std::mutex> Guard(this->m_TagAllocationMutex);

    this->m_ReusableTags.insert(Tag);

    while (!this->m_ReusableTags.empty() &&
        *this->m_ReusableTags.rbegin() == this->m_TagUnallocatedStart - 1)
    {
        this->m_ReusableTags.erase(--this->m_TagUnallocatedStart);
    }
}

void Mile::Cirno::Client::ReceiveWorkerEntryPoint()
{
    this->m_ReceiveWorkerStarted = true;

    while (this->m_ReceiveWorkerStarted)
    {
        std::vector<std::uint8_t> Content;

        {
            Content.resize(Mile::Cirno::HeaderSize);
            DWORD NumberOfBytesRecvd = 0;
            DWORD Flags = MSG_WAITALL;
            if (!::MileSocketRecv(
                this->m_Socket,
                &Content[0],
                Mile::Cirno::HeaderSize,
                &NumberOfBytesRecvd,
                &Flags))
            {
                Mile::Cirno::ThrowException(
                    "MileSocketRecv",
                    ::WSAGetLastError());
                break;
            }
            if (Mile::Cirno::HeaderSize != NumberOfBytesRecvd)
            {
                Mile::Cirno::ThrowException(
                    "Mile::Cirno::HeaderSize != NumberOfBytesRecvd",
                    ERROR_INVALID_DATA);
                break;
            }
        }

        std::span<std::uint8_t> HeaderSpan = std::span<std::uint8_t>(Content);
        Mile::Cirno::Header Header = Mile::Cirno::PopHeader(HeaderSpan);

        if (Header.Size)
        {
            Content.resize(Mile::Cirno::HeaderSize + Header.Size);
            DWORD NumberOfBytesRecvd = 0;
            DWORD Flags = MSG_WAITALL;
            if (!::MileSocketRecv(
                this->m_Socket,
                &Content[Mile::Cirno::HeaderSize],
                Header.Size,
                &NumberOfBytesRecvd,
                &Flags))
            {
                Mile::Cirno::ThrowException(
                    "MileSocketRecv",
                    ::WSAGetLastError());
                break;
            }
            if (Header.Size != NumberOfBytesRecvd)
            {
                Mile::Cirno::ThrowException(
                    "Header.Size != NumberOfBytesRecvd",
                    ERROR_INVALID_DATA);
                break;
            }
        }

        {
            std::lock_guard<std::mutex> Guard(this->m_ReceiveWorkerMutex);
            this->m_Responses.emplace(Header.Tag, Content);
        }
    }
}

Mile::Cirno::Client::~Client()
{
    if (this->m_ReceiveWorkerStarted)
    {
        this->m_ReceiveWorkerStarted = false;
        if (this->m_ReceiveWorkerThread)
        {
            ::WaitForSingleObject(this->m_ReceiveWorkerThread, INFINITE);
            ::CloseHandle(this->m_ReceiveWorkerThread);
            this->m_ReceiveWorkerThread = nullptr;
        }
        if (INVALID_SOCKET != this->m_Socket)
        {
            ::closesocket(this->m_Socket);
            this->m_Socket = INVALID_SOCKET;
        }
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
    if (!this->m_ReceiveWorkerStarted)
    {
        return APTX_EIO;
    }

    std::uint16_t Tag = MILE_CIRNO_NOTAG;
    if (MileCirnoVersionRequestMessage != RequestType)
    {
        Tag = this->AllocateTag();
        if (MILE_CIRNO_NOTAG == Tag)
        {
            return APTX_EIO;
        }
    }
    auto TagCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (MILE_CIRNO_NOTAG != Tag)
        {
            this->FreeTag(Tag);
        }
    });

    Mile::Cirno::Header RequestHeader;
    RequestHeader.Size = static_cast<std::uint32_t>(RequestContent.size());
    RequestHeader.Type = static_cast<std::uint8_t>(RequestType);
    RequestHeader.Tag = Tag;
    std::vector<std::uint8_t> RequestHeaderBuffer;
    Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);
    {
        std::lock_guard<std::mutex> Guard(this->m_SendOperationMutex);
        DWORD NumberOfBytesSent = 0;
        if (!::MileSocketSend(
            this->m_Socket,
            &RequestHeaderBuffer[0],
            static_cast<DWORD>(RequestHeaderBuffer.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
        if (!::MileSocketSend(
            this->m_Socket,
            &RequestContent[0],
            static_cast<DWORD>(RequestContent.size()),
            &NumberOfBytesSent,
            0))
        {
            return APTX_EIO;
        }
    }

    std::vector<std::uint8_t> Content;

    for (;;)
    {
        {
            std::lock_guard<std::mutex> Guard(this->m_ReceiveWorkerMutex);
            if (this->m_Responses.contains(Tag))
            {
                Content = std::move(this->m_Responses.at(Tag));
                this->m_Responses.erase(Tag);
                break;
            }
        }
        if (Content.empty())
        {
            ::SwitchToThread();
        }
    }

    if (Content.size() < Mile::Cirno::HeaderSize)
    {
        return APTX_EIO;
    }

    std::span<std::uint8_t> ResponseSpan =
        std::span<std::uint8_t>(Content);
    Mile::Cirno::Header ResponseContentHeader =
        Mile::Cirno::PopHeader(ResponseSpan);
    if (ResponseType == ResponseContentHeader.Type)
    {
        ResponseContent.assign(ResponseSpan.begin(), ResponseSpan.end());
    }
    else if (MileCirnoErrorResponseMessage == ResponseContentHeader.Type)
    {
        std::uint32_t ErrorCode =
            Mile::Cirno::PopErrorResponse(ResponseSpan).Code;
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
    else if (MileCirnoLinuxErrorResponseMessage == ResponseContentHeader.Type)
    {
        return Mile::Cirno::PopLinuxErrorResponse(ResponseSpan).Code;
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

void Mile::Cirno::Client::Initialize()
{
    this->m_ReceiveWorkerThread = Mile::CreateThread([this]()
    {
        this->ReceiveWorkerEntryPoint();
    });
    if (this->m_ReceiveWorkerThread)
    {
        ::Sleep(100);
    }
    else
    {
        Mile::Cirno::ThrowException(
            "Mile::CreateThread",
            ::GetLastError());
    }
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
                WSA_FLAG_OVERLAPPED);
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

    Object->Initialize();

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
        WSA_FLAG_OVERLAPPED);
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

    Object->Initialize();

    return Object;
}
