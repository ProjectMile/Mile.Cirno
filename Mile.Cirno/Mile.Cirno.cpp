/*
 * PROJECT:   Mile.Cirno
 * FILE:      Mile.Cirno.cpp
 * PURPOSE:   Implementation for Mile.Cirno
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#define _WINSOCKAPI_
#define WIN32_NO_STATUS
#include <Windows.h>
#include <WinSock2.h>
#include <hvsocket.h>

#include <Mile.Helpers.Base.h>

#include <dokan/dokan.h>

#include <cstdint>

#include <clocale>
#include <cstdio>
#include <cwchar>

#include <span>
#include <vector>
#include <string>

#include "../Mile.Cirno.Core/Mile.Cirno.Protocol.h"

namespace Mile::Cirno
{
    std::uint8_t* AsUInt8(
        std::uint8_t* Source)
    {
        return reinterpret_cast<std::uint8_t*>(Source);
    }

    std::uint16_t* AsUInt16(
        std::uint8_t* Source)
    {
        return reinterpret_cast<std::uint16_t*>(Source);
    }

    std::uint32_t* AsUInt32(
        std::uint8_t* Source)
    {
        return reinterpret_cast<std::uint32_t*>(Source);
    }

    std::uint64_t* AsUInt64(
        std::uint8_t* Source)
    {
        return reinterpret_cast<std::uint64_t*>(Source);
    }

    bool ReceiveFrame(
        _In_ SOCKET SocketHandle,
        std::vector<std::uint8_t>& Content)
    {
        Content.clear();

        Content.resize(sizeof(MILE_CIRNO_HEADER));
        DWORD NumberOfBytesRecvd = 0;
        DWORD Flags = 0;
        if (::MileSocketRecv(
            SocketHandle,
            &Content[0],
            static_cast<DWORD>(sizeof(MILE_CIRNO_HEADER)),
            &NumberOfBytesRecvd,
            &Flags))
        {
            if (sizeof(MILE_CIRNO_HEADER) == NumberOfBytesRecvd)
            {
                PMILE_CIRNO_HEADER Header =
                    reinterpret_cast<PMILE_CIRNO_HEADER>(&Content[0]);
                DWORD Size = *AsUInt32(Header->Size);
                Content.resize(Size);
                NumberOfBytesRecvd = 0;
                Flags = 0;
                if (::MileSocketRecv(
                    SocketHandle,
                    &Content[sizeof(MILE_CIRNO_HEADER)],
                    Size - sizeof(MILE_CIRNO_HEADER),
                    &NumberOfBytesRecvd,
                    &Flags))
                {
                    if (Size - sizeof(MILE_CIRNO_HEADER) == NumberOfBytesRecvd)
                    {
                        return true;
                    }
                }
            }
        }

        Content.clear();
        return false;
    }
}

void Test()
{
    WSADATA WSAData = { 0 };
    {
        int WSAError = ::WSAStartup(MAKEWORD(2, 2), &WSAData);
        if (NO_ERROR != WSAError)
        {
            std::wprintf(
                L"[ERROR] WSAStartup failed (%d).\n",
                WSAError);
            return;
        }
    }

    SOCKET Socket = INVALID_SOCKET;

    do
    {
        Socket = ::WSASocketW(
            AF_HYPERV,
            SOCK_STREAM,
            HV_PROTOCOL_RAW,
            nullptr,
            0,
            WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET == Socket)
        {
            std::wprintf(
                L"[ERROR] WSASocketW failed (%d).\n",
                ::WSAGetLastError());
            break;
        }

        const std::uint32_t Plan9SharePort = 50001;

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
        SocketAddress.ServiceId.Data1 = Plan9SharePort;

        if (SOCKET_ERROR == ::WSAConnect(
            Socket,
            reinterpret_cast<sockaddr*>(&SocketAddress),
            sizeof(SocketAddress),
            nullptr,
            nullptr,
            nullptr,
            nullptr))
        {
            std::wprintf(
                L"[ERROR] WSAConnect failed (%d).\n",
                ::WSAGetLastError());
            break;
        }

        std::string Version = "9P2000.L";

        std::vector<std::uint8_t> SendBuffer(
            sizeof(MILE_CIRNO_VERSION_REQUEST)
            + Version.size());

        PMILE_CIRNO_VERSION_REQUEST Request =
            reinterpret_cast<PMILE_CIRNO_VERSION_REQUEST>(&SendBuffer[0]);
        *Mile::Cirno::AsUInt32(Request->Header.Size) =
            static_cast<std::uint32_t>(SendBuffer.size());
        *Mile::Cirno::AsUInt8(Request->Header.Type) =
            MileCirnoVersionRequestMessage;
        *Mile::Cirno::AsUInt16(Request->Header.Tag) =
            MILE_CIRNO_NOTAG;
        *Mile::Cirno::AsUInt32(Request->MaximumMessageSize) =
            1 << 16;
        *Mile::Cirno::AsUInt16(Request->ProtocolVersion.Length) =
            static_cast<std::uint16_t>(Version.size());
        std::memcpy(
            Request->ProtocolVersion.String,
            Version.c_str(),
            *Mile::Cirno::AsUInt16(Request->ProtocolVersion.Length));

        DWORD NumberOfBytesSent = 0;
        if (!::MileSocketSend(
            Socket,
            Request,
            *Mile::Cirno::AsUInt32(Request->Header.Size),
            &NumberOfBytesSent,
            0))
        {
            std::wprintf(
                L"[ERROR] MileSocketSend failed (%d).\n",
                ::WSAGetLastError());
            break;
        }

        std::vector<std::uint8_t> ResponseContent;
        if (!Mile::Cirno::ReceiveFrame(Socket, ResponseContent))
        {
            std::wprintf(
                L"[ERROR] Mile::Cirno::ReceiveFrame failed.\n");
            break;
        }

        PMILE_CIRNO_VERSION_RESPONSE Response =
            reinterpret_cast<PMILE_CIRNO_VERSION_RESPONSE>(
                &ResponseContent[0]);

        std::printf(
            "[INFO] Response.ProtocolVersion = %s\n",
            std::string(
                reinterpret_cast<char*>(Response->ProtocolVersion.String),
                *Mile::Cirno::AsUInt16(Response->ProtocolVersion.Length)).c_str());

        break;

    } while (false);

    if (INVALID_SOCKET != Socket)
    {
        ::closesocket(Socket);
    }

    ::WSACleanup();

    std::getchar();
}

NTSTATUS DOKAN_CALLBACK MileCirnoZwCreateFile(
    _In_ LPCWSTR FileName,
    _In_ PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ ULONG FileAttributes,
    _In_ ULONG ShareAccess,
    _In_ ULONG CreateDisposition,
    _In_ ULONG CreateOptions,
    _In_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(SecurityContext);
    UNREFERENCED_PARAMETER(DesiredAccess);
    UNREFERENCED_PARAMETER(FileAttributes);
    UNREFERENCED_PARAMETER(ShareAccess);
    UNREFERENCED_PARAMETER(CreateDisposition);
    UNREFERENCED_PARAMETER(CreateOptions);
    UNREFERENCED_PARAMETER(DokanFileInfo);
    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoFindFiles(
    _In_ LPCWSTR FileName,
    _In_ PFillFindData FillFindData,
    _In_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(FillFindData);
    UNREFERENCED_PARAMETER(DokanFileInfo);

    WIN32_FIND_DATAW FindData = { 0 };
    FindData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    ::wcscpy_s(FindData.cFileName, L"一起摇摆");
    FillFindData(&FindData, DokanFileInfo);

    FindData.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    ::wcscpy_s(FindData.cFileName, L"让我们一起摇摆");
    FillFindData(&FindData, DokanFileInfo);

    return STATUS_SUCCESS;
}

int main()
{
    ::Test();

    DOKAN_OPTIONS Options = { 0 };
    Options.Version = DOKAN_VERSION;
    Options.SingleThread;
    Options.Options =
        DOKAN_OPTION_DEBUG |
        DOKAN_OPTION_STDERR |
        DOKAN_OPTION_ALT_STREAM |
        DOKAN_OPTION_WRITE_PROTECT |
        DOKAN_OPTION_MOUNT_MANAGER |
        DOKAN_OPTION_CASE_SENSITIVE |
        DOKAN_OPTION_DISPATCH_DRIVER_LOGS;
    Options.GlobalContext;
    Options.MountPoint = L"C:\\Windows\\System32\\HostDriverStore";
    Options.UNCName;
    Options.Timeout;
    Options.AllocationUnitSize;
    Options.SectorSize;
    Options.VolumeSecurityDescriptorLength;
    Options.VolumeSecurityDescriptor;

    DOKAN_OPERATIONS Operations = { 0 };
    Operations.ZwCreateFile = ::MileCirnoZwCreateFile;
    Operations.Cleanup;
    Operations.CloseFile;
    Operations.ReadFile;
    Operations.WriteFile;
    Operations.FlushFileBuffers;
    Operations.GetFileInformation;
    Operations.FindFiles = ::MileCirnoFindFiles;
    Operations.FindFilesWithPattern = nullptr;
    Operations.SetFileAttributesW;
    Operations.SetFileTime;
    Operations.DeleteFileW;
    Operations.DeleteDirectory;
    Operations.MoveFileW;
    Operations.SetEndOfFile;
    Operations.SetAllocationSize;
    Operations.LockFile;
    Operations.UnlockFile;
    Operations.GetDiskFreeSpaceW;
    Operations.GetVolumeInformationW;
    Operations.Mounted;
    Operations.Unmounted;
    Operations.GetFileSecurityW;
    Operations.SetFileSecurityW;
    Operations.FindStreams;

    ::DokanInit();

    int Result = ::DokanMain(&Options, &Operations);
    
    ::DokanShutdown();

    return Result;
}
