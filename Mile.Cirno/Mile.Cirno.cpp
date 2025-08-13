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
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return DokanFileInfo->IsDirectory
            ? STATUS_OBJECT_PATH_NOT_FOUND
            : STATUS_OBJECT_NAME_NOT_FOUND;
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
        Mile::Cirno::ClunkRequest Request;
        Request.FileId = FileId;
        g_Instance->Clunk(Request);
        g_Instance->FreeFileId(FileId);
    }
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
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
        return STATUS_NOT_IMPLEMENTED;
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
            Request.Count = Mile::Cirno::DefaultMaximumMessageSize;
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
    catch (std::exception const& ex)
    {
        std::printf("%s\n", ex.what());
        return STATUS_NOT_IMPLEMENTED;
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
        return STATUS_NOT_IMPLEMENTED;
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
        return STATUS_NOT_IMPLEMENTED;
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
        return STATUS_NOT_IMPLEMENTED;
    }

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_NOT_IMPLEMENTED;
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
            Request.Count = Mile::Cirno::DefaultMaximumMessageSize;
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
        return STATUS_NOT_IMPLEMENTED;
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
        return STATUS_NOT_IMPLEMENTED;
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
        }
        if (!g_Instance)
        {
            Mile::Cirno::ThrowException(
                "!Instance",
                ERROR_INVALID_DATA);
        }

        {
            Mile::Cirno::VersionRequest Request;
            Request.MaximumMessageSize =
                Mile::Cirno::DefaultMaximumMessageSize;
            Request.ProtocolVersion =
                Mile::Cirno::DefaultProtocolVersion;
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
            Request.AccessName = AccessName;
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
    Operations.ZwCreateFile = ::MileCirnoZwCreateFile;
    Operations.Cleanup;
    Operations.CloseFile = ::MileCirnoCloseFile;
    Operations.ReadFile = ::MileCirnoReadFile;
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

// References:
// - https://github.com/freebsd/freebsd-src/tree/release/14.3.0
//   - /sys/compat/linux/linux_errno.h
//   - /sys/compat/linuxkpi/common/include/linux/errno.h
// - https://git.musl-libc.org/cgit/musl/tree
//   - /src/errno/__strerror.h

// Operation not permitted
#define LX_EPERM 1
// No such file or directory
#define LX_ENOENT 2
// No such process
#define LX_ESRCH 3
// Interrupted system call
#define LX_EINTR 4
// I/O error
#define LX_EIO 5
// No such device or address
#define LX_ENXIO 6
// Argument list too long
#define LX_E2BIG 7
// Exec format error
#define LX_ENOEXEC 8
// Bad file descriptor
#define LX_EBADF 9
// No child process
#define LX_ECHILD 10
// Resource temporarily unavailable
#define LX_EAGAIN 11
// Out of memory
#define LX_ENOMEM 12
// Permission denied
#define LX_EACCES 13
// Bad address
#define LX_EFAULT 14
// Block device required
#define LX_ENOTBLK 15
// Resource busy
#define LX_EBUSY 16
// File exists
#define LX_EEXIST 17
// Cross-device link
#define LX_EXDEV 18
// No such device
#define LX_ENODEV 19
// Not a directory
#define LX_ENOTDIR 20
// Is a directory
#define LX_EISDIR 21
// Invalid argument
#define LX_EINVAL 22
// Too many open files in system
#define LX_ENFILE 23
// No file descriptors available
#define LX_EMFILE 24
// Not a tty
#define LX_ENOTTY 25
// Text file busy
#define LX_ETXTBSY 26
// File too large
#define LX_EFBIG 27
// No space left on device
#define LX_ENOSPC 28
// Invalid seek
#define LX_ESPIPE 29
// Read-only file system
#define LX_EROFS 30
// Too many links
#define LX_EMLINK 31
// Broken pipe
#define LX_EPIPE 32
// Domain error
#define LX_EDOM 33
// Result not representable
#define LX_ERANGE 34
// Resource deadlock would occur
#define LX_EDEADLK 35
// Filename too long
#define LX_ENAMETOOLONG 36
// No locks available
#define LX_ENOLCK 37
// Function not implemented
#define LX_ENOSYS 38
// Directory not empty
#define LX_ENOTEMPTY 39
// Symbolic link loop
#define LX_ELOOP 40
// Resource temporarily unavailable
#define LX_EWOULDBLOCK LX_EAGAIN
// No message of desired type
#define LX_ENOMSG 42
// Identifier removed
#define LX_EIDRM 43
#define LX_ECHRNG 44
#define LX_EL2NSYNC 45
#define LX_EL3HLT 46
#define LX_EL3RST 47
#define LX_ELNRNG 48
#define LX_EUNATCH 49
#define LX_ENOCSI 50
#define LX_EL2HLT 51
#define LX_EBADE 52
#define LX_EBADR 53
#define LX_EXFULL 54
#define LX_ENOANO 55
#define LX_EBADRQC 56
#define LX_EBADSLT 57
// Resource deadlock would occur
#define LX_EDEADLOCK LX_EDEADLK
#define LX_EBFONT 59
// Device not a stream
#define LX_ENOSTR 60
// No data available
#define LX_ENODATA 61
// Device timeout
#define LX_ENOTIME 62
// Out of streams resources
#define LX_ENOSR 63
#define LX_ENONET 64
#define LX_ENOPKG 65
#define LX_EREMOTE 66
// Link has been severed
#define LX_ENOLINK 67
#define LX_EADV 68
#define LX_ESRMNT 69
#define LX_ECOMM 70
// Protocol error
#define LX_EPROTO 71
// Multihop attempted
#define LX_EMULTIHOP 72
#define LX_EDOTDOT 73
// Bad message
#define LX_EBADMSG 74
// Value too large for data type
#define LX_EOVERFLOW 75
#define LX_ENOTUNIQ 76
// File descriptor in bad state
#define LX_EBADFD 77
#define LX_EREMCHG 78
#define LX_ELIBACC 79
#define LX_ELIBBAD 80
#define LX_ELIBSCN 81
#define LX_ELIBMAX 82
#define LX_ELIBEXEC 83
// Illegal byte sequence
#define LX_EILSEQ 84
#define LX_ERESTART 85
#define LX_ESTRPIPE 86
#define LX_EUSERS 87
// Not a socket
#define LX_ENOTSOCK 88
// Destination address required
#define LX_EDESTADDRREQ 89
// Message too large
#define LX_EMSGSIZE 90
// Protocol wrong type for socket
#define LX_EPROTOTYPE 91
// Protocol not available
#define LX_ENOPROTOOPT 92
// Protocol not supported
#define LX_EPROTONOTSUPPORT 93
// Socket type not supported
#define LX_ESOCKNOTSUPPORT 94
// Not supported
#define LX_EOPNOTSUPPORT 95
// Not supported
#define LX_ENOTSUP LX_EOPNOTSUPPORT
// Protocol family not supported
#define LX_EPFNOTSUPPORT 96
// Address family not supported by protocol
#define LX_EAFNOTSUPPORT 97
// Address in use
#define LX_EADDRINUSE 98
// Address not available
#define LX_EADDRNOTAVAIL 99
// Network is down
#define LX_ENETDOWN 100
// Network unreachable
#define LX_ENETUNREACH 101
// Connection reset by network
#define LX_ENETRESET 102
// Connection aborted
#define LX_ECONNABORTED 103
// Connection reset by peer
#define LX_ECONNRESET 104
// No buffer space available
#define LX_ENOBUFS 105
// Socket is connected
#define LX_EISCONN 106
// Socket not connected
#define LX_ENOTCONN 107
// Cannot send after socket shutdown
#define LX_ESHUTDOWN 108
#define LX_ETOOMANYREFS 109
// Operation timed out
#define LX_ETIMEDOUT 110
// Connection refused
#define LX_ECONNREFUSED 111
// Host is down
#define LX_EHOSTDOWN 112
// Host is unreachable
#define LX_EHOSTUNREACH 113
// Operation already in progress
#define LX_EALREADY 114
// Operation in progress
#define LX_EINPROGRESS 115
// Stale file handle
#define LX_ESTALE 116
#define LX_EUCLEAN 117
#define LX_ENOTNAM 118
#define LX_ENAVAIL 119
#define LX_EISNAM 120
// Remote I/O error
#define LX_EREMOTEIO 121
// Quota exceeded
#define LX_EDQUOT 122
// No medium found
#define LX_ENOMEDIUM 123
// Wrong medium type
#define LX_EMEDIUMTYPE 124
// Operation canceled
#define LX_ECANCELED 125
// Required key not available
#define LX_ENOKEY 126
// Key has expired
#define LX_EKEYEXPIRED 127
// Key has been revoked
#define LX_EKEYREVOKED 128
// Key was rejected by service
#define LX_EKEYREJECTED 129
// Previous owner died
#define LX_EOWNERDEAD 130
// State not recoverable
#define LX_ENOTRECOVERABLE 131
#define LX_ERFKILL 132
#define LX_EHWPOISON 133
#define LX_ERESTARTSYS 512
#define LX_ERESTARTNOINTR 513
#define LX_ERESTARTNOHAND 514
#define LX_ENOIOCTLCMD 515
#define LX_ERESTART_RESTARTBLOCK 516
#define LX_EPROBE_DEFER 517
#define LX_EOPENSTALE 518
#define LX_ENOPARAM 519

BOOLEAN LxUtilLxErrorToNtStatus(INT ErrorCode, PNTSTATUS Status)
{
    BOOLEAN Result = TRUE;

    switch (ErrorCode)
    {
    case 0:
        *Status = STATUS_SUCCESS;
        break;
    case -LX_EPERM:
        *Status = STATUS_NOT_SUPPORTED;
        break;
    case -LX_ENOENT:
        *Status = STATUS_OBJECT_NAME_NOT_FOUND;
        break;
    case -LX_ESRCH:
        *Status = STATUS_OBJECTID_NOT_FOUND;
        break;
    case -LX_EINTR: // fallthrough
    case -LX_ERESTARTSYS: // fallthrough
    case -LX_ERESTARTNOINTR: // fallthrough
    case -LX_ERESTARTNOHAND: // fallthrough
    case -LX_ERESTART_RESTARTBLOCK:
        *Status = STATUS_CANCELLED;
        break;
    case -LX_EIO:
        *Status = STATUS_IO_DEVICE_ERROR;
        break;
    case -LX_ENXIO:
        *Status = STATUS_NO_SUCH_DEVICE;
        break;
    case -LX_EBADF:
        *Status = STATUS_INVALID_HANDLE;
        break;
    case -LX_EAGAIN:
        *Status = STATUS_RETRY;
        break;
    case -LX_ENOMEM: // fallthrough
    case -LX_ENFILE: // fallthrough
    case -LX_EMFILE:
        *Status = STATUS_INSUFFICIENT_RESOURCES;
        break;
    case -LX_EACCES:
        *Status = STATUS_ACCESS_DENIED;
        break;
    case -LX_EFAULT:
        *Status = STATUS_ACCESS_VIOLATION;
        break;
    case -LX_EEXIST:
        *Status = STATUS_OBJECT_NAME_COLLISION;
        break;
    case -LX_ENODEV:
        *Status = STATUS_DEVICE_NOT_CONNECTED;
        break;
    case -LX_ENOTDIR:
        *Status = STATUS_NOT_A_DIRECTORY;
        break;
    case -LX_EISDIR:
        *Status = STATUS_FILE_IS_A_DIRECTORY;
        break;
    case -LX_EINVAL:
        *Status = STATUS_INVALID_PARAMETER;
        break;
    case -LX_EFBIG: // fallthrough
    case -LX_ENOSPC:
        *Status = STATUS_DISK_FULL;
        break;
    case -LX_EROFS:
        *Status = STATUS_MEDIA_WRITE_PROTECTED;
        break;
    case -LX_EMLINK: // fallthrough
    case -LX_ELOOP:
        *Status = STATUS_REPARSE_POINT_NOT_RESOLVED;
        break;
    case -LX_EPIPE:
        *Status = STATUS_CONNECTION_DISCONNECTED;
        break;
    case -LX_ERANGE:
        *Status = STATUS_INTEGER_OVERFLOW;
        break;
    case -LX_ENAMETOOLONG:
        *Status = STATUS_OBJECT_NAME_INVALID;
        break;
    case -LX_ENOTEMPTY:
        *Status = STATUS_DIRECTORY_NOT_EMPTY;
        break;
    case -LX_EOVERFLOW:
        *Status = STATUS_BUFFER_TOO_SMALL;
        break;
    case -LX_ENOMEDIUM:
        *Status = STATUS_NO_MEDIA_IN_DEVICE;
        break;
    case -LX_EMEDIUMTYPE:
        *Status = STATUS_UNRECOGNIZED_MEDIA;
        break;
    default:
        Result = FALSE;
        *Status = STATUS_UNSUCCESSFUL;
        break;
    }

    return Result;
}
