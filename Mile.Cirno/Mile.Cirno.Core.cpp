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

#include <mutex>
#include <set>
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

Mile::Cirno::Client Mile::Cirno::Client::ConnectWithTcpSocket(
    std::string const& Host,
    std::string const& Port)
{
    Mile::Cirno::Client Object = Mile::Cirno::Client();

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
                Object.m_Socket = Socket;
                break;
            }

            Checkpoint = "WSAConnect";
            Error = ::WSAGetLastError();
            ::closesocket(Socket);
        }

        ::freeaddrinfo(AddressInfo);
    }

    if (INVALID_SOCKET == Object.m_Socket && 0 != Error)
    {
        Mile::Cirno::ThrowException(Checkpoint, Error);
    }

    return Object;
}

Mile::Cirno::Client Mile::Cirno::Client::ConnectWithHyperVSocket(
    std::uint32_t const& Port)
{
    Mile::Cirno::Client Object = Mile::Cirno::Client();

    SOCKET Socket = ::WSASocketW(
        AF_HYPERV,
        SOCK_STREAM,
        HV_PROTOCOL_RAW,
        nullptr,
        0,
        WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == Socket)
    {
        Mile::Cirno::ThrowException("WSASocketW", ::WSAGetLastError());
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
        Mile::Cirno::ThrowException("WSAConnect", ::WSAGetLastError());
    }

    Object.m_Socket = Socket;

    return Object;
}

namespace
{
    std::mutex g_TagAllocationMutex;
    std::uint16_t g_UnallocatedTagStart = 0;
    std::set<std::uint16_t> g_ReusableTags;
}

std::uint16_t Mile::Cirno::AllocateTag()
{
    std::lock_guard<std::mutex> Guard(g_TagAllocationMutex);

    if (g_ReusableTags.empty())
    {
        if (MILE_CIRNO_NOTAG == g_UnallocatedTagStart)
        {
            return MILE_CIRNO_NOTAG;
        }
        else
        {
            return g_UnallocatedTagStart++;
        }
    }

    std::uint16_t Result = *g_ReusableTags.begin();
    g_ReusableTags.erase(Result);
    return Result;
}

void Mile::Cirno::FreeTag(
    std::uint16_t const& Tag)
{
    std::lock_guard<std::mutex> Guard(g_TagAllocationMutex);

    g_ReusableTags.insert(Tag);

    while (!g_ReusableTags.empty()
        && *g_ReusableTags.rbegin() == g_UnallocatedTagStart - 1)
    {
        g_ReusableTags.erase(--g_UnallocatedTagStart);
    }
}
