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

#include <filesystem>
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
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
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

NTSTATUS DOKAN_CALLBACK MileCirnoGetFileInformation(
    _In_ LPCWSTR FileName,
    _Out_ LPBY_HANDLE_FILE_INFORMATION Buffer,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    std::memset(Buffer, 0, sizeof(BY_HANDLE_FILE_INFORMATION));

    try
    {
        Mile::Cirno::WalkRequest WalkRequest;
        WalkRequest.FileId = g_RootDirectoryFileId;
        WalkRequest.NewFileId = g_Instance->AllocateFileId();
        std::filesystem::path RelativePath(&FileName[1]);
        for (std::filesystem::path const& Element : RelativePath)
        {
            WalkRequest.Names.push_back(Element.string());
        }
        g_Instance->Walk(WalkRequest);
        auto CurrentCleanupHandler = Mile::ScopeExitTaskHandler([&]()
        {
            if (MILE_CIRNO_NOFID == WalkRequest.NewFileId)
            {
                try
                {
                    Mile::Cirno::ClunkRequest ClunkRequest;
                    ClunkRequest.FileId = WalkRequest.NewFileId;
                    g_Instance->Clunk(ClunkRequest);
                }
                catch (std::exception const& ex)
                {
                    std::printf("%s\n", ex.what());
                }
                g_Instance->FreeFileId(WalkRequest.NewFileId);
            }
        });

        Mile::Cirno::LinuxOpenRequest OpenRequest;
        OpenRequest.FileId = WalkRequest.NewFileId;
        OpenRequest.Flags = 0;
        g_Instance->LinuxOpen(OpenRequest);

        Mile::Cirno::GetAttrRequest InformationRequest;
        InformationRequest.FileId = WalkRequest.NewFileId;
        InformationRequest.RequestMask = MileCirnoLinuxGetAttrFlagAll;
        Mile::Cirno::GetAttrResponse InformationResponse =
            g_Instance->GetAttr(InformationRequest);

        if (S_IFDIR & InformationResponse.Mode)
        {
            Buffer->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        }

        Buffer->ftCreationTime;
        Buffer->ftLastAccessTime;
        Buffer->ftLastWriteTime;
        Buffer->dwVolumeSerialNumber;

        Buffer->nFileSizeHigh =
            static_cast<DWORD>(InformationResponse.FileSize >> 32);
        Buffer->nFileSizeLow =
            static_cast<DWORD>(InformationResponse.FileSize);

        Buffer->nNumberOfLinks =
            static_cast<DWORD>(InformationResponse.NumberOfHardLinks);

        Buffer->nFileIndexHigh;
        Buffer->nFileIndexLow;
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoFindFiles(
    _In_ LPCWSTR FileName,
    _In_ PFillFindData FillFindData,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(FillFindData);
    UNREFERENCED_PARAMETER(DokanFileInfo);

    //bool IsRootFolder = !std::wcscmp(FileName, L"\\");

    std::uint32_t CurrentDirectoryFileId = MILE_CIRNO_NOFID;
    try
    {
        Mile::Cirno::WalkRequest Request;
        Request.FileId = g_RootDirectoryFileId;
        Request.NewFileId = g_Instance->AllocateFileId();
        std::filesystem::path RelativePath(&FileName[1]);
        for (std::filesystem::path const& Element : RelativePath)
        {
            Request.Names.push_back(Element.string());
        }
        g_Instance->Walk(Request);
        CurrentDirectoryFileId = Request.NewFileId;
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_UNSUCCESSFUL;
    }
    auto CleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (MILE_CIRNO_NOFID == CurrentDirectoryFileId)
        {
            try
            {
                Mile::Cirno::ClunkRequest Request;
                Request.FileId = CurrentDirectoryFileId;
                g_Instance->Clunk(Request);
            }
            catch (std::exception const& ex)
            {
                std::printf("%s\n", ex.what());
            }
            g_Instance->FreeFileId(CurrentDirectoryFileId);
        }
    });

    try
    {
        Mile::Cirno::LinuxOpenRequest Request;
        Request.FileId = CurrentDirectoryFileId;
        Request.Flags =
            MileCirnoLinuxOpenCreateFlagNonBlock |
            MileCirnoLinuxOpenCreateFlagLargeFile |
            MileCirnoLinuxOpenCreateFlagDirectory |
            MileCirnoLinuxOpenCreateFlagCloseOnExecute;
        g_Instance->LinuxOpen(Request);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_UNSUCCESSFUL;
    }

    try
    {
        std::uint64_t LastOffset = 0;
        do
        {
            Mile::Cirno::ReadDirRequest Request;
            Request.FileId = CurrentDirectoryFileId;
            Request.Offset = LastOffset;
            LastOffset = 0;
            Request.Count =
                (1 << 16) - Mile::Cirno::HeaderSize - sizeof(std::uint32_t);
            Mile::Cirno::ReadDirResponse Response =
                g_Instance->ReadDir(Request);
            for (Mile::Cirno::DirectoryEntry const& Entry : Response.Data)
            {
                LastOffset = Entry.Offset;

                //if (IsRootFolder)
                {
                    if ("." == Entry.Name || ".." == Entry.Name)
                    {
                        continue;
                    }
                }

                WIN32_FIND_DATAW FindData = { 0 };
                FindData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                ::wcscpy_s(
                    FindData.cFileName,
                    Mile::ToWideString(CP_UTF8, Entry.Name).c_str());
                std::printf(
                    "[INFO] List Directory: %s\n",
                    Entry.Name.c_str());

                try
                {
                    Mile::Cirno::WalkRequest WalkRequest;
                    WalkRequest.FileId = g_RootDirectoryFileId;
                    WalkRequest.NewFileId = g_Instance->AllocateFileId();
                    std::filesystem::path RelativePath(&FileName[1]);
                    for (std::filesystem::path const& Element : RelativePath)
                    {
                        WalkRequest.Names.push_back(Element.string());
                    }
                    WalkRequest.Names.push_back(Entry.Name);
                    g_Instance->Walk(WalkRequest);
                    auto CurrentCleanupHandler = Mile::ScopeExitTaskHandler([&]()
                    {
                        if (MILE_CIRNO_NOFID == WalkRequest.NewFileId)
                        {
                            try
                            {
                                Mile::Cirno::ClunkRequest ClunkRequest;
                                ClunkRequest.FileId = WalkRequest.NewFileId;
                                g_Instance->Clunk(ClunkRequest);
                            }
                            catch (std::exception const& ex)
                            {
                                std::printf("%s\n", ex.what());
                            }
                            g_Instance->FreeFileId(WalkRequest.NewFileId);
                        }
                    });

                    Mile::Cirno::LinuxOpenRequest OpenRequest;
                    OpenRequest.FileId = WalkRequest.NewFileId;
                    OpenRequest.Flags = 0;
                    g_Instance->LinuxOpen(OpenRequest);

                    Mile::Cirno::GetAttrRequest InformationRequest;
                    InformationRequest.FileId = WalkRequest.NewFileId;
                    InformationRequest.RequestMask = MileCirnoLinuxGetAttrFlagAll;
                    Mile::Cirno::GetAttrResponse InformationResponse =
                        g_Instance->GetAttr(InformationRequest);

                    if (S_IFDIR & InformationResponse.Mode)
                    {
                        FindData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                    }
                    
                    FindData.ftCreationTime;
                    FindData.ftLastAccessTime;
                    FindData.ftLastWriteTime;

                    FindData.nFileSizeHigh =
                        static_cast<DWORD>(InformationResponse.FileSize >> 32);
                    FindData.nFileSizeLow =
                        static_cast<DWORD>(InformationResponse.FileSize);
                }
                catch (std::exception const& ex)
                {
                    std::printf("%s\n", ex.what());
                }

                FillFindData(&FindData, DokanFileInfo);
            }
        } while (LastOffset);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

int main()
{
    auto CleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        if (g_Instance)
        {
            if (MILE_CIRNO_NOFID == g_RootDirectoryFileId)
            {
                g_Instance->FreeFileId(g_RootDirectoryFileId);
            }
            delete g_Instance;
            g_Instance = nullptr;
        }

        ::WSACleanup();

        ::DokanShutdown();
    });

    ::DokanInit();

    WSADATA WSAData = { 0 };
    {
        int WSAError = ::WSAStartup(MAKEWORD(2, 2), &WSAData);
        if (NO_ERROR != WSAError)
        {
            std::wprintf(
                L"[ERROR] WSAStartup failed (%d).\n",
                WSAError);
            return -1;
        }
    }

    try
    {
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
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return -1;
    }

    //::Test();

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
    Operations.GetFileInformation = ::MileCirnoGetFileInformation;
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
