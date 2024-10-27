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

#include <mutex>
#include <set>
#include <span>
#include <vector>
#include <string>

#include "Mile.Cirno.Core.h"

namespace
{
    std::mutex g_TagAllocationMutex;
    std::uint16_t g_UnallocatedTagStart = 0;
    std::set<std::uint16_t> g_ReusableTags;
}

namespace Mile::Cirno
{
    std::uint16_t AllocateTag()
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

    void FreeTag(std::uint16_t const& Tag)
    {
        std::lock_guard<std::mutex> Guard(g_TagAllocationMutex);

        g_ReusableTags.insert(Tag);

        while (!g_ReusableTags.empty()
            && *g_ReusableTags.rbegin() == g_UnallocatedTagStart - 1)
        {
            g_ReusableTags.erase(--g_UnallocatedTagStart);
        }
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

        Mile::Cirno::VersionRequest RequestContent;
        RequestContent.MaximumMessageSize = 1 << 16;
        RequestContent.ProtocolVersion = "9P2000.L";

        std::vector<std::uint8_t> RequestContentBuffer;
        Mile::Cirno::PushVersionRequest(RequestContentBuffer, RequestContent);

        Mile::Cirno::Header RequestHeader;
        RequestHeader.Size = static_cast<std::uint32_t>(
            RequestContentBuffer.size());
        RequestHeader.Type = MileCirnoVersionRequestMessage;
        RequestHeader.Tag = MILE_CIRNO_NOTAG;

        std::vector<std::uint8_t> RequestHeaderBuffer;
        Mile::Cirno::PushHeader(RequestHeaderBuffer, RequestHeader);

        {
            DWORD NumberOfBytesSent = 0;
            if (!::MileSocketSend(
                Socket,
                &RequestHeaderBuffer[0],
                static_cast<DWORD>(RequestHeaderBuffer.size()),
                &NumberOfBytesSent,
                0))
            {
                std::wprintf(
                    L"[ERROR] MileSocketSend failed (%d).\n",
                    ::WSAGetLastError());
                break;
            }
        }
        {
            DWORD NumberOfBytesSent = 0;
            if (!::MileSocketSend(
                Socket,
                &RequestContentBuffer[0],
                static_cast<DWORD>(RequestContentBuffer.size()),
                &NumberOfBytesSent,
                0))
            {
                std::wprintf(
                    L"[ERROR] MileSocketSend failed (%d).\n",
                    ::WSAGetLastError());
                break;
            }
        }

        {
            std::vector<std::uint8_t> ResponseHeaderBuffer;
            ResponseHeaderBuffer.resize(Mile::Cirno::HeaderSize);
            {
                DWORD NumberOfBytesRecvd = 0;
                DWORD Flags = 0;
                if (!::MileSocketRecv(
                    Socket,
                    &ResponseHeaderBuffer[0],
                    Mile::Cirno::HeaderSize,
                    &NumberOfBytesRecvd,
                    &Flags))
                {
                    std::wprintf(
                        L"[ERROR] MileSocketRecv failed (%d).\n",
                        ::WSAGetLastError());
                    break;
                }
                if (Mile::Cirno::HeaderSize != NumberOfBytesRecvd)
                {
                    std::wprintf(
                        L"[ERROR] Invalid response header size.\n");
                    break;
                }
            }

            std::span<std::uint8_t> ResponseHeaderBufferSpan =
                std::span<std::uint8_t>(ResponseHeaderBuffer);
            Mile::Cirno::Header ResponseHeader =
                Mile::Cirno::PopHeader(ResponseHeaderBufferSpan);
            if (RequestHeader.Tag != ResponseHeader.Tag)
            {
                std::wprintf(
                    L"[ERROR] Tag in Header should be equal.\n");
                break;
            }
            if (MileCirnoVersionResponseMessage != ResponseHeader.Type)
            {
                std::wprintf(
                    L"[ERROR] Invalid response type.\n");
                break;
            }

            std::vector<std::uint8_t> ResponseContentBuffer;
            ResponseContentBuffer.resize(ResponseHeader.Size);
            {
                DWORD NumberOfBytesRecvd = 0;
                DWORD Flags = 0;
                if (!::MileSocketRecv(
                    Socket,
                    &ResponseContentBuffer[0],
                    ResponseHeader.Size,
                    &NumberOfBytesRecvd,
                    &Flags))
                {
                    std::wprintf(
                        L"[ERROR] MileSocketRecv failed (%d).\n",
                        ::WSAGetLastError());
                    break;
                }
                if (ResponseHeader.Size != NumberOfBytesRecvd)
                {
                    std::wprintf(
                        L"[ERROR] Invalid response content header size.\n");
                    break;
                }
            }

            std::span<std::uint8_t> ResponseContentBufferSpan =
                std::span<std::uint8_t>(ResponseContentBuffer);
            Mile::Cirno::VersionResponse ResponseContent =
                Mile::Cirno::PopVersionResponse(ResponseContentBufferSpan);

            std::printf(
                "[INFO] Response.ProtocolVersion = %s\n",
                ResponseContent.ProtocolVersion.c_str());
        }

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
