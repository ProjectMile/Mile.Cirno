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

        std::mutex m_TagAllocationMutex;
        std::uint16_t m_TagUnallocatedStart = 0;
        std::set<std::uint16_t> m_ReusableTags;
        std::mutex m_FileIdAllocationMutex;
        std::uint32_t m_FileIdUnallocatedStart = 0;
        std::set<std::uint32_t> m_ReusableFileIds;
        SOCKET m_Socket = INVALID_SOCKET;
        std::mutex m_SendOperationMutex;
        std::mutex m_ReceiveWorkerMutex;
        HANDLE m_ReceiveWorkerThread = nullptr;
        bool m_ReceiveWorkerStarted = false;
        std::map<std::uint16_t, std::vector<std::uint8_t>> m_Responses;

        Client() = default;

        std::uint16_t AllocateTag();

        void FreeTag(
            std::uint16_t const& Tag);

        void ReceiveWorkerEntryPoint();

    public:

        ~Client();

        std::uint32_t AllocateFileId();

        void FreeFileId(
            std::uint32_t const& FileId);

        void Request(
            MILE_CIRNO_MESSAGE_TYPE const& RequestType,
            std::vector<std::uint8_t> const& RequestContent,
            MILE_CIRNO_MESSAGE_TYPE const& ResponseType,
            std::vector<std::uint8_t>& ResponseContent);

        VersionResponse Version(
            VersionRequest const& Request);

        AttachResponse Attach(
            AttachRequest const& Request);

        WalkResponse Walk(
            WalkRequest const& Request);

        void Clunk(
            ClunkRequest const& Request);

        LinuxOpenResponse LinuxOpen(
            LinuxOpenRequest const& Request);

        ReadDirResponse ReadDir(
            ReadDirRequest const& Request);

        GetAttrResponse GetAttr(
            GetAttrRequest const& Request);

        StatFsResponse StatFs(
            StatFsRequest const& Request);

        ReadResponse Read(
            ReadRequest const& Request);

        void Remove(
            RemoveRequest const& Request);

        void SetAttr(
            SetAttrRequest const& Request);

        void Fsync(
            FsyncRequest const& Request);

        void RenameAt(
            RenameAtRequest const& Request);

        WriteResponse Write(
            WriteRequest const& Request);

        MkDirResponse MkDir(
            MkDirRequest const& Request);

        LinuxCreateResponse LinuxCreate(
            LinuxCreateRequest const& Request);

    private:

        void Initialize();

    public:

        static Client* ConnectWithTcpSocket(
            std::string const& Host,
            std::string const& Port);

        static Client* ConnectWithHyperVSocket(
            std::uint32_t const& Port);
    };
}

#endif // !MILE_CIRNO_CORE
