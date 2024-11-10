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

 // Win32 time epoch is 00:00:00, January 1 1601.
 // UNIX time epoch is 00:00:00, January 1 1970.
 // There are 11644473600 seconds between these two epochs.
const std::uint64_t SecondsBetweenWin32TimeAndUnixTime = 11644473600ULL;

FILETIME ToFileTime(
    std::uint64_t UnixTimeSeconds,
    std::uint64_t UnixTimeNanoseconds)
{
    std::uint64_t RawResult = UnixTimeSeconds;
    RawResult += SecondsBetweenWin32TimeAndUnixTime;
    RawResult *= 1000 * 1000 * 10;
    RawResult += UnixTimeNanoseconds / 100;
    FILETIME Result;
    Result.dwLowDateTime = static_cast<DWORD>(RawResult);
    Result.dwHighDateTime = static_cast<DWORD>(RawResult >> 32);      
    return Result;
}

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
    UNREFERENCED_PARAMETER(SecurityContext);
    UNREFERENCED_PARAMETER(ShareAccess);

    ACCESS_MASK ConvertedDesiredAccess = 0;
    DWORD ConvertedFlagsAndAttributes = 0;
    DWORD ConvertedCreationDisposition = 0;
    ::DokanMapKernelToUserCreateFileFlags(
        DesiredAccess,
        FileAttributes,
        CreateOptions,
        CreateDisposition,
        &ConvertedDesiredAccess,
        &ConvertedFlagsAndAttributes,
        &ConvertedCreationDisposition);

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

        Mile::Cirno::GetAttrRequest GetAttrRequest;
        GetAttrRequest.FileId = WalkRequest.NewFileId;
        GetAttrRequest.RequestMask = MileCirnoLinuxGetAttrFlagMode;
        Mile::Cirno::GetAttrResponse GetAttrResponse =
            g_Instance->GetAttr(GetAttrRequest);

        bool RequestCreate = false;
        std::uint32_t Flags =
            MileCirnoLinuxOpenCreateFlagLargeFile |
            MileCirnoLinuxOpenCreateFlagCloseOnExecute;
        if ((GENERIC_READ | GENERIC_WRITE) & ConvertedDesiredAccess)
        {
            Flags |= MileCirnoLinuxOpenCreateFlagReadWrite;
        }
        else if (GENERIC_READ & ConvertedDesiredAccess)
        {
            Flags |= MileCirnoLinuxOpenCreateFlagReadOnly;
        }
        else if (GENERIC_WRITE & ConvertedDesiredAccess)
        {
            Flags |= MileCirnoLinuxOpenCreateFlagWriteOnly;
        }
        if (FILE_FLAG_OVERLAPPED & ConvertedFlagsAndAttributes)
        {
            Flags |= MileCirnoLinuxOpenCreateFlagNonBlock;
        }
        switch (ConvertedCreationDisposition)
        {
        case CREATE_NEW:
            RequestCreate = true;
            Flags |= MileCirnoLinuxOpenCreateFlagCreate;
            Flags |= MileCirnoLinuxOpenCreateFlagCreateOnlyWhenNotExist;
            break;
        case CREATE_ALWAYS:
            RequestCreate = true;
            Flags |= MileCirnoLinuxOpenCreateFlagCreate;
            break;
        case OPEN_EXISTING:
            break;
        case OPEN_ALWAYS:
            Flags |= MileCirnoLinuxOpenCreateFlagCreate;
            break;
        case TRUNCATE_EXISTING:
            Flags |= MileCirnoLinuxOpenCreateFlagTruncate;
            break;
        default:
            break;
        }
        if (S_IFDIR & GetAttrResponse.Mode)
        {
            Flags |= MileCirnoLinuxOpenCreateFlagDirectory;
        }

        {
            Mile::Cirno::LinuxOpenRequest Request;
            Request.FileId = WalkRequest.NewFileId;
            Request.Flags = Flags;
            g_Instance->LinuxOpen(Request);
        }

        DokanFileInfo->Context = WalkRequest.NewFileId;
        if (S_IFDIR & GetAttrResponse.Mode)
        {
            DokanFileInfo->IsDirectory = TRUE;
        }
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

void DOKAN_CALLBACK MileCirnoCloseFile(
    _In_ LPCWSTR FileName,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID != FileId)
    {
        return;
    }

    try
    {
        Mile::Cirno::ClunkRequest ClunkRequest;
        ClunkRequest.FileId = FileId;
        g_Instance->Clunk(ClunkRequest);
        g_Instance->FreeFileId(FileId);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
    }
}

