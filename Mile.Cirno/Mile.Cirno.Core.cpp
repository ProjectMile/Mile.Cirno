/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.cpp
 * PURPOSE:   Implementation for Mile.Cirno Core Infrastructures
 *
 * LICENSE:   The MIT License
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

[[noreturn]] void Mile::Cirno::ThrowException(
    std::string_view Checkpoint,
    std::int32_t const& Code)
{
    throw std::runtime_error(Mile::FormatString(
        "[Mile.Cirno] %s Failed. (Code = %d)",
        Checkpoint.data(),
        Code));
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
            DWORD Flags = 0;
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

        {
            Content.resize(Mile::Cirno::HeaderSize + Header.Size);
            DWORD NumberOfBytesRecvd = 0;
            DWORD Flags = 0;
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
    std::lock_guard<std::mutex> Guard(this->m_TagAllocationMutex);

    this->m_ReusableTags.insert(Tag);

    while (!this->m_ReusableTags.empty()
        && *this->m_ReusableTags.rbegin() == this->m_TagUnallocatedStart - 1)
    {
        this->m_ReusableTags.erase(--this->m_TagUnallocatedStart);
    }
}

void Mile::Cirno::Client::Request(
    std::uint16_t const& Tag,
    MILE_CIRNO_MESSAGE_TYPE const& RequestType,
    std::vector<std::uint8_t> const& RequestContent,
    MILE_CIRNO_MESSAGE_TYPE const& ResponseType,
    std::vector<std::uint8_t>& ResponseContent)
{
    if (!this->m_ReceiveWorkerStarted)
    {
        return;
    }

    Mile::Cirno::Header RequestHeader;
    RequestHeader.Size = static_cast<std::uint32_t>(RequestContent.size());
    RequestHeader.Type = static_cast<std::uint8_t>(RequestType);
    RequestHeader.Tag = Tag;
    std::vector<std::uint8_t> RequestHeaderBuffer;
    Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);
    {
        DWORD NumberOfBytesSent = 0;
        if (!::MileSocketSend(
            this->m_Socket,
            &RequestHeaderBuffer[0],
            static_cast<DWORD>(RequestHeaderBuffer.size()),
            &NumberOfBytesSent,
            0))
        {
            Mile::Cirno::ThrowException(
                "MileSocketSend(RequestHeaderBuffer)",
                ::WSAGetLastError());
        }
    }
    {
        DWORD NumberOfBytesSent = 0;
        if (!::MileSocketSend(
            this->m_Socket,
            &RequestContent[0],
            static_cast<DWORD>(RequestContent.size()),
            &NumberOfBytesSent,
            0))
        {
            Mile::Cirno::ThrowException(
                "MileSocketSend(RequestContent)",
                ::WSAGetLastError());
        }
    }

    std::vector<std::uint8_t> Content;

    for (;;)
    {
        std::lock_guard<std::mutex> Guard(this->m_ReceiveWorkerMutex);
        auto Iterator = this->m_Responses.find(Tag);
        if (this->m_Responses.end() != Iterator)
        {
            Content = Iterator->second;
            this->m_Responses.erase(Iterator);
            break;
        }
        ::Sleep(100);
    }

    if (Content.size() < Mile::Cirno::HeaderSize)
    {
        Mile::Cirno::ThrowException(
            "Content.size() < Mile::Cirno::HeaderSize",
            ERROR_INVALID_DATA);
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
        Mile::Cirno::ErrorResponse Response =
            Mile::Cirno::PopErrorResponse(ResponseSpan);
        Mile::Cirno::ThrowException(
            Response.Message.c_str(),
            Response.Code);
    }
    else if (MileCirnoLinuxErrorResponseMessage == ResponseContentHeader.Type)
    {
        Mile::Cirno::LinuxErrorResponse Response =
            Mile::Cirno::PopLinuxErrorResponse(ResponseSpan);
        Mile::Cirno::ThrowException(
            "MileCirnoVersionResponseMessage",
            Response.Code);
    }
    else
    {
        Mile::Cirno::ThrowException(
            "ResponseType != ResponseContentHeader.Type",
            ResponseType);
    }
}

Mile::Cirno::VersionResponse Mile::Cirno::Client::Version(
    Mile::Cirno::VersionRequest const& Request)
{
    std::vector<std::uint8_t> RequestBuffer;
    Mile::Cirno::PushVersionRequest(
        RequestBuffer,
        Request);
    std::vector<std::uint8_t> ResponseBuffer;
    this->Request(
        MILE_CIRNO_NOTAG,
        MileCirnoVersionRequestMessage,
        RequestBuffer,
        MileCirnoVersionResponseMessage,
        ResponseBuffer);
    std::span<std::uint8_t> ResponseSpan =
        std::span<std::uint8_t>(ResponseBuffer);
    Mile::Cirno::VersionResponse Response =
        Mile::Cirno::PopVersionResponse(ResponseSpan);
    return Response;
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

    addrinfo AddressHints = { 0 };
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

    Object->m_ReceiveWorkerThread = Mile::CreateThread([Object]()
    {
        Object->ReceiveWorkerEntryPoint();
    });
    if (Object->m_ReceiveWorkerThread)
    {
        ::Sleep(100);
    }
    else
    {
        Mile::Cirno::ThrowException(
            "Mile::CreateThread",
            ::GetLastError());
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
        WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == Socket)
    {
        Mile::Cirno::ThrowException(
            "WSASocketW",
            ::WSAGetLastError());
    }

    SOCKADDR_HV SocketAddress = { 0 };
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

    Object->m_ReceiveWorkerThread = Mile::CreateThread([Object]()
    {
        Object->ReceiveWorkerEntryPoint();
    });
    if (Object->m_ReceiveWorkerThread)
    {
        ::Sleep(100);
    }
    else
    {
        Mile::Cirno::ThrowException(
            "Mile::CreateThread",
            ::GetLastError());
    }

    return Object;
}
