/*
 * PROJECT:    Mile.Cirno
 * FILE:       Mile.Cirno.cpp
 * PURPOSE:    Implementation for Mile.Cirno
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#define _WINSOCKAPI_
#define WIN32_NO_STATUS
#include <Windows.h>
#include <WinSock2.h>
#include <hvsocket.h>

#include <Mile.Project.Version.h>

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

#include "Aptx.Posix.Error.h"
#include "Aptx.Posix.FileMode.h"

NTSTATUS ToNtStatus(
    std::uint32_t ErrorCode)
{
    switch (ErrorCode)
    {
    case 0:
        return STATUS_SUCCESS;
    case APTX_EPERM:
        return STATUS_NOT_SUPPORTED;
    case APTX_ENOENT:
        return STATUS_OBJECT_NAME_NOT_FOUND;
    case APTX_ESRCH:
        return STATUS_OBJECTID_NOT_FOUND;
    case APTX_EINTR: // fallthrough
    case APTX_LINUX_ERESTARTSYS: // fallthrough
    case APTX_LINUX_ERESTARTNOINTR: // fallthrough
    case APTX_LINUX_ERESTARTNOHAND: // fallthrough
    case APTX_LINUX_ERESTART_RESTARTBLOCK:
        return STATUS_CANCELLED;
    case APTX_EIO:
        return STATUS_IO_DEVICE_ERROR;
    case APTX_ENXIO:
        return STATUS_NO_SUCH_DEVICE;
    case APTX_EBADF:
        return STATUS_INVALID_HANDLE;
    case APTX_LINUX_EAGAIN:
        return STATUS_RETRY;
    case APTX_ENOMEM: // fallthrough
    case APTX_ENFILE: // fallthrough
    case APTX_EMFILE:
        return STATUS_INSUFFICIENT_RESOURCES;
    case APTX_EACCES:
        return STATUS_ACCESS_DENIED;
    case APTX_EFAULT:
        return STATUS_ACCESS_VIOLATION;
    case APTX_EEXIST:
        return STATUS_OBJECT_NAME_COLLISION;
    case APTX_ENODEV:
        return STATUS_DEVICE_NOT_CONNECTED;
    case APTX_ENOTDIR:
        return STATUS_NOT_A_DIRECTORY;
    case APTX_EISDIR:
        return STATUS_FILE_IS_A_DIRECTORY;
    case APTX_EINVAL:
        return STATUS_INVALID_PARAMETER;
    case APTX_EFBIG: // fallthrough
    case APTX_ENOSPC:
        return STATUS_DISK_FULL;
    case APTX_EROFS:
        return STATUS_MEDIA_WRITE_PROTECTED;
    case APTX_EMLINK: // fallthrough
    case APTX_LINUX_ELOOP:
        return STATUS_REPARSE_POINT_NOT_RESOLVED;
    case APTX_EPIPE:
        return STATUS_CONNECTION_DISCONNECTED;
    case APTX_ERANGE:
        return STATUS_INTEGER_OVERFLOW;
    case APTX_LINUX_ENAMETOOLONG:
        return STATUS_OBJECT_NAME_INVALID;
    case APTX_LINUX_ENOTEMPTY:
        return STATUS_DIRECTORY_NOT_EMPTY;
    case APTX_LINUX_EOVERFLOW:
        return STATUS_BUFFER_TOO_SMALL;
    case APTX_LINUX_ENOMEDIUM:
        return STATUS_NO_MEDIA_IN_DEVICE;
    case APTX_LINUX_EMEDIUMTYPE:
        return STATUS_UNRECOGNIZED_MEDIA;
    default:
        break;
    }

    return STATUS_UNSUCCESSFUL;
}

NTSTATUS GetNtStatusAndLogToConsole(
    std::string const& CheckPoint) noexcept
{
    try
    {
        throw;
    }
    catch (Mile::Cirno::LinuxErrorResponse const& ex)
    {
        std::printf(
            "[Mile.Cirno] %s Failed. (Linux Error Code = %d)\n",
            CheckPoint.c_str(),
            ex.Code);
        return ::ToNtStatus(ex.Code);
    }
    catch (Mile::Cirno::ErrorResponse const& ex)
    {
        std::printf(
            "[Mile.Cirno] %s Failed. (Error Code = %d)\n",
            CheckPoint.c_str(),
            ex.Code);
        if (ex.Code > APTX_ERANGE || 11 == ex.Code)
        {
            // Because there is no convention implementation for non-Linux
            // environment, returns STATUS_UNSUCCESSFUL if the error code is not
            // contained in Version 7 Unix Error Codes which are supported by
            // all major POSIX-compliant environments.
            return STATUS_UNSUCCESSFUL;
        }
        return ::ToNtStatus(ex.Code);
    }
    catch (std::exception const& ex)
    {
        std::printf(
            "[Mile.Cirno] %s Failed. (Exception: %s)\n",
            CheckPoint.c_str(),
            ex.what());
    }
    catch (...)
    {
        std::printf(
            "[Mile.Cirno] %s Failed. (Unknown Exception)\n",
            CheckPoint.c_str());
    }

    return STATUS_UNSUCCESSFUL;
}

// Win32 time epoch is 00:00:00, January 1 1601.
// UNIX time epoch is 00:00:00, January 1 1970.
// There are 11644473600 seconds between these two epochs.
const std::uint64_t SecondsBetweenWin32TimeAndUnixTime = 11644473600ULL;

// Win32 time is in 100-nanosecond intervals.
const std::uint64_t Win32TimeFrequency = 1000 * 1000 * 10;

FILETIME ToFileTime(
    std::uint64_t UnixTimeSeconds,
    std::uint64_t UnixTimeNanoseconds)
{
    std::uint64_t RawResult = UnixTimeSeconds;
    RawResult += SecondsBetweenWin32TimeAndUnixTime;
    RawResult *= Win32TimeFrequency;
    RawResult += UnixTimeNanoseconds / 100;
    FILETIME Result;
    Result.dwLowDateTime = static_cast<DWORD>(RawResult);
    Result.dwHighDateTime = static_cast<DWORD>(RawResult >> 32);
    return Result;
}

void FromFileTime(
    FILETIME const& FileTime,
    std::uint64_t& UnixTimeSeconds,
    std::uint64_t& UnixTimeNanoseconds)
{
    ULARGE_INTEGER RawValue;
    RawValue.LowPart = FileTime.dwLowDateTime;
    RawValue.HighPart = FileTime.dwHighDateTime;
    UnixTimeSeconds = RawValue.QuadPart / Win32TimeFrequency;
    UnixTimeSeconds -= SecondsBetweenWin32TimeAndUnixTime;
    UnixTimeNanoseconds = (RawValue.QuadPart % Win32TimeFrequency) * 100;
}

namespace
{
    Mile::Cirno::Client* g_Instance = nullptr;
    std::uint32_t g_RootDirectoryFileId = MILE_CIRNO_NOFID;
    std::uint32_t g_MaximumMessageSize = Mile::Cirno::DefaultMaximumMessageSize;
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

    std::wprintf(
        L"[INFO] FileName = %s\n",
        FileName);

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
        /*if ((GENERIC_READ | GENERIC_WRITE) & ConvertedDesiredAccess)
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
        }*/
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
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("ZwCreateFile");
    }

    return STATUS_SUCCESS;
}

