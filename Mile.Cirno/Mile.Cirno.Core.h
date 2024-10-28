/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.h
 * PURPOSE:   Definition for Mile.Cirno Core Infrastructures
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#ifndef MILE_CIRNO_CORE

#include "Mile.Cirno.Protocol.h"

namespace Mile::Cirno
{
    [[noreturn]] void ThrowException(
        std::string_view Checkpoint,
        std::int32_t const& Code);

    class Client
    {
    private:

        SOCKET m_Socket = INVALID_SOCKET;

        Client() = default;

    public:

        ~Client() = default;

        static Client ConnectWithTcpSocket(
            std::string const& Host,
            std::string const& Port);

        static Client ConnectWithHyperVSocket(
            std::uint32_t const& Port);
    };

    std::uint16_t AllocateTag();

    void FreeTag(
        std::uint16_t const& Tag);
}

#endif // !MILE_CIRNO_CORE
