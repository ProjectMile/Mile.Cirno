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

std::uint32_t SimpleAttach(
    std::uint32_t const& AuthenticationFileId,
    std::string const& UserName,
    std::string const& AccessName,
    std::uint32_t const& NumericUserName)
{
    Mile::Cirno::AttachRequest Request;
    Request.FileId = g_Instance->AllocateFileId();
    Request.AuthenticationFileId = AuthenticationFileId;
    Request.UserName = UserName;
    Request.AccessName = AccessName;
    Request.NumericUserName = NumericUserName;
    g_Instance->Attach(Request);
    return Request.FileId;
}

std::uint32_t SimpleWalk(
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath)
{
    Mile::Cirno::WalkRequest WalkRequest;
    WalkRequest.FileId = RootDirectoryFileId;
    WalkRequest.NewFileId = g_Instance->AllocateFileId();
    for (std::filesystem::path const& Element : RelativeFilePath)
    {
        WalkRequest.Names.push_back(Element.string());
    }
    g_Instance->Walk(WalkRequest);
    return WalkRequest.NewFileId;
}

void SimpleClunk(
    std::uint32_t const& FileId)
{
    Mile::Cirno::ClunkRequest Request;
    Request.FileId = FileId;
    g_Instance->Clunk(Request);
    g_Instance->FreeFileId(FileId);
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

    bool MaybeRequestLinuxCreate = false;
    bool MaybeRequestCreateDirectory = false;
    std::uint32_t FileMode = 0;
    std::uint32_t Flags =
        MileCirnoLinuxOpenCreateFlagLargeFile |
        MileCirnoLinuxOpenCreateFlagCloseOnExecute;
    if ((FILE_GENERIC_READ | FILE_GENERIC_WRITE) & ConvertedDesiredAccess)
    {
        Flags |= MileCirnoLinuxOpenCreateFlagReadWrite;
        FileMode |= APTX_IRUSR | APTX_IRGRP | APTX_IROTH;
        FileMode |= APTX_IWUSR | APTX_IWGRP | APTX_IWOTH;
    }
    else if (FILE_GENERIC_READ & ConvertedDesiredAccess)
    {
        Flags |= MileCirnoLinuxOpenCreateFlagReadOnly;
        FileMode |= APTX_IRUSR | APTX_IRGRP | APTX_IROTH;
    }
    else if (FILE_GENERIC_WRITE & ConvertedDesiredAccess)
    {
        Flags |= MileCirnoLinuxOpenCreateFlagWriteOnly;
        FileMode |= APTX_IWUSR | APTX_IWGRP | APTX_IWOTH;
    }
    if (FILE_FLAG_OVERLAPPED & ConvertedFlagsAndAttributes)
    {
        Flags |= MileCirnoLinuxOpenCreateFlagNonBlock;
    }
    switch (ConvertedCreationDisposition)
    {
    case CREATE_NEW:
        MaybeRequestLinuxCreate = true;
        if (DokanFileInfo->IsDirectory)
        {
            MaybeRequestCreateDirectory = true;
        }
        Flags |= MileCirnoLinuxOpenCreateFlagCreateOnlyWhenNotExist;
        break;
    case CREATE_ALWAYS: // fallthrough
    case OPEN_ALWAYS:
        MaybeRequestLinuxCreate = true;
        if (DokanFileInfo->IsDirectory)
        {
            MaybeRequestCreateDirectory = true;
        }
        Flags |= MileCirnoLinuxOpenCreateFlagCreate;
        break;
    case OPEN_EXISTING:
        break;
    case TRUNCATE_EXISTING:
        Flags |= MileCirnoLinuxOpenCreateFlagTruncate;
        if ((MileCirnoLinuxOpenCreateFlagReadWrite & Flags) ||
            (MileCirnoLinuxOpenCreateFlagWriteOnly & Flags))
        {
            Flags |= MileCirnoLinuxOpenCreateFlagWriteOnly;
        }
        break;
    default:
        break;
    }

    std::uint32_t RootDirectoryFileId = MILE_CIRNO_NOFID;
    auto RootDirectoryFileIdCleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        try
        {
            if (MILE_CIRNO_NOFID != RootDirectoryFileId)
            {
                ::SimpleClunk(RootDirectoryFileId);
            }
        }
        catch (...)
        {
            ::GetNtStatusAndLogToConsole("ZwCreateFile");
        }
    });

    try
    {
        std::filesystem::path FilePath(&FileName[1]);

        RootDirectoryFileId = ::SimpleWalk(
            g_RootDirectoryFileId,
            FilePath.parent_path());

        std::uint32_t RootDirectoryFileMode = 0;
        std::uint32_t RootDirectoryGroupId = 0;
        {
            Mile::Cirno::GetAttributesRequest Request;
            Request.FileId = RootDirectoryFileId;
            Request.RequestMask =
                MileCirnoLinuxGetAttributesFlagMode |
                MileCirnoLinuxGetAttributesFlagGroupId;
            Mile::Cirno::GetAttributesResponse Response =
                g_Instance->GetAttributes(Request);
            RootDirectoryFileMode = Response.Mode;
            RootDirectoryGroupId = Response.GroupId;
        }

        std::uint32_t FileId = MILE_CIRNO_NOFID;
        try
        {
            FileId = ::SimpleWalk(RootDirectoryFileId, FilePath.filename());
        }
        catch (...)
        {
            if (MaybeRequestCreateDirectory)
            {
                Mile::Cirno::MakeDirectoryRequest Request;
                Request.DirectoryFileId = RootDirectoryFileId;
                Request.Name = FilePath.filename().string();
                Request.Mode = APTX_IRWXU;
                Request.Mode |= APTX_IRGRP | APTX_IXGRP;
                Request.Mode |= APTX_IROTH | APTX_IXOTH;
                Request.GroupId = RootDirectoryGroupId;
                g_Instance->MakeDirectory(Request);
                FileId = ::SimpleWalk(RootDirectoryFileId, FilePath.filename());
                DokanFileInfo->Context = FileId;
                std::printf(
                    "[Mile.Cirno] Directory %s created with FileId = %u\n",
                    FilePath.string().c_str(),
                    FileId);
                return STATUS_SUCCESS;
            }

            if (MaybeRequestLinuxCreate)
            {
                Mile::Cirno::LinuxCreateRequest Request;
                Request.FileId = RootDirectoryFileId;
                Request.Name = FilePath.filename().string();
                Request.Flags = Flags;
                Request.Mode = FileMode;
                Request.GroupId = RootDirectoryGroupId;
                g_Instance->LinuxCreate(Request);
                // Must be the full path to walk.
                FileId = ::SimpleWalk(g_RootDirectoryFileId, FilePath);
                DokanFileInfo->Context = FileId;
                std::printf(
                    "[Mile.Cirno] File %s created with FileId = %u\n",
                    FilePath.string().c_str(),
                    FileId);
                return STATUS_SUCCESS;
            }

            throw;
        }

        {
            Mile::Cirno::GetAttributesRequest Request;
            Request.FileId = FileId;
            Request.RequestMask = MileCirnoLinuxGetAttributesFlagMode;
            Mile::Cirno::GetAttributesResponse Response =
                g_Instance->GetAttributes(Request);
            if (APTX_IFDIR & Response.Mode)
            {
                DokanFileInfo->IsDirectory = TRUE;
                Flags |= MileCirnoLinuxOpenCreateFlagDirectory;
            }
        }

        Mile::Cirno::LinuxOpenRequest Request;
        Request.FileId = FileId;
        Request.Flags = Flags;
        try
        {
            g_Instance->LinuxOpen(Request);
        }
        catch (...)
        {
            Request.Flags &= ~MileCirnoLinuxOpenCreateFlagReadWrite;
            Request.Flags &= ~MileCirnoLinuxOpenCreateFlagWriteOnly;
            g_Instance->LinuxOpen(Request);
        }
        DokanFileInfo->Context = FileId;
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
        ::SimpleClunk(FileId);
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

NTSTATUS DOKAN_CALLBACK MileCirnoWriteFile(
    _In_ LPCWSTR FileName,
    _In_ LPCVOID Buffer,
    _In_ DWORD NumberOfBytesToWrite,
    _Out_opt_ LPDWORD NumberOfBytesWritten,
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
    DWORD UnproceededSize = NumberOfBytesToWrite;

    try
    {
        if (DokanFileInfo->WriteToEndOfFile || -1 == Offset)
        {
            Mile::Cirno::GetAttributesRequest Request;
            Request.FileId = FileId;
            Request.RequestMask = MileCirnoLinuxGetAttributesFlagSize;
            Mile::Cirno::GetAttributesResponse Response =
                g_Instance->GetAttributes(Request);
            Offset = Response.FileSize;
        }

        while (UnproceededSize)
        {
            Mile::Cirno::WriteRequest Request;
            Request.FileId = FileId;
            Request.Offset = Offset + ProceededSize;
            std::uint32_t RequestCount = g_MaximumMessageSize;
            RequestCount -= Mile::Cirno::WriteRequestHeaderSize;
            if (UnproceededSize < RequestCount)
            {
                RequestCount = UnproceededSize;
            }
            Request.Data.resize(RequestCount);
            std::memcpy(
                Request.Data.data(),
                static_cast<const std::uint8_t*>(Buffer) + ProceededSize,
                Request.Data.size());
            Mile::Cirno::WriteResponse Response = g_Instance->Write(Request);
            DWORD CurrentProceededSize = static_cast<DWORD>(Response.Count);
            if (!CurrentProceededSize)
            {
                break;
            }
            ProceededSize += CurrentProceededSize;
            UnproceededSize -= CurrentProceededSize;
        }
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("WriteFile");
    }

    if (NumberOfBytesWritten)
    {
        *NumberOfBytesWritten = ProceededSize;
    }

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoFlushFileBuffers(
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
        Mile::Cirno::FlushFileRequest Request = {};
        Request.FileId = FileId;
        g_Instance->FlushFile(Request);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("FlushFileBuffers");
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
        Mile::Cirno::GetAttributesRequest Request;
        Request.FileId = FileId;
        Request.RequestMask =
            MileCirnoLinuxGetAttributesFlagMode |
            MileCirnoLinuxGetAttributesFlagNumberOfHardLinks |
            MileCirnoLinuxGetAttributesFlagLastAccessTime |
            MileCirnoLinuxGetAttributesFlagLastWriteTime |
            MileCirnoLinuxGetAttributesFlagSize;
        Mile::Cirno::GetAttributesResponse Response =
            g_Instance->GetAttributes(Request);

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
            Mile::Cirno::ReadDirectoryRequest Request;
            Request.FileId = FileId;
            Request.Offset = LastOffset;
            LastOffset = 0;
            Request.Count = g_MaximumMessageSize;
            Request.Count -= Mile::Cirno::ReadDirectoryResponseHeaderSize;
            Mile::Cirno::ReadDirectoryResponse Response =
                g_Instance->ReadDirectory(Request);
            for (Mile::Cirno::DirectoryEntry const& Entry : Response.Data)
            {
                LastOffset = Entry.Offset;

                if ("." == Entry.Name || ".." == Entry.Name)
                {
                    continue;
                }

                WIN32_FIND_DATAW FindData = {};
                FindData.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                ::wcscpy_s(
                    FindData.cFileName,
                    Mile::ToWideString(CP_UTF8, Entry.Name).c_str());

                try
                {
                    std::uint32_t CurrentFileId =
                        ::SimpleWalk(FileId, Entry.Name);
                    auto CurrentCleanupHandler = Mile::ScopeExitTaskHandler([&]()
                    {
                        try
                        {
                            if (MILE_CIRNO_NOFID != CurrentFileId)
                            {
                                ::SimpleClunk(CurrentFileId);
                            }
                        }
                        catch (...)
                        {
                            ::GetNtStatusAndLogToConsole("FindFiles");
                        }
                    });

                    Mile::Cirno::GetAttributesRequest InformationRequest;
                    InformationRequest.FileId = CurrentFileId;
                    InformationRequest.RequestMask =
                        MileCirnoLinuxGetAttributesFlagMode |
                        MileCirnoLinuxGetAttributesFlagNumberOfHardLinks |
                        MileCirnoLinuxGetAttributesFlagLastAccessTime |
                        MileCirnoLinuxGetAttributesFlagLastWriteTime |
                        MileCirnoLinuxGetAttributesFlagSize;
                    Mile::Cirno::GetAttributesResponse InformationResponse =
                        g_Instance->GetAttributes(InformationRequest);

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
                    ::GetNtStatusAndLogToConsole("FindFiles");
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

NTSTATUS DOKAN_CALLBACK MileCirnoSetFileAttributes(
    _In_ LPCWSTR FileName,
    _In_ DWORD FileAttributes,
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
        Mile::Cirno::SetAttributesRequest Request = {};
        Request.FileId = FileId;
        Request.Valid = MileCirnoLinuxSetAttributesFlagMode;
        Request.Mode = APTX_IRUSR | APTX_IRGRP | APTX_IROTH;
        if (FILE_ATTRIBUTE_READONLY & FileAttributes)
        {
            Request.Mode |= APTX_IWUSR | APTX_IWGRP | APTX_IWOTH;
        }
        Request.Mode |= (FILE_ATTRIBUTE_DIRECTORY & FileAttributes)
            ? APTX_IFDIR
            : APTX_IFREG;
        if (FILE_ATTRIBUTE_REPARSE_POINT & FileAttributes)
        {
            Request.Mode |= APTX_IFLNK;
        }
        g_Instance->SetAttributes(Request);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("SetFileAttributes");
    }

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoSetFileTime(
    _In_ LPCWSTR FileName,
    _In_ CONST FILETIME* CreationTime,
    _In_ CONST FILETIME* LastAccessTime,
    _In_ CONST FILETIME* LastWriteTime,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(CreationTime);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_INVALID_HANDLE;
    }

    try
    {
        Mile::Cirno::SetAttributesRequest Request = {};
        Request.FileId = FileId;
        Request.Valid = 0;
        if (LastAccessTime)
        {
            Request.Valid =
                MileCirnoLinuxSetAttributesFlagLastAccessTime |
                MileCirnoLinuxSetAttributesFlagLastAccessTimeSet;
            ::FromFileTime(
                *LastAccessTime,
                Request.LastAccessTimeSeconds,
                Request.LastAccessTimeNanoseconds);
        }
        if (LastWriteTime)
        {
            Request.Valid =
                MileCirnoLinuxSetAttributesFlagLastWriteTime |
                MileCirnoLinuxSetAttributesFlagLastWriteTimeSet;
            ::FromFileTime(
                *LastWriteTime,
                Request.LastWriteTimeSeconds,
                Request.LastWriteTimeNanoseconds);
        }
        g_Instance->SetAttributes(Request);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("SetFileTime");
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
        Mile::Cirno::ReadDirectoryRequest Request;
        Request.FileId = FileId;
        Request.Offset = 0;
        Request.Count = g_MaximumMessageSize;
        Request.Count -= Mile::Cirno::ReadDirectoryResponseHeaderSize;
        Mile::Cirno::ReadDirectoryResponse Response =
            g_Instance->ReadDirectory(Request);
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

NTSTATUS DOKAN_CALLBACK MileCirnoMoveFile(
    _In_ LPCWSTR FileName,
    _In_ LPCWSTR NewFileName,
    _In_ BOOL ReplaceIfExisting,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(ReplaceIfExisting);

    std::uint32_t FileId = static_cast<std::uint32_t>(
        DokanFileInfo->Context);
    if (MILE_CIRNO_NOFID == FileId)
    {
        return STATUS_INVALID_HANDLE;
    }

    std::filesystem::path OldFilePath(&FileName[1]);
    std::filesystem::path NewFilePath(&NewFileName[1]);

    try
    {
        std::uint32_t OldDirectoryFileId =
            ::SimpleWalk(g_RootDirectoryFileId, OldFilePath.parent_path());

        std::uint32_t NewDirectoryFileId =
            ::SimpleWalk(g_RootDirectoryFileId, NewFilePath.parent_path());

        auto CleanupHandler = Mile::ScopeExitTaskHandler([&]()
        {
            try
            {
                if (MILE_CIRNO_NOFID != NewDirectoryFileId)
                {
                    ::SimpleClunk(NewDirectoryFileId);
                }

                if (MILE_CIRNO_NOFID != OldDirectoryFileId)
                {
                    ::SimpleClunk(OldDirectoryFileId);
                }
            }
            catch (...)
            {
                ::GetNtStatusAndLogToConsole("MoveFile");
            }
        });

        Mile::Cirno::RenameAtRequest Request;
        Request.OldDirectoryFileId = OldDirectoryFileId;
        Request.OldName = OldFilePath.filename().string();
        Request.NewDirectoryFileId = NewDirectoryFileId;
        Request.NewName = NewFilePath.filename().string();
        g_Instance->RenameAt(Request);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("MoveFile");
    }

    return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK MileCirnoSetEndOfFile(
    _In_ LPCWSTR FileName,
    _In_ LONGLONG ByteOffset,
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
        Mile::Cirno::SetAttributesRequest Request = {};
        Request.FileId = FileId;
        Request.Valid = MileCirnoLinuxSetAttributesFlagSize;
        Request.FileSize = ByteOffset;
        g_Instance->SetAttributes(Request);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("SetEndOfFile");
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
        Mile::Cirno::FileSystemStatusRequest Request;
        Request.FileId = g_RootDirectoryFileId;
        Mile::Cirno::FileSystemStatusResponse Response =
            g_Instance->FileSystemStatus(Request);
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

        wchar_t System32Directory[MAX_PATH] = {};
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
                ::SimpleClunk(g_RootDirectoryFileId);
            }
            delete g_Instance;
            g_Instance = nullptr;
        }

        ::WSACleanup();

        ::DokanShutdown();
    });

    ::DokanInit();

    WSADATA WSAData = {};
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

        g_RootDirectoryFileId = ::SimpleAttach(
            MILE_CIRNO_NOFID,
            "",
            AccessName,
            MILE_CIRNO_NONUNAME);
    }
    catch (...)
    {
        ::GetNtStatusAndLogToConsole("Initialize");
        return -1;
    }

    std::wstring ConvertedMountPoint = Mile::ToWideString(CP_UTF8, MountPoint);

    DOKAN_OPTIONS Options = {};
    Options.Version = DOKAN_VERSION;
    Options.SingleThread;
    Options.Options =
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

    DOKAN_OPERATIONS Operations = {};
    Operations.ZwCreateFile = ::MileCirnoZwCreateFile;
    Operations.Cleanup = ::MileCirnoCleanup;
    Operations.CloseFile = ::MileCirnoCloseFile;
    Operations.ReadFile = ::MileCirnoReadFile;
    Operations.WriteFile = ::MileCirnoWriteFile;
    Operations.FlushFileBuffers = ::MileCirnoFlushFileBuffers;
    Operations.GetFileInformation = ::MileCirnoGetFileInformation;
    Operations.FindFiles = ::MileCirnoFindFiles;
    Operations.FindFilesWithPattern = nullptr;
    Operations.SetFileAttributesW = ::MileCirnoSetFileAttributes;
    Operations.SetFileTime = ::MileCirnoSetFileTime;
    Operations.DeleteFileW = ::MileCirnoDeleteFile;
    Operations.DeleteDirectory = ::MileCirnoDeleteDirectory;
    Operations.MoveFileW = ::MileCirnoMoveFile;
    Operations.SetEndOfFile = ::MileCirnoSetEndOfFile;
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