void DOKAN_CALLBACK MileCirnoCleanup(
    _In_ LPCWSTR FileName,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return;
    }

    if (DokanFileInfo->DeletePending)
    {
        try
        {
            Mile::Cirno::RemoveRequest Request;
            Request.FileId = FileId;
            g_Instance->Remove(Request);
        }
        catch (...)
        {
            ::GetNtStatusAndLogToConsole("Cleanup");
        }
    }
}

void DOKAN_CALLBACK MileCirnoCloseFile(
    _In_ LPCWSTR FileName,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return;
    }

    try
    {
        Mile::Cirno::ClunkRequest Request;
        Request.FileId = FileId;
        g_Instance->Clunk(Request);
        g_Instance->FreeFileId(FileId);
    }
    catch (...)
    {
        ::GetNtStatusAndLogToConsole("CloseFile");
    }
}

NTSTATUS DOKAN_CALLBACK MileCirnoReadFile(
    _In_ LPCWSTR FileName,
    _Out_opt_ LPVOID Buffer,
    _In_ DWORD BufferLength,
    _Out_opt_ LPDWORD ReadLength,
    _In_ LONGLONG Offset,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_INVALID_HANDLE;
    }

    DWORD ProceededSize = 0;
    DWORD UnproceededSize = BufferLength;

    try
    {
        while (UnproceededSize)
        {
            Mile::Cirno::ReadRequest Request;
            Request.FileId = FileId;
            Request.Offset = Offset + ProceededSize;
            Request.Count = g_MaximumMessageSize;
            Request.Count -= Mile::Cirno::ReadResponseHeaderSize;
            if (UnproceededSize < Request.Count)
            {
                Request.Count = UnproceededSize;
            }
            Mile::Cirno::ReadResponse Response = g_Instance->Read(Request);
            DWORD CurrentProceededSize =
                static_cast<DWORD>(Response.Data.size());
            if (!CurrentProceededSize)
            {
                break;
            }
            if (Buffer)
            {
                std::memcpy(
                    static_cast<std::uint8_t*>(Buffer) + ProceededSize,
                    &Response.Data[0],
                    CurrentProceededSize);
            }
            ProceededSize += CurrentProceededSize;
            UnproceededSize -= CurrentProceededSize;
        }
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("ReadFile");
    }

    if (ReadLength)
    {
        *ReadLength = ProceededSize;
    }

    return STATUS_SUCCESS;
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
        return STATUS_INVALID_HANDLE;
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

        if (APTX_IFDIR & Response.Mode)
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
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("GetFileInformation");
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
        return STATUS_NOT_A_DIRECTORY;
    }

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_INVALID_HANDLE;
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
            Request.Count = g_MaximumMessageSize;
            Request.Count -= Mile::Cirno::ReadDirResponseHeaderSize;
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
                            catch (...)
                            {
                                ::GetNtStatusAndLogToConsole("FindFiles.Clunk");
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

                    if (APTX_IFDIR & InformationResponse.Mode)
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
                catch (...)
                {
                    ::GetNtStatusAndLogToConsole("FindFiles.Walk");
                }

                FillFindData(&FindData, DokanFileInfo);
            }
        } while (LastOffset);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("FindFiles");
    }

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoDeleteFile(
    _In_ LPCWSTR FileName,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(DokanFileInfo);

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoDeleteDirectory(
    _In_ LPCWSTR FileName,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_INVALID_HANDLE;
    }

    try
    {
        Mile::Cirno::ReadDirRequest Request;
        Request.FileId = FileId;
        Request.Offset = 0;
        Request.Count = g_MaximumMessageSize;
        Request.Count -= Mile::Cirno::ReadDirResponseHeaderSize;
        Mile::Cirno::ReadDirResponse Response =
            g_Instance->ReadDir(Request);
        if (Response.Data.size() > 2)
        {
            return STATUS_DIRECTORY_NOT_EMPTY;
        }
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("DeleteDirectory");
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
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("GetDiskFreeSpace");
    }

    return STATUS_SUCCESS;
}

