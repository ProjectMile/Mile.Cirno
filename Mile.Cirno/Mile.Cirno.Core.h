/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Cirno.Core.h
 * PURPOSE:    Definition for Mile.Cirno Core Infrastructures
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#ifndef MILE_CIRNO_CORE

#include "Mile.Cirno.Protocol.h"

#include <map>
#include <mutex>
#include <set>

namespace Mile::Cirno
{
    [[noreturn]] void ThrowException(
        std::string_view Checkpoint,
        std::int32_t const& Code);

    class Client
    {
    private:

        std::mutex m_FileIdAllocationMutex;
        std::uint32_t m_FileIdUnallocatedStart = 0;
        std::set<std::uint32_t> m_ReusableFileIds;
        SOCKET m_Socket = INVALID_SOCKET;
        std::mutex m_RequestResponseMutex;
        
        Client() = default;

        bool SocketRecv(
            _Out_opt_ LPVOID Buffer,
            _In_ DWORD NumberOfBytesToRecv,
            _Out_opt_ LPDWORD NumberOfBytesRecvd,
            _Inout_ LPDWORD Flags);

        bool SocketSend(
            _In_opt_ LPCVOID Buffer,
            _In_ DWORD NumberOfBytesToSend,
            _Out_opt_ LPDWORD NumberOfBytesSent,
            _In_ DWORD Flags);

    public:

        ~Client();

        std::uint32_t AllocateFileId();

        void FreeFileId(
            std::uint32_t const& FileId);

        std::uint32_t RequestResponse(
            MILE_CIRNO_MESSAGE_TYPE const& RequestType,
            std::vector<std::uint8_t> const& RequestContent,
            MILE_CIRNO_MESSAGE_TYPE const& ResponseType,
            std::vector<std::uint8_t>& ResponseContent);

        std::uint32_t Version(
            VersionRequest const& Request,
            VersionResponse& Response);

        std::uint32_t Attach(
            AttachRequest const& Request,
            AttachResponse& Response);

        std::uint32_t Walk(
            WalkRequest const& Request,
            WalkResponse& Response);

        std::uint32_t Clunk(
            ClunkRequest const& Request);

        std::uint32_t LinuxOpen(
            LinuxOpenRequest const& Request,
            LinuxOpenResponse& Response);

        std::uint32_t ReadDirectory(
            ReadDirectoryRequest const& Request,
            ReadDirectoryResponse& Response);

        std::uint32_t GetAttributes(
            GetAttributesRequest const& Request,
            GetAttributesResponse& Response);

        std::uint32_t FileSystemStatus(
            FileSystemStatusRequest const& Request,
            FileSystemStatusResponse& Response);

        std::uint32_t Read(
            ReadRequest const& Request,
            ReadResponse& Response);

        std::uint32_t Remove(
            RemoveRequest const& Request);

        std::uint32_t SetAttributes(
            SetAttributesRequest const& Request);

        std::uint32_t FlushFile(
            FlushFileRequest const& Request);

        std::uint32_t RenameAt(
            RenameAtRequest const& Request);

        std::uint32_t Write(
            WriteRequest const& Request,
            WriteResponse& Response);

        std::uint32_t MakeDirectory(
            MakeDirectoryRequest const& Request,
            MakeDirectoryResponse& Response);

        std::uint32_t LinuxCreate(
            LinuxCreateRequest const& Request,
            LinuxCreateResponse& Response);

        std::uint32_t Read(
            std::uint32_t const& FileId,
            std::uint64_t const& Offset,
            void* Buffer,
            std::uint32_t const& NumberOfBytesToRead,
            std::uint32_t& NumberOfBytesRead);

        std::uint32_t Write(
            std::uint32_t const& FileId,
            std::uint64_t const& Offset,
            const void* Buffer,
            std::uint32_t const& NumberOfBytesToWrite,
            std::uint32_t& NumberOfBytesWritten);

    public:

        static Client* ConnectWithTcpSocket(
            std::string const& Host,
            std::string const& Port);

        static Client* ConnectWithHyperVSocket(
            std::uint32_t const& Port);
    };
}

#endif // !MILE_CIRNO_CORE