NTSTATUS DOKAN_CALLBACK MileCirnoGetFileInformation(
    _In_ LPCWSTR FileName,
    _Out_ LPBY_HANDLE_FILE_INFORMATION Buffer,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_UNSUCCESSFUL;
    }

    std::memset(Buffer, 0, sizeof(BY_HANDLE_FILE_INFORMATION));

    try
    {
        Mile::Cirno::GetAttrRequest Request;
        Request.FileId = FileId;
        Request.RequestMask =
            MileCirnoLinuxGetAttrFlagMode |
            MileCirnoLinuxGetAttrFlagNumberOfHardLinks |
            MileCirnoLinuxGetAttrFlagLastAccessTime |
            MileCirnoLinuxGetAttrFlagLastWriteTime |
            MileCirnoLinuxGetAttrFlagSize;
        Mile::Cirno::GetAttrResponse Response = g_Instance->GetAttr(Request);

        if (S_IFDIR & Response.Mode)
        {
            Buffer->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
        }

        Buffer->ftCreationTime;

        Buffer->ftLastAccessTime = ::ToFileTime(
            Response.LastAccessTimeSeconds,
            Response.LastAccessTimeNanoseconds);
        Buffer->ftLastWriteTime = ::ToFileTime(
            Response.LastWriteTimeSeconds,
            Response.LastWriteTimeNanoseconds);

        Buffer->dwVolumeSerialNumber;

        Buffer->nFileSizeHigh =
            static_cast<DWORD>(Response.FileSize >> 32);
        Buffer->nFileSizeLow =
            static_cast<DWORD>(Response.FileSize);

        Buffer->nNumberOfLinks =
            static_cast<DWORD>(Response.NumberOfHardLinks);

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

    if (!DokanFileInfo->IsDirectory)
    {
        return STATUS_UNSUCCESSFUL;
    }

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_UNSUCCESSFUL;
    }

    try
    {
        std::uint64_t LastOffset = 0;
        do
        {
            Mile::Cirno::ReadDirRequest Request;
            Request.FileId = FileId;
            Request.Offset = LastOffset;
            LastOffset = 0;
            Request.Count =
                (1 << 16) - Mile::Cirno::HeaderSize - sizeof(std::uint32_t);
            Mile::Cirno::ReadDirResponse Response =
                g_Instance->ReadDir(Request);
            for (Mile::Cirno::DirectoryEntry const& Entry : Response.Data)
            {
                LastOffset = Entry.Offset;

                if ("." == Entry.Name || ".." == Entry.Name)
                {
                    continue;
                }

                WIN32_FIND_DATAW FindData = { 0 };
                FindData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                ::wcscpy_s(
                    FindData.cFileName,
                    Mile::ToWideString(CP_UTF8, Entry.Name).c_str());

                try
                {
                    Mile::Cirno::WalkRequest WalkRequest;
                    WalkRequest.FileId = FileId;
                    WalkRequest.NewFileId = g_Instance->AllocateFileId();
                    WalkRequest.Names.push_back(Entry.Name);
                    g_Instance->Walk(WalkRequest);
                    auto CurrentCleanupHandler = Mile::ScopeExitTaskHandler([&]()
                    {
                        if (MILE_CIRNO_NOFID != WalkRequest.NewFileId)
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

                    Mile::Cirno::GetAttrRequest InformationRequest;
                    InformationRequest.FileId = WalkRequest.NewFileId;
                    InformationRequest.RequestMask =
                        MileCirnoLinuxGetAttrFlagMode |
                        MileCirnoLinuxGetAttrFlagNumberOfHardLinks |
                        MileCirnoLinuxGetAttrFlagLastAccessTime |
                        MileCirnoLinuxGetAttrFlagLastWriteTime |
                        MileCirnoLinuxGetAttrFlagSize;
                    Mile::Cirno::GetAttrResponse InformationResponse =
                        g_Instance->GetAttr(InformationRequest);

                    if (S_IFDIR & InformationResponse.Mode)
                    {
                        FindData.dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                    }
                    
                    FindData.ftCreationTime;

                    FindData.ftLastAccessTime = ::ToFileTime(
                        InformationResponse.LastAccessTimeSeconds,
                        InformationResponse.LastAccessTimeNanoseconds);
                    FindData.ftLastWriteTime = ::ToFileTime(
                        InformationResponse.LastWriteTimeSeconds,
                        InformationResponse.LastWriteTimeNanoseconds);
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

NTSTATUS DOKAN_CALLBACK MileCirnoGetDiskFreeSpace(
    _Out_opt_ PULONGLONG FreeBytesAvailable,
    _Out_opt_ PULONGLONG TotalNumberOfBytes,
    _Out_opt_ PULONGLONG TotalNumberOfFreeBytes,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    try
    {
        Mile::Cirno::StatFsRequest Request;
        Request.FileId = g_RootDirectoryFileId;
        Mile::Cirno::StatFsResponse Response = g_Instance->StatFs(Request);
        if (FreeBytesAvailable)
        {
            *FreeBytesAvailable = Response.BlockSize * Response.AvailableBlocks;
        }
        if (TotalNumberOfBytes)
        {
            *TotalNumberOfBytes = Response.BlockSize * Response.TotalBlocks;
        }
        if (TotalNumberOfFreeBytes)
        {
            *TotalNumberOfFreeBytes = Response.BlockSize * Response.FreeBlocks;
        }
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
        DOKAN_OPTION_WRITE_PROTECT |
        DOKAN_OPTION_MOUNT_MANAGER |
        DOKAN_OPTION_CASE_SENSITIVE;
    Options.GlobalContext;
    Options.MountPoint = L"C:\\Windows\\System32\\HostDriverStore";
    Options.UNCName;
    Options.Timeout = INFINITE;
    Options.AllocationUnitSize;
    Options.SectorSize;
    Options.VolumeSecurityDescriptorLength;
    Options.VolumeSecurityDescriptor;

    DOKAN_OPERATIONS Operations = { 0 };
    Operations.ZwCreateFile = ::MileCirnoZwCreateFile;
    Operations.Cleanup;
    Operations.CloseFile = ::MileCirnoCloseFile;
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
    Operations.GetDiskFreeSpaceW = ::MileCirnoGetDiskFreeSpace;
    Operations.GetVolumeInformationW;
    Operations.Mounted;
    Operations.Unmounted;
    Operations.GetFileSecurityW;
    Operations.SetFileSecurityW;
    Operations.FindStreams;
    return ::DokanMain(&Options, &Operations);
}
