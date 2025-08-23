/*
 * PROJECT:    Mouri Internal Library Essentials
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

std::uint32_t CalculateFnv1aHash(
    std::string_view Data)
{
    // Reference: http://www.isthe.com/chongo/tech/comp/fnv/index.html

    const std::uint32_t FnvPrime = 0x01000193;
    const std::uint32_t FnvOffsetBasis = 0x811C9DC5;

    std::uint32_t Hash = FnvOffsetBasis;
    for (char const& Byte : Data)
    {
        Hash ^= static_cast<std::uint8_t>(Byte);
        Hash *= FnvPrime;
    }
    return Hash;
}

DWORD ToFileAttributes(
    std::uint32_t PosixFileMode)
{
    DWORD Result = APTX_IFDIR & PosixFileMode
        ? FILE_ATTRIBUTE_DIRECTORY
        : FILE_ATTRIBUTE_ARCHIVE;

    if (!((APTX_IWUSR | APTX_IWGRP | APTX_IWOTH) & PosixFileMode))
    {
        Result |= FILE_ATTRIBUTE_READONLY;
    }

    return Result;
}

namespace
{
    Mile::Cirno::Client* g_Instance = nullptr;
    std::string g_AccessName;
    std::uint32_t g_VolumeSerialNumber = 0;
    std::uint32_t g_RootDirectoryFileId = MILE_CIRNO_NOFID;
    std::uint32_t g_MaximumMessageSize = Mile::Cirno::DefaultMaximumMessageSize;
}

NTSTATUS MileCirnoSimpleClunk(
    std::uint32_t const& FileId)
{
    try
    {
        Mile::Cirno::ClunkRequest Request;
        Request.FileId = FileId;
        g_Instance->Clunk(Request);
        g_Instance->FreeFileId(FileId);
    }
    catch (...)
    {
        return ::GetNtStatusAndLogToConsole("MileCirnoSimpleClunk");
    }

    std::printf(
        "[Mile.Cirno] File ID %u is clunked.\n",
        FileId);

    return STATUS_SUCCESS;
}

NTSTATUS MileCirnoSimpleAttach(
    std::uint32_t& OutputFileId,
    std::uint32_t const& AuthenticationFileId,
    std::string const& UserName,
    std::string const& AccessName,
    std::uint32_t const& NumericUserName)
{
    try
    {
        OutputFileId = g_Instance->AllocateFileId();
        Mile::Cirno::AttachRequest Request;
        Request.FileId = OutputFileId;
        Request.AuthenticationFileId = AuthenticationFileId;
        Request.UserName = UserName;
        Request.AccessName = AccessName;
        Request.NumericUserName = NumericUserName;
        g_Instance->Attach(Request);
    }
    catch (...)
    {
        NTSTATUS Status = ::GetNtStatusAndLogToConsole(
            "MileCirnoSimpleAttach");
        ::MileCirnoSimpleClunk(OutputFileId);
        OutputFileId = MILE_CIRNO_NOFID;
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS MileCirnoSimpleWalk(
    std::uint32_t& OutputFileId,
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath)
{
    try
    {
        OutputFileId = g_Instance->AllocateFileId();
        Mile::Cirno::WalkRequest WalkRequest;
        WalkRequest.FileId = RootDirectoryFileId;
        WalkRequest.NewFileId = OutputFileId;
        for (std::filesystem::path const& Element : RelativeFilePath)
        {
            WalkRequest.Names.push_back(Mile::ToString(
                CP_UTF8,
                Element.wstring()));
        }
        g_Instance->Walk(WalkRequest);
    }
    catch (...)
    {
        NTSTATUS Status = ::GetNtStatusAndLogToConsole(
            "MileCirnoSimpleWalk");
        ::MileCirnoSimpleClunk(OutputFileId);
        OutputFileId = MILE_CIRNO_NOFID;
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS MileCirnoSimpleMakeDirectory(
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath)
{
    NTSTATUS Status = STATUS_SUCCESS;

    std::uint32_t RelativeRootDirectoryFileId = MILE_CIRNO_NOFID;
    Status = ::MileCirnoSimpleWalk(
        RelativeRootDirectoryFileId,
        RootDirectoryFileId,
        RelativeFilePath.parent_path());
    if (STATUS_SUCCESS == Status)
    {
        try
        {
            std::uint32_t RootDirectoryGroupId = 0;
            {
                Mile::Cirno::GetAttributesRequest Request;
                Request.FileId = RelativeRootDirectoryFileId;
                Request.RequestMask =
                    MileCirnoLinuxGetAttributesFlagGroupId;
                Mile::Cirno::GetAttributesResponse Response =
                    g_Instance->GetAttributes(Request);
                RootDirectoryGroupId = Response.GroupId;
            }

            {
                Mile::Cirno::MakeDirectoryRequest Request;
                Request.DirectoryFileId = RelativeRootDirectoryFileId;
                Request.Name = Mile::ToString(
                    CP_UTF8,
                    RelativeFilePath.filename().wstring());
                Request.Mode = APTX_IRWXU;
                Request.Mode |= APTX_IRGRP | APTX_IXGRP;
                Request.Mode |= APTX_IROTH | APTX_IXOTH;
                Request.GroupId = RootDirectoryGroupId;
                g_Instance->MakeDirectory(Request);
            }
        }
        catch (...)
        {
            Status = ::GetNtStatusAndLogToConsole(
                "MileCirnoSimpleMakeDirectory");
        }

        ::MileCirnoSimpleClunk(RelativeRootDirectoryFileId);
    }

    return Status;
}

NTSTATUS MileCirnoSimpleLinuxCreate(
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath,
    std::uint32_t Flags,
    std::uint32_t Mode)
{
    NTSTATUS Status = STATUS_SUCCESS;

    std::uint32_t RelativeRootDirectoryFileId = MILE_CIRNO_NOFID;
    Status = ::MileCirnoSimpleWalk(
        RelativeRootDirectoryFileId,
        RootDirectoryFileId,
        RelativeFilePath.parent_path());
    if (STATUS_SUCCESS == Status)
    {
        try
        {
            std::uint32_t RootDirectoryGroupId = 0;
            {
                Mile::Cirno::GetAttributesRequest Request;
                Request.FileId = RelativeRootDirectoryFileId;
                Request.RequestMask =
                    MileCirnoLinuxGetAttributesFlagGroupId;
                Mile::Cirno::GetAttributesResponse Response =
                    g_Instance->GetAttributes(Request);
                RootDirectoryGroupId = Response.GroupId;
            }

            {
                Mile::Cirno::LinuxCreateRequest Request;
                Request.FileId = RelativeRootDirectoryFileId;
                Request.Name = Mile::ToString(
                    CP_UTF8,
                    RelativeFilePath.filename().wstring());
                Request.Flags = Flags;
                Request.Mode = Mode;
                Request.GroupId = RootDirectoryGroupId;
                g_Instance->LinuxCreate(Request);
            }
        }
        catch (...)
        {
            Status = ::GetNtStatusAndLogToConsole(
                "MileCirnoSimpleLinuxCreate");
        }

        ::MileCirnoSimpleClunk(RelativeRootDirectoryFileId);
    }

    return Status;
}

#define MILE_CIRNO_ACCESS_READ ( \
    GENERIC_READ | \
    FILE_GENERIC_READ)
#define MILE_CIRNO_ACCESS_WRITE ( \
    GENERIC_WRITE | \
    FILE_GENERIC_WRITE | \
    DELETE | \
    WRITE_DAC | \
    WRITE_OWNER)
#define MILE_CIRNO_ACCESS_EXECUTE ( \
    GENERIC_EXECUTE | \
    FILE_GENERIC_EXECUTE)

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
    UNREFERENCED_PARAMETER(FileAttributes);
    UNREFERENCED_PARAMETER(ShareAccess);

    DokanFileInfo->Context = MILE_CIRNO_NOFID;

    std::wprintf(
        L"[INFO] FileName = %s\n",
        FileName);

    if (0 == ::_wcsicmp(FileName, L"\\System Volume Information") ||
        0 == ::_wcsicmp(FileName, L"\\$RECYCLE.BIN"))
    {
        std::wprintf(L"[INFO] %s is skipped.\n", FileName);
        return STATUS_NO_SUCH_FILE;
    }

    NTSTATUS Status = STATUS_SUCCESS;
    std::filesystem::path RelativeFilePath(&FileName[1]);

    if (FILE_DIRECTORY_FILE == (FILE_DIRECTORY_FILE & CreateOptions))
    {
        if (FILE_CREATE == CreateDisposition ||
            FILE_OPEN_IF == CreateDisposition)
        {
            Status = ::MileCirnoSimpleMakeDirectory(
                g_RootDirectoryFileId,
                RelativeFilePath);
            if (STATUS_SUCCESS != Status)
            {
                return Status;
            }
            CreateDisposition = FILE_OPEN;
        }
    }

    std::uint32_t ConvertedFileMode = 0;
    std::uint32_t ConvertedFlags =
        MileCirnoLinuxOpenCreateFlagLargeFile |
        MileCirnoLinuxOpenCreateFlagCloseOnExecute;

    bool Readable = false;
    bool Writable = false;
    if (GENERIC_ALL == DesiredAccess)
    {
        DesiredAccess = FILE_ALL_ACCESS;
    }
    if (MILE_CIRNO_ACCESS_READ & DesiredAccess)
    {
        Readable = true;
        ConvertedFileMode |= APTX_IRUSR | APTX_IRGRP | APTX_IROTH;
    }
    if (MILE_CIRNO_ACCESS_WRITE & DesiredAccess)
    {
        Writable = true;
        ConvertedFileMode |= APTX_IWUSR | APTX_IWGRP | APTX_IWOTH;
    }
    if (MILE_CIRNO_ACCESS_EXECUTE & DesiredAccess)
    {
        ConvertedFileMode |= APTX_IXUSR | APTX_IXGRP | APTX_IXOTH;
    }
    if (Readable && !Writable)
    {
        ConvertedFlags |= MileCirnoLinuxOpenCreateFlagReadOnly;
    }
    else if (!Readable && Writable)
    {
        ConvertedFlags |= MileCirnoLinuxOpenCreateFlagWriteOnly;
    }
    else if (Readable && Writable)
    {
        ConvertedFlags |= MileCirnoLinuxOpenCreateFlagReadWrite;
    }
    if (!(FILE_SYNCHRONOUS_IO_ALERT & CreateOptions) &&
        !(FILE_SYNCHRONOUS_IO_NONALERT & CreateOptions))
    {
        ConvertedFlags |= MileCirnoLinuxOpenCreateFlagNonBlock;
    }
    if (FILE_NO_INTERMEDIATE_BUFFERING & CreateOptions)
    {
        ConvertedFlags |= MileCirnoLinuxOpenCreateFlagDirect;
    }

    std::uint32_t FileId = MILE_CIRNO_NOFID;
    Status = ::MileCirnoSimpleWalk(
        FileId,
        g_RootDirectoryFileId,
        RelativeFilePath);
    if (STATUS_SUCCESS != Status)
    {
        if (DokanFileInfo->IsDirectory)
        {
            return STATUS_INVALID_PARAMETER;
        }

        if (FILE_OPEN == CreateDisposition ||
            FILE_OVERWRITE == CreateDisposition)
        {
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }

        // These are available dispositions:
        // - FILE_SUPERSEDE
        // - FILE_CREATE
        // - FILE_OPEN_IF
        // - FILE_OVERWRITE_IF
        // According to the documentation, these dispositions will create the
        // file if the file does not exist.

        Status = ::MileCirnoSimpleLinuxCreate(
            g_RootDirectoryFileId,
            RelativeFilePath,
            ConvertedFlags | MileCirnoLinuxOpenCreateFlagCreate,
            ConvertedFileMode);
        if (STATUS_SUCCESS != Status)
        {
            return Status;
        }
        CreateDisposition = FILE_OPEN;

        Status = ::MileCirnoSimpleWalk(
            FileId,
            g_RootDirectoryFileId,
            RelativeFilePath);
        if (STATUS_SUCCESS != Status)
        {
            return Status;
        }
    }

    if (FILE_CREATE == CreateDisposition)
    {
        Status = STATUS_OBJECT_NAME_COLLISION;
    }

    // These are available dispositions:
    // - FILE_SUPERSEDE
    // - FILE_OPEN
    // - FILE_OPEN_IF
    // - FILE_OVERWRITE
    // - FILE_OVERWRITE_IF
    // According to the documentation, these dispositions will only open the
    // file if the file exists and the dispositions is FILE_OPEN or
    // FILE_OPEN_IF, or will overwrite the file if the file exists.

    if (STATUS_SUCCESS == Status)
    {
        if (FILE_SUPERSEDE == CreateDisposition ||
            FILE_OVERWRITE == CreateDisposition ||
            FILE_OVERWRITE_IF == CreateDisposition)
        {
            ConvertedFlags |= MileCirnoLinuxOpenCreateFlagTruncate;
        }

        try
        {
            std::uint32_t FileMode = 0;
            {
                Mile::Cirno::GetAttributesRequest Request;
                Request.FileId = FileId;
                Request.RequestMask = MileCirnoLinuxGetAttributesFlagMode;
                Mile::Cirno::GetAttributesResponse Response =
                    g_Instance->GetAttributes(Request);
                FileMode = Response.Mode;
            }

            if (APTX_IFDIR & FileMode)
            {
                DokanFileInfo->IsDirectory = TRUE;
                ConvertedFlags |= MileCirnoLinuxOpenCreateFlagDirectory;
                if (FILE_NON_DIRECTORY_FILE & CreateOptions)
                {
                    Status = STATUS_OBJECT_NAME_NOT_FOUND;
                }
            }

            if (STATUS_SUCCESS == Status)
            {
                Mile::Cirno::LinuxOpenRequest Request;
                Request.FileId = FileId;
                Request.Flags = ConvertedFlags;
                try
                {
                    g_Instance->LinuxOpen(Request);
                    DokanFileInfo->Context = FileId;
                }
                catch (...)
                {
                    Status = ::GetNtStatusAndLogToConsole("ZwCreateFile.Open");
                    if (STATUS_MEDIA_WRITE_PROTECTED == Status ||
                        STATUS_ACCESS_DENIED == Status)
                    {
                        std::printf(
                            "[INFO] Retrying to open File %u as read-only.\n",
                            FileId);
                        Status = STATUS_SUCCESS;
                        Request.Flags &= ~MileCirnoLinuxOpenCreateFlagWriteOnly;
                        Request.Flags &= ~MileCirnoLinuxOpenCreateFlagReadWrite;
                        Request.Flags |= MileCirnoLinuxOpenCreateFlagReadOnly;
                        g_Instance->LinuxOpen(Request);
                        DokanFileInfo->Context = FileId;
                    }
                }
            }
        }
        catch (...)
        {
            Status = ::GetNtStatusAndLogToConsole("ZwCreateFile.Open");
        }
    }

    if (STATUS_SUCCESS != Status)
    {
        ::MileCirnoSimpleClunk(FileId);
    }

    return Status;
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
        ::MileCirnoSimpleClunk(FileId);
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

        Buffer->dwFileAttributes = ::ToFileAttributes(
            Response.Mode);

        Buffer->ftLastAccessTime = ::ToFileTime(
            Response.LastAccessTimeSeconds,
            Response.LastAccessTimeNanoseconds);
        Buffer->ftLastWriteTime = ::ToFileTime(
            Response.LastWriteTimeSeconds,
            Response.LastWriteTimeNanoseconds);

        // Assume creation time is the same as last write time.
        Buffer->ftCreationTime = Buffer->ftLastWriteTime;

        Buffer->dwVolumeSerialNumber = g_VolumeSerialNumber;

        Buffer->nFileSizeHigh =
            static_cast<DWORD>(Response.FileSize >> 32);
        Buffer->nFileSizeLow =
            static_cast<DWORD>(Response.FileSize);

        Buffer->nNumberOfLinks =
            static_cast<DWORD>(Response.NumberOfHardLinks);

        Buffer->nFileIndexHigh =
            static_cast<DWORD>(Response.UniqueId.Path >> 32);
        Buffer->nFileIndexLow =
            static_cast<DWORD>(Response.UniqueId.Path);
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
                ::wcscpy_s(
                    FindData.cFileName,
                    Mile::ToWideString(CP_UTF8, Entry.Name).c_str());

                std::uint32_t CurrentFileId = MILE_CIRNO_NOFID;
                auto CurrentCleanupHandler = Mile::ScopeExitTaskHandler([&]()
                {
                    try
                    {
                        if (MILE_CIRNO_NOFID != CurrentFileId)
                        {
                            ::MileCirnoSimpleClunk(CurrentFileId);
                        }
                    }
                    catch (...)
                    {
                        ::GetNtStatusAndLogToConsole("FindFiles");
                    }
                });
                if (STATUS_SUCCESS != ::MileCirnoSimpleWalk(
                    CurrentFileId,
                    FileId,
                    Mile::ToWideString(CP_UTF8, Entry.Name)))
                {
                    continue;
                }

                try
                {
                    Mile::Cirno::GetAttributesRequest InformationRequest;
                    InformationRequest.FileId = CurrentFileId;
                    InformationRequest.RequestMask =
                        MileCirnoLinuxGetAttributesFlagMode |
                        MileCirnoLinuxGetAttributesFlagLastAccessTime |
                        MileCirnoLinuxGetAttributesFlagLastWriteTime |
                        MileCirnoLinuxGetAttributesFlagSize;
                    Mile::Cirno::GetAttributesResponse InformationResponse =
                        g_Instance->GetAttributes(InformationRequest);

                    FindData.dwFileAttributes = ::ToFileAttributes(
                        InformationResponse.Mode);

                    FindData.ftLastAccessTime = ::ToFileTime(
                        InformationResponse.LastAccessTimeSeconds,
                        InformationResponse.LastAccessTimeNanoseconds);
                    FindData.ftLastWriteTime = ::ToFileTime(
                        InformationResponse.LastWriteTimeSeconds,
                        InformationResponse.LastWriteTimeNanoseconds);

                    // Assume creation time is the same as last write time.
                    FindData.ftCreationTime = FindData.ftLastWriteTime;

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
        if (!(FILE_ATTRIBUTE_READONLY & FileAttributes))
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

    NTSTATUS Status = STATUS_SUCCESS;

    std::uint32_t OldDirectoryFileId = MILE_CIRNO_NOFID;
    std::uint32_t NewDirectoryFileId = MILE_CIRNO_NOFID;
    auto CleanupHandler = Mile::ScopeExitTaskHandler([&]()
    {
        try
        {
            if (MILE_CIRNO_NOFID != NewDirectoryFileId)
            {
                ::MileCirnoSimpleClunk(NewDirectoryFileId);
            }

            if (MILE_CIRNO_NOFID != OldDirectoryFileId)
            {
                ::MileCirnoSimpleClunk(OldDirectoryFileId);
            }
        }
        catch (...)
        {
            ::GetNtStatusAndLogToConsole("MoveFile");
        }
    });

    Status = ::MileCirnoSimpleWalk(
        OldDirectoryFileId,
        g_RootDirectoryFileId,
        OldFilePath.parent_path());
    if (STATUS_SUCCESS != Status)
    {
        return Status;
    }
    Status = ::MileCirnoSimpleWalk(
        NewDirectoryFileId,
        g_RootDirectoryFileId,
        NewFilePath.parent_path());
    if (STATUS_SUCCESS != Status)
    {
        return Status;
    }

    try
    {
        Mile::Cirno::RenameAtRequest Request;
        Request.OldDirectoryFileId = OldDirectoryFileId;
        Request.OldName = Mile::ToString(
            CP_UTF8,
            OldFilePath.filename().wstring());
        Request.NewDirectoryFileId = NewDirectoryFileId;
        Request.NewName = Mile::ToString(
            CP_UTF8,
            NewFilePath.filename().wstring());
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

NTSTATUS DOKAN_CALLBACK MileCirnoSetAllocationSize(
    _In_ LPCWSTR FileName,
    _In_ LONGLONG AllocSize,
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
        Request.FileSize = AllocSize;
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

NTSTATUS DOKAN_CALLBACK MileCirnoGetVolumeInformation(
    _Out_opt_ LPWSTR VolumeNameBuffer,
    _In_ DWORD VolumeNameSize,
    _Out_opt_ LPDWORD VolumeSerialNumber,
    _Out_opt_ LPDWORD MaximumComponentLength,
    _Out_opt_ LPDWORD FileSystemFlags,
    _Out_opt_ LPWSTR FileSystemNameBuffer,
    _In_ DWORD FileSystemNameSize,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    if (VolumeNameBuffer)
    {
        std::wstring ConvertedAccessName = Mile::ToWideString(
            CP_UTF8,
            g_AccessName);
        ::wcsncpy_s(
            VolumeNameBuffer,
            VolumeNameSize,
            ConvertedAccessName.c_str(),
            _TRUNCATE);
    }

    if (VolumeSerialNumber)
    {
        *VolumeSerialNumber = g_VolumeSerialNumber;
    }

    if (MaximumComponentLength)
    {
        *MaximumComponentLength = 255;
    }

    if (FileSystemFlags)
    {
        *FileSystemFlags =
            FILE_CASE_SENSITIVE_SEARCH |
            FILE_CASE_PRESERVED_NAMES |
            FILE_SUPPORTS_REMOTE_STORAGE |
            FILE_UNICODE_ON_DISK;
    }

    if (FileSystemNameBuffer)
    {
        // We should return the file system name as "NTFS" for compatibility.
        ::wcsncpy_s(
            FileSystemNameBuffer,
            FileSystemNameSize,
            L"NTFS",
            _TRUNCATE);
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
                ::MileCirnoSimpleClunk(g_RootDirectoryFileId);
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

        ::MileCirnoSimpleAttach(
            g_RootDirectoryFileId,
            MILE_CIRNO_NOFID,
            "",
            AccessName,
            MILE_CIRNO_NONUNAME);
        if (MILE_CIRNO_NOFID == g_RootDirectoryFileId)
        {
            std::printf("[ERROR] Attach to %s failed.\n", AccessName.c_str());
            return -1;
        }

        g_AccessName = AccessName;

        g_VolumeSerialNumber = ::CalculateFnv1aHash(Mile::FormatString(
            "Mile.Cirno://%s:%s/%s",
            Host.c_str(),
            Port.c_str(),
            AccessName.c_str()));
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
    Operations.SetAllocationSize = ::MileCirnoSetAllocationSize;
    Operations.LockFile;
    Operations.UnlockFile;
    Operations.GetDiskFreeSpaceW = ::MileCirnoGetDiskFreeSpace;
    Operations.GetVolumeInformationW = ::MileCirnoGetVolumeInformation;
    Operations.Mounted;
    Operations.Unmounted;
    Operations.GetFileSecurityW;
    Operations.SetFileSecurityW;
    Operations.FindStreams;
    int DokanStatus = ::DokanMain(&Options, &Operations);
    switch (DokanStatus)
    {
    case DOKAN_SUCCESS:
        std::printf(
            "[INFO] The operation completed successfully.\n");
        break;
    case DOKAN_ERROR:
        std::printf(
            "[ERROR] Failed to mount %s to %s.\n",
            AccessName.c_str(),
            MountPoint.c_str());
        break;
    case DOKAN_DRIVE_LETTER_ERROR:
        std::printf(
            "[ERROR] The drive letter is not available.\n");
        break;
    case DOKAN_DRIVER_INSTALL_ERROR:
        std::printf(
            "[ERROR] Unable to install Dokany driver.\n");
        break;
    case DOKAN_START_ERROR:
        std::printf(
            "[ERROR] Unable to connect to Dokany driver.\n");
        break;
    case DOKAN_MOUNT_ERROR:
        std::printf(
            "[ERROR] Unable to assign a drive letter or mount point.\n");
        break;
    case DOKAN_MOUNT_POINT_ERROR:
        std::printf(
            "[ERROR] The mount point is invalid.\n");
        break;
    case DOKAN_VERSION_ERROR:
        std::printf(
            "[ERROR] The version of Dokany driver is not supported.\n");
        break;
    default:
        std::printf(
            "[ERROR] Unknown Dokany error: %d\n",
            DokanStatus);
        break;
    }
    // Use ExitProcess to ensure exit the process successfully when failed.
    if (DOKAN_SUCCESS != DokanStatus)
    {
        ::ExitProcess(static_cast<UINT>(DokanStatus));
    }
    return DokanStatus;
}
