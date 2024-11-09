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

#include <Mile.Helpers.CppBase.h>

#include <dokan/dokan.h>

#include <cstdint>

#include <clocale>
#include <cstdio>
#include <cwchar>

#include <span>
#include <vector>
#include <string>

#include "Mile.Cirno.Core.h"
#include "Mile.Cirno.Protocol.Parser.h"

namespace
{
    Mile::Cirno::Client* g_Instance = nullptr;
    std::uint32_t g_RootDirectoryFileId = MILE_CIRNO_NOFID;
}

void Test()
{
    try
    {
        std::uint32_t FileId = g_Instance->AllocateFileId();
        {
            Mile::Cirno::WalkRequest Request;
            Request.FileId = g_RootDirectoryFileId;
            Request.NewFileId = FileId;
            Request.Names.push_back("FileRepository");
            Request.Names.push_back("nvmii.inf_amd64_9af988a7aa90f6d3");
            //Request.Names.push_back("amd_dpfc.sys");
            Mile::Cirno::WalkResponse Response = g_Instance->Walk(Request);
            Response = Response;
        }

        {
            Mile::Cirno::LinuxOpenRequest Request;
            Request.FileId = FileId;
            Request.Flags =
                MileCirnoLinuxOpenCreateFlagNonBlock |
                MileCirnoLinuxOpenCreateFlagLargeFile |
                MileCirnoLinuxOpenCreateFlagDirectory |
                MileCirnoLinuxOpenCreateFlagCloseOnExecute;
            Mile::Cirno::LinuxOpenResponse Response = g_Instance->LinuxOpen(Request);
            Response = Response;
        }

        std::uint64_t LastOffset = 0;
        do
        {
            Mile::Cirno::ReadDirRequest Request;
            Request.FileId = FileId;
            Request.Offset = LastOffset;
            LastOffset = 0;
            Request.Count = (1 << 16) - Mile::Cirno::HeaderSize - sizeof(std::uint32_t);
            Mile::Cirno::ReadDirResponse Response = g_Instance->ReadDir(Request);
            for (Mile::Cirno::DirectoryEntry const& Entry : Response.Data)
            {
                LastOffset = Entry.Offset;
                std::printf(
                    "[INFO] List Directory: %s\n",
                    Entry.Name.c_str());
            }
        } while (LastOffset);

        {
            Mile::Cirno::GetAttrRequest Request;
            Request.FileId = FileId;
            Request.RequestMask = MileCirnoLinuxGetAttrFlagAll;
            Mile::Cirno::GetAttrResponse Response = g_Instance->GetAttr(Request);
            Response = Response;
        }

        {
            Mile::Cirno::ClunkRequest Request;
            Request.FileId = FileId;
            g_Instance->Clunk(Request);
        }
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
    }

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
    ::DokanInit();
    auto DokanCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        ::DokanShutdown();
    });

    WSADATA WSAData = { 0 };
    {
        int WSAError = ::WSAStartup(MAKEWORD(2, 2), &WSAData);
        if (NO_ERROR != WSAError)
        {
            std::wprintf(
                L"[ERROR] WSAStartup failed (%d).\n",
                WSAError);
            return WSAError;
        }
    }
    auto WSACleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        ::WSACleanup();
    });

    try
    {
        const std::uint32_t Plan9SharePort = 50001;
        g_Instance = Mile::Cirno::Client::ConnectWithHyperVSocket(
            Plan9SharePort);
        if (!g_Instance)
        {
            Mile::Cirno::ThrowException(
                "!Instance",
                ERROR_INVALID_DATA);
        }
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
    }
    auto GlobalInstanceCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (g_Instance)
        {
            delete g_Instance;
            g_Instance = nullptr;
        }
    });

    try
    {
        Mile::Cirno::VersionRequest Request;
        Request.MaximumMessageSize = 1 << 16;
        Request.ProtocolVersion = "9P2000.L";
        Mile::Cirno::VersionResponse Response =
            g_Instance->Version(Request);
        std::printf(
            "[INFO] Response.ProtocolVersion = %s\n"
            "[INFO] Response.MaximumMessageSize = %u\n",
            Response.ProtocolVersion.c_str(),
            Response.MaximumMessageSize);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
    }

    try
    {
        Mile::Cirno::AttachRequest Request;
        Request.FileId = g_Instance->AllocateFileId();
        Request.AuthenticationFileId = MILE_CIRNO_NOFID;
        Request.UserName = "";
        Request.AccessName = "HostDriverStore";
        Request.NumericUserName = MILE_CIRNO_NONUNAME;
        Mile::Cirno::AttachResponse Response = g_Instance->Attach(Request);
        g_RootDirectoryFileId = Request.FileId;
        std::printf(
            "[INFO] Response.UniqueId.Path = 0x%016llX\n",
            Response.UniqueId.Path);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
    }

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
    return ::DokanMain(&Options, &Operations);
}