int main()
{
    ::std::printf(
        "Mile.Cirno " MILE_PROJECT_VERSION_UTF8_STRING " (Build "
        MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")" "\n"
        "(c) Kenji Mouri. All rights reserved.\n"
        "\n");

    std::vector<std::string> Arguments = Mile::SplitCommandLineString(
        Mile::ToString(CP_UTF8, ::GetCommandLineW()));

    bool ParseSuccess = false;
    bool ShowHelp = false;
    std::string Host;
    std::string Port;
    std::string AccessName;
    std::string MountPoint;

    if (Arguments.empty() || 1 == Arguments.size())
    {
        std::printf(
            "[INFO] Mile.Cirno will run as the NanaBox EnableHostDriverStore "
            "integration mode.\n"
            "[INFO] Use \"Mile.Cirno Help\" for more commands.\n"
            "\n");

        ParseSuccess = true;
        Host = "HvSocket";
        Port = "50001";
        AccessName = "HostDriverStore";

        wchar_t System32Directory[MAX_PATH] = { 0 };
        ::GetSystemDirectoryW(
            System32Directory,
            sizeof(System32Directory) / sizeof(*System32Directory));
        MountPoint = Mile::ToString(CP_UTF8, System32Directory);
        MountPoint += "\\HostDriverStore";
    }
    else if (0 == ::_stricmp(Arguments[1].c_str(), "Help"))
    {
        ParseSuccess = true;
        ShowHelp = true;
    }
    else if (0 == ::_stricmp(Arguments[1].c_str(), "Mount"))
    {
        if (7 == Arguments.size() &&
            0 == ::_stricmp(Arguments[2].c_str(), "TCP"))
        {
            ParseSuccess = true;
            Host = Arguments[3];
            Port = Arguments[4];
            AccessName = Arguments[5];
            MountPoint = Arguments[6];
        }
        else if (6 == Arguments.size() &&
            0 == ::_stricmp(Arguments[2].c_str(), "HvSocket"))
        {
            ParseSuccess = true;
            Host = "HvSocket";
            Port = Arguments[3];
            AccessName = Arguments[4];
            MountPoint = Arguments[5];
        }
    }

    if (!ParseSuccess)
    {
        ShowHelp = true;
        std::printf(
            "[ERROR] Unrecognized command.\n"
            "\n");
    }

    if (ShowHelp)
    {
        std::printf(
            "Format: Mile.Cirno [Command] <Option1> <Option2> ...\n"
            "\n"
            "Commands:\n"
            "\n"
            "  Help - Show this content.\n"
            "\n"
            "  Mount TCP [Host] [Port] [AccessName] [MountPoint]\n"
            "    - Mount the specific 9p share over TCP.\n"
            "  Mount HvSocket [Port] [AccessName] [MountPoint]\n"
            "    - Mount the specific 9p share over Hyper-V Socket.\n"
            "\n"
            "Notes:\n"
            "  - All command options are case-insensitive.\n"
            "  - Mile.Cirno will run as the NanaBox EnableHostDriverStore\n"
            "    integration mode if you don't specify another command, which\n"
            "    is equivalent to the following command:\n"
            "      Mile.Cirno Mount HvSocket 50001 HostDriverStore "
            "%%SystemRoot%%\\System32\\HostDriverStore"
            "\n"
            "Examples:\n"
            "\n"
            "  Mile.Cirno Mount TCP 192.168.1.234 12345 MyShare C:\\MyMount\n"
            "  Mile.Cirno Mount HvSocket 50001 HostDriverStore Z:\\\n"
            "\n");
        return 0;
    }

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
        if (0 == ::_stricmp(Host.c_str(), "HvSocket"))
        {
            g_Instance = Mile::Cirno::Client::ConnectWithHyperVSocket(
                Mile::ToUInt32(Port));
        }
        else
        {
            g_Instance = Mile::Cirno::Client::ConnectWithTcpSocket(Host, Port);
        }

        {
            Mile::Cirno::VersionRequest Request;
            Request.MaximumMessageSize = g_MaximumMessageSize;
            Request.ProtocolVersion = Mile::Cirno::DefaultProtocolVersion;
            Mile::Cirno::VersionResponse Response =
                g_Instance->Version(Request);
            std::printf(
                "[INFO] VersionResponse.ProtocolVersion = %s\n"
                "[INFO] VersionResponse.MaximumMessageSize = %u\n",
                Response.ProtocolVersion.c_str(),
                Response.MaximumMessageSize);
            if (Mile::Cirno::DefaultProtocolVersion != Response.ProtocolVersion)
            {
                std::printf("[ERROR] The protocol version is not supported.\n");
                return -1;
            }
            g_MaximumMessageSize = Response.MaximumMessageSize;
        }

        {
            Mile::Cirno::AttachRequest Request;
            Request.FileId = g_Instance->AllocateFileId();
            Request.AuthenticationFileId = MILE_CIRNO_NOFID;
            Request.UserName = "";
            Request.AccessName = AccessName;
            Request.NumericUserName = MILE_CIRNO_NONUNAME;
            Mile::Cirno::AttachResponse Response = g_Instance->Attach(Request);
            g_RootDirectoryFileId = Request.FileId;
            std::printf(
                "[INFO] Response.UniqueId.Path = 0x%016llX\n",
                Response.UniqueId.Path);
        }
    }
    catch (...)
    {
        ::GetNtStatusAndLogToConsole("Initialize");
        return -1;
    }

    std::wstring ConvertedMountPoint = Mile::ToWideString(CP_UTF8, MountPoint);

    DOKAN_OPTIONS Options = { 0 };
    Options.Version = DOKAN_VERSION;
    Options.SingleThread;
    Options.Options =
        DOKAN_OPTION_WRITE_PROTECT |
        DOKAN_OPTION_MOUNT_MANAGER |
        DOKAN_OPTION_CASE_SENSITIVE;
    Options.GlobalContext;
    Options.MountPoint = ConvertedMountPoint.c_str();
    Options.UNCName;
    Options.Timeout = INFINITE;
    Options.AllocationUnitSize;
    Options.SectorSize;
    Options.VolumeSecurityDescriptorLength;
    Options.VolumeSecurityDescriptor;

    DOKAN_OPERATIONS Operations = { 0 };
    Operations.ZwCreateFile = ::MileCirnoZwCreateFile; //
    Operations.Cleanup = ::MileCirnoCleanup;
    Operations.CloseFile = ::MileCirnoCloseFile;
    Operations.ReadFile = ::MileCirnoReadFile;
    Operations.WriteFile; //
    Operations.FlushFileBuffers;
    Operations.GetFileInformation = ::MileCirnoGetFileInformation;
    Operations.FindFiles = ::MileCirnoFindFiles;
    Operations.FindFilesWithPattern = nullptr;
    Operations.SetFileAttributesW;
    Operations.SetFileTime;
    Operations.DeleteFileW = ::MileCirnoDeleteFile;
    Operations.DeleteDirectory = ::MileCirnoDeleteDirectory;
    Operations.MoveFileW; //
    Operations.SetEndOfFile; //
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
