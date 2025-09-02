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

std::uint32_t SimpleClunk(
    std::uint32_t const& FileId)
{
    Mile::Cirno::ClunkRequest Request = {};
    Request.FileId = FileId;
    std::uint32_t ErrorCode = g_Instance->Clunk(Request);
    if (0 == ErrorCode)
    {
        g_Instance->FreeFileId(FileId);
    }
    return ErrorCode;
}

std::uint32_t SimpleAttach(
    std::uint32_t& OutputFileId,
    std::uint32_t const& AuthenticationFileId,
    std::string const& UserName,
    std::string const& AccessName,
    std::uint32_t const& NumericUserName)
{
    OutputFileId = MILE_CIRNO_NOFID;
    Mile::Cirno::AttachRequest Request = {};
    Request.FileId = g_Instance->AllocateFileId();
    Request.AuthenticationFileId = AuthenticationFileId;
    Request.UserName = UserName;
    Request.AccessName = AccessName;
    Request.NumericUserName = NumericUserName;
    Mile::Cirno::AttachResponse Response = {};
    std::uint32_t ErrorCode = g_Instance->Attach(Request, Response);
    if (0 == ErrorCode)
    {
        OutputFileId = Request.FileId;
    }
    else
    {
        // Only unregister the file ID because the file ID is not used by the
        // server if failed to attach.
        g_Instance->FreeFileId(Request.FileId);
    }

    return ErrorCode;
}

std::uint32_t SimpleWalk(
    std::uint32_t& OutputFileId,
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath)
{
    OutputFileId = MILE_CIRNO_NOFID;
    std::uint32_t ErrorCode = 0;
    Mile::Cirno::WalkRequest WalkRequest = {};
    WalkRequest.FileId = RootDirectoryFileId;
    WalkRequest.NewFileId = g_Instance->AllocateFileId();
    try
    {
        for (std::filesystem::path const& Element : RelativeFilePath)
        {
            WalkRequest.Names.push_back(Mile::ToString(
                CP_UTF8,
                Element.wstring()));
        }
    }
    catch (...)
    {
        ErrorCode = APTX_EINVAL;
    }
    if (0 == ErrorCode)
    {
        Mile::Cirno::WalkResponse WalkResponse = {};
        ErrorCode = g_Instance->Walk(WalkRequest, WalkResponse);
    }

    if (0 == ErrorCode)
    {
        OutputFileId = WalkRequest.NewFileId;
    }
    else
    {
        // Only unregister the file ID because the file ID is not used by the
        // server if failed to walk.
        g_Instance->FreeFileId(WalkRequest.NewFileId);
    }
    return ErrorCode;
}

std::uint32_t SimpleMakeDirectory(
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath)
{
    std::uint32_t ErrorCode = 0;

    std::uint32_t RelativeRootDirectoryFileId = MILE_CIRNO_NOFID;
    ErrorCode = ::SimpleWalk(
        RelativeRootDirectoryFileId,
        RootDirectoryFileId,
        RelativeFilePath.parent_path());
    if (0 == ErrorCode)
    {
        std::uint32_t RootDirectoryGroupId = 0;
        {
            Mile::Cirno::GetAttributesRequest Request = {};
            Request.FileId = RelativeRootDirectoryFileId;
            Request.RequestMask =
                MileCirnoLinuxGetAttributesFlagGroupId;
            Mile::Cirno::GetAttributesResponse Response = {};
            ErrorCode = g_Instance->GetAttributes(Request, Response);
            if (0 == ErrorCode)
            {
                RootDirectoryGroupId = Response.GroupId;
            }
        }
        if (0 == ErrorCode)
        {
            Mile::Cirno::MakeDirectoryRequest Request = {};
            Request.DirectoryFileId = RelativeRootDirectoryFileId;
            Request.Name = Mile::ToString(
                CP_UTF8,
                RelativeFilePath.filename().wstring());
            Request.Mode = APTX_IRWXU;
            Request.Mode |= APTX_IRGRP | APTX_IXGRP;
            Request.Mode |= APTX_IROTH | APTX_IXOTH;
            Request.GroupId = RootDirectoryGroupId;
            Mile::Cirno::MakeDirectoryResponse Response = {};
            ErrorCode = g_Instance->MakeDirectory(Request, Response);
        }

        ::SimpleClunk(RelativeRootDirectoryFileId);
    }

    return ErrorCode;
}

std::uint32_t SimpleLinuxCreate(
    std::uint32_t const& RootDirectoryFileId,
    std::filesystem::path const& RelativeFilePath,
    std::uint32_t Flags,
    std::uint32_t Mode)
{
    std::uint32_t ErrorCode = 0;

    std::uint32_t RelativeRootDirectoryFileId = MILE_CIRNO_NOFID;
    ErrorCode = ::SimpleWalk(
        RelativeRootDirectoryFileId,
        RootDirectoryFileId,
        RelativeFilePath.parent_path());
    if (0 == ErrorCode)
    {
        std::uint32_t RootDirectoryGroupId = 0;
        {
            Mile::Cirno::GetAttributesRequest Request = {};
            Request.FileId = RelativeRootDirectoryFileId;
            Request.RequestMask =
                MileCirnoLinuxGetAttributesFlagGroupId;
            Mile::Cirno::GetAttributesResponse Response = {};
            ErrorCode = g_Instance->GetAttributes(Request, Response);
            if (0 == ErrorCode)
            {
                RootDirectoryGroupId = Response.GroupId;
            }
        }
        if (0 == ErrorCode)
        {
            Mile::Cirno::LinuxCreateRequest Request = {};
            Request.FileId = RelativeRootDirectoryFileId;
            Request.Name = Mile::ToString(
                CP_UTF8,
                RelativeFilePath.filename().wstring());
            Request.Flags = Flags;
            Request.Mode = Mode;
            Request.GroupId = RootDirectoryGroupId;
            Mile::Cirno::LinuxCreateResponse Response = {};
            ErrorCode = g_Instance->LinuxCreate(Request, Response);
        }

        ::SimpleClunk(RelativeRootDirectoryFileId);
    }

    return ErrorCode;
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

    if (0 == ::_wcsicmp(FileName, L"\\System Volume Information") ||
        0 == ::_wcsicmp(FileName, L"\\$RECYCLE.BIN"))
    {
        return STATUS_NO_SUCH_FILE;
    }

    std::uint32_t ErrorCode = 0;

    std::filesystem::path RelativeFilePath(&FileName[1]);

    if (FILE_DIRECTORY_FILE == (FILE_DIRECTORY_FILE & CreateOptions))
    {
        if (FILE_CREATE == CreateDisposition ||
            FILE_OPEN_IF == CreateDisposition)
        {
            ErrorCode = ::SimpleMakeDirectory(
                g_RootDirectoryFileId,
                RelativeFilePath);
            if (0 != ErrorCode)
            {
                return ::ToNtStatus(ErrorCode);
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

    NTSTATUS Status = STATUS_SUCCESS;

    std::uint32_t FileId = MILE_CIRNO_NOFID;
    ErrorCode = ::SimpleWalk(
        FileId,
        g_RootDirectoryFileId,
        RelativeFilePath);
    if (0 != ErrorCode)
    {
        Status = ::ToNtStatus(ErrorCode);
    }
    if (STATUS_SUCCESS != Status)
    {
        Status = STATUS_SUCCESS;

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

        ErrorCode = ::SimpleLinuxCreate(
            g_RootDirectoryFileId,
            RelativeFilePath,
            ConvertedFlags | MileCirnoLinuxOpenCreateFlagCreate,
            ConvertedFileMode);
        if (0 != ErrorCode)
        {
            return ::ToNtStatus(ErrorCode);
        }
        CreateDisposition = FILE_OPEN;

        ErrorCode = ::SimpleWalk(
            FileId,
            g_RootDirectoryFileId,
            RelativeFilePath);
        if (0 != ErrorCode)
        {
            return ::ToNtStatus(ErrorCode);
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

        Mile::Cirno::LinuxOpenRequest Request = {};
        Request.FileId = FileId;
        Request.Flags = ConvertedFlags;
        Mile::Cirno::LinuxOpenResponse Response = {};
        ErrorCode = g_Instance->LinuxOpen(Request, Response);
        if (APTX_EROFS == ErrorCode || APTX_EACCES == ErrorCode)
        {
            Request.Flags &= ~MileCirnoLinuxOpenCreateFlagWriteOnly;
            Request.Flags &= ~MileCirnoLinuxOpenCreateFlagReadWrite;
            Request.Flags |= MileCirnoLinuxOpenCreateFlagReadOnly;
            ErrorCode = g_Instance->LinuxOpen(Request, Response);
        }
        if (0 != ErrorCode)
        {
            Status = ::ToNtStatus(ErrorCode);
        }

        if (STATUS_SUCCESS == Status)
        {
            if (MileCirnoQidTypeDirectory == Response.UniqueId.Type)
            {
                DokanFileInfo->IsDirectory = TRUE;
                if (FILE_NON_DIRECTORY_FILE & CreateOptions)
                {
                    Status = STATUS_OBJECT_NAME_NOT_FOUND;
                }
            }
        }

        if (STATUS_SUCCESS == Status)
        {
            DokanFileInfo->Context = FileId;
        }
    }

    if (STATUS_SUCCESS != Status)
    {
        ::SimpleClunk(FileId);
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
        Mile::Cirno::RemoveRequest Request = {};
        Request.FileId = FileId;
        g_Instance->Remove(Request);
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

    ::SimpleClunk(FileId);
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

    NTSTATUS Status = STATUS_SUCCESS;

    DWORD ProceededSize = 0;
    DWORD UnproceededSize = BufferLength;

    while (UnproceededSize)
    {
        Mile::Cirno::ReadRequest Request = {};
        Request.FileId = FileId;
        Request.Offset = Offset + ProceededSize;
        Request.Count = g_MaximumMessageSize;
        Request.Count -= Mile::Cirno::ReadResponseHeaderSize;
        if (UnproceededSize < Request.Count)
        {
            Request.Count = UnproceededSize;
        }
        Mile::Cirno::ReadResponse Response = {};
        std::uint32_t ErrorCode = g_Instance->Read(Request, Response);
        if (0 != ErrorCode)
        {
            Status = ::ToNtStatus(ErrorCode);
            break;
        }
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

    if (ReadLength)
    {
        *ReadLength = ProceededSize;
    }

    return Status;
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

    NTSTATUS Status = STATUS_SUCCESS;

    if (DokanFileInfo->WriteToEndOfFile || -1 == Offset)
    {
        Mile::Cirno::GetAttributesRequest Request = {};
        Request.FileId = FileId;
        Request.RequestMask = MileCirnoLinuxGetAttributesFlagSize;
        Mile::Cirno::GetAttributesResponse Response = {};
        std::uint32_t ErrorCode = g_Instance->GetAttributes(Request, Response);
        if (0 != ErrorCode)
        {
            Status = ::ToNtStatus(ErrorCode);
        }
        else
        {
            Offset = Response.FileSize;
        }
    }

    if (STATUS_SUCCESS == Status)
    {
        while (UnproceededSize)
        {
            Mile::Cirno::WriteRequest Request = {};
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
            Mile::Cirno::WriteResponse Response = {};
            std::uint32_t ErrorCode = g_Instance->Write(Request, Response);
            if (0 != ErrorCode)
            {
                Status = ::ToNtStatus(ErrorCode);
                break;
            }
            DWORD CurrentProceededSize = static_cast<DWORD>(Response.Count);
            if (!CurrentProceededSize)
            {
                break;
            }
            ProceededSize += CurrentProceededSize;
            UnproceededSize -= CurrentProceededSize;
        }
    }

    if (NumberOfBytesWritten)
    {
        *NumberOfBytesWritten = ProceededSize;
    }

    return Status;
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

    Mile::Cirno::FlushFileRequest Request = {};
    Request.FileId = FileId;
    return ::ToNtStatus(g_Instance->FlushFile(Request));
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

    Mile::Cirno::GetAttributesRequest Request = {};
    Request.FileId = FileId;
    Request.RequestMask =
        MileCirnoLinuxGetAttributesFlagMode |
        MileCirnoLinuxGetAttributesFlagNumberOfHardLinks |
        MileCirnoLinuxGetAttributesFlagLastAccessTime |
        MileCirnoLinuxGetAttributesFlagLastWriteTime |
        MileCirnoLinuxGetAttributesFlagSize;
    Mile::Cirno::GetAttributesResponse Response = {};
    std::uint32_t ErrorCode = g_Instance->GetAttributes(Request, Response);
    if (0 != ErrorCode)
    {
        return ::ToNtStatus(ErrorCode);
    }

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

    NTSTATUS Status = STATUS_SUCCESS;

    std::uint64_t LastOffset = 0;
    do
    {
        Mile::Cirno::ReadDirectoryRequest Request = {};
        Request.FileId = FileId;
        Request.Offset = LastOffset;
        LastOffset = 0;
        Request.Count = g_MaximumMessageSize;
        Request.Count -= Mile::Cirno::ReadDirectoryResponseHeaderSize;
        Mile::Cirno::ReadDirectoryResponse Response = {};
        {
            std::uint32_t ErrorCode = g_Instance->ReadDirectory(
                Request,
                Response);
            if (0 != ErrorCode)
            {
                Status = ::ToNtStatus(ErrorCode);
                break;
            }
        }
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
                if (MILE_CIRNO_NOFID != CurrentFileId)
                {
                    ::SimpleClunk(CurrentFileId);
                }
            });
            if (0 != ::SimpleWalk(
                CurrentFileId,
                FileId,
                Mile::ToWideString(CP_UTF8, Entry.Name)))
            {
                continue;
            }

            Mile::Cirno::GetAttributesRequest InformationRequest = {};
            InformationRequest.FileId = CurrentFileId;
            InformationRequest.RequestMask =
                MileCirnoLinuxGetAttributesFlagMode |
                MileCirnoLinuxGetAttributesFlagLastAccessTime |
                MileCirnoLinuxGetAttributesFlagLastWriteTime |
                MileCirnoLinuxGetAttributesFlagSize;
            Mile::Cirno::GetAttributesResponse InformationResponse = {};
            std::uint32_t ErrorCode = g_Instance->GetAttributes(
                InformationRequest,
                InformationResponse);
            if (0 != ErrorCode)
            {
                continue;
            }

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

            FillFindData(&FindData, DokanFileInfo);
        }
    } while (LastOffset);

    return Status;
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
    return ::ToNtStatus(g_Instance->SetAttributes(Request));
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
    return ::ToNtStatus(g_Instance->SetAttributes(Request));
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

    Mile::Cirno::ReadDirectoryRequest Request = {};
    Request.FileId = FileId;
    Request.Offset = 0;
    Request.Count = g_MaximumMessageSize;
    Request.Count -= Mile::Cirno::ReadDirectoryResponseHeaderSize;
    Mile::Cirno::ReadDirectoryResponse Response = {};
    std::uint32_t ErrorCode = g_Instance->ReadDirectory(Request, Response);
    if (0 != ErrorCode)
    {
        return ::ToNtStatus(ErrorCode);
    }
    if (Response.Data.size() > 2)
    {
        return STATUS_DIRECTORY_NOT_EMPTY;
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

    std::uint32_t ErrorCode = 0;

    std::uint32_t OldDirectoryFileId = MILE_CIRNO_NOFID;
    ErrorCode = ::SimpleWalk(
        OldDirectoryFileId,
        g_RootDirectoryFileId,
        OldFilePath.parent_path());
    if (0 == ErrorCode)
    {
        std::uint32_t NewDirectoryFileId = MILE_CIRNO_NOFID;
        ErrorCode = ::SimpleWalk(
            NewDirectoryFileId,
            g_RootDirectoryFileId,
            NewFilePath.parent_path());
        if (0 != ErrorCode)
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
            ErrorCode = g_Instance->RenameAt(Request);

            ::SimpleClunk(NewDirectoryFileId);
        }

        ::SimpleClunk(OldDirectoryFileId);
    }

    if (0 != ErrorCode)
    {
        return ::ToNtStatus(ErrorCode);
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

    Mile::Cirno::SetAttributesRequest Request = {};
    Request.FileId = FileId;
    Request.Valid = MileCirnoLinuxSetAttributesFlagSize;
    Request.FileSize = ByteOffset;
    return ::ToNtStatus(g_Instance->SetAttributes(Request));
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

    Mile::Cirno::SetAttributesRequest Request = {};
    Request.FileId = FileId;
    Request.Valid = MileCirnoLinuxSetAttributesFlagSize;
    Request.FileSize = AllocSize;
    return ::ToNtStatus(g_Instance->SetAttributes(Request));
}

NTSTATUS DOKAN_CALLBACK MileCirnoGetDiskFreeSpace(
    _Out_opt_ PULONGLONG FreeBytesAvailable,
    _Out_opt_ PULONGLONG TotalNumberOfBytes,
    _Out_opt_ PULONGLONG TotalNumberOfFreeBytes,
    _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
{
    UNREFERENCED_PARAMETER(DokanFileInfo);

    Mile::Cirno::FileSystemStatusRequest Request = {};
    Request.FileId = g_RootDirectoryFileId;
    Mile::Cirno::FileSystemStatusResponse Response = {};
    std::uint32_t ErrorCode = g_Instance->FileSystemStatus(Request, Response);
    if (0 != ErrorCode)
    {
        return ::ToNtStatus(ErrorCode);
    }
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

    std::printf(
        "[INFO] Host = %s\n"
        "[INFO] Port = %s\n"
        "[INFO] AccessName = %s\n"
        "[INFO] MountPoint = %s\n"
        "\n",
        Host.c_str(),
        Port.c_str(),
        AccessName.c_str(),
        MountPoint.c_str());

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
            std::printf(
                "[ERROR] WSAStartup failed (%d).\n",
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
            Mile::Cirno::VersionResponse Response = {};
            if (0 != g_Instance->Version(Request, Response))
            {
                std::printf("[ERROR] Version negotiation failed.\n");
                return -1;
            }
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

        if (0 != ::SimpleAttach(
            g_RootDirectoryFileId,
            MILE_CIRNO_NOFID,
            "",
            AccessName,
            MILE_CIRNO_NONUNAME))
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
    Operations.ZwCreateFile = [](
        _In_ LPCWSTR FileName,
        _In_ PDOKAN_IO_SECURITY_CONTEXT SecurityContext,
        _In_ ACCESS_MASK DesiredAccess,
        _In_ ULONG FileAttributes,
        _In_ ULONG ShareAccess,
        _In_ ULONG CreateDisposition,
        _In_ ULONG CreateOptions,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoZwCreateFile(
            FileName,
            SecurityContext,
            DesiredAccess,
            FileAttributes,
            ShareAccess,
            CreateDisposition,
            CreateOptions,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"ZwCreateFile(\"%s\") = 0x%08X, "
            L"FileId = %llu, "
            L"ProcessId = %lu\n",
            FileName,
            Status,
            DokanFileInfo->Context,
            DokanFileInfo->ProcessId).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.Cleanup = [](
        _In_ LPCWSTR FileName,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
    {
        ::MileCirnoCleanup(FileName, DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"Cleanup(%llu), "
            L"DeletePending=%d\n",
            DokanFileInfo->Context,
            DokanFileInfo->DeletePending).c_str());
#endif // !NDEBUG
    };
    Operations.CloseFile = [](
        _In_ LPCWSTR FileName,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo)
    {
        ::MileCirnoCloseFile(FileName, DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"CloseFile(%llu)\n",
            DokanFileInfo->Context).c_str());
#endif // !NDEBUG
    };
    Operations.ReadFile = [](
        _In_ LPCWSTR FileName,
        _Out_opt_ LPVOID Buffer,
        _In_ DWORD BufferLength,
        _Out_opt_ LPDWORD ReadLength,
        _In_ LONGLONG Offset,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoReadFile(
            FileName,
            Buffer,
            BufferLength,
            ReadLength,
            Offset,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"ReadFile(%llu, %u, %llu) = 0x%08X, "
            L"Read=%u\n",
            DokanFileInfo->Context,
            BufferLength,
            Offset,
            Status,
            ReadLength ? *ReadLength : 0).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.WriteFile = [](
        _In_ LPCWSTR FileName,
        _In_ LPCVOID Buffer,
        _In_ DWORD NumberOfBytesToWrite,
        _Out_opt_ LPDWORD NumberOfBytesWritten,
        _In_ LONGLONG Offset,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoWriteFile(
            FileName,
            Buffer,
            NumberOfBytesToWrite,
            NumberOfBytesWritten,
            Offset,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"WriteFile(%llu, %u, %llu) = 0x%08X, "
            L"Written=%u\n",
            DokanFileInfo->Context,
            NumberOfBytesToWrite,
            Offset,
            Status,
            NumberOfBytesWritten ? *NumberOfBytesWritten : 0).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.FlushFileBuffers = [](
        _In_ LPCWSTR FileName,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoFlushFileBuffers(
            FileName,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"FlushFileBuffers(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.GetFileInformation = [](
        _In_ LPCWSTR FileName,
        _Out_ LPBY_HANDLE_FILE_INFORMATION Buffer,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoGetFileInformation(
            FileName,
            Buffer,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"GetFileInformation(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.FindFiles = [](
        _In_ LPCWSTR FileName,
        _In_ PFillFindData FillFindData,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoFindFiles(
            FileName,
            FillFindData,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"FindFiles(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.FindFilesWithPattern = nullptr;
    Operations.SetFileAttributesW = [](
        _In_ LPCWSTR FileName,
        _In_ DWORD FileAttributes,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoSetFileAttributes(
            FileName,
            FileAttributes,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"SetFileAttributes(%llu, 0x%08X) = 0x%08X\n",
            DokanFileInfo->Context,
            FileAttributes,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.SetFileTime = [](
        _In_ LPCWSTR FileName,
        _In_ CONST FILETIME* CreationTime,
        _In_ CONST FILETIME* LastAccessTime,
        _In_ CONST FILETIME* LastWriteTime,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoSetFileTime(
            FileName,
            CreationTime,
            LastAccessTime,
            LastWriteTime,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"SetFileTime(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.DeleteFileW = [](
        _In_ LPCWSTR FileName,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoDeleteFile(
            FileName,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"DeleteFile(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.DeleteDirectory = [](
        _In_ LPCWSTR FileName,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoDeleteDirectory(
            FileName,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"DeleteDirectory(%llu) = 0x%08X\n",
            DokanFileInfo->Context,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.MoveFileW = [](
        _In_ LPCWSTR FileName,
        _In_ LPCWSTR NewFileName,
        _In_ BOOL ReplaceIfExisting,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoMoveFile(
            FileName,
            NewFileName,
            ReplaceIfExisting,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"MoveFile(\"%s\", \"%s\", ReplaceIfExisting=%d) = 0x%08X\n",
            FileName,
            NewFileName,
            ReplaceIfExisting,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.SetEndOfFile = [](
        _In_ LPCWSTR FileName,
        _In_ LONGLONG ByteOffset,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoSetEndOfFile(
            FileName,
            ByteOffset,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"SetEndOfFile(%llu, %llu) = 0x%08X\n",
            DokanFileInfo->Context,
            ByteOffset,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.SetAllocationSize = [](
        _In_ LPCWSTR FileName,
        _In_ LONGLONG AllocSize,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoSetAllocationSize(
            FileName,
            AllocSize,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"SetAllocationSize(%llu, %llu) = 0x%08X\n",
            DokanFileInfo->Context,
            AllocSize,
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.LockFile;
    Operations.UnlockFile;
    Operations.GetDiskFreeSpaceW = [](
        _Out_opt_ PULONGLONG FreeBytesAvailable,
        _Out_opt_ PULONGLONG TotalNumberOfBytes,
        _Out_opt_ PULONGLONG TotalNumberOfFreeBytes,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoGetDiskFreeSpace(
            FreeBytesAvailable,
            TotalNumberOfBytes,
            TotalNumberOfFreeBytes,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"GetDiskFreeSpace() = 0x%08X\n",
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
    Operations.GetVolumeInformationW = [](
        _Out_opt_ LPWSTR VolumeNameBuffer,
        _In_ DWORD VolumeNameSize,
        _Out_opt_ LPDWORD VolumeSerialNumber,
        _Out_opt_ LPDWORD MaximumComponentLength,
        _Out_opt_ LPDWORD FileSystemFlags,
        _Out_opt_ LPWSTR FileSystemNameBuffer,
        _In_ DWORD FileSystemNameSize,
        _Inout_ PDOKAN_FILE_INFO DokanFileInfo) -> NTSTATUS
    {
        NTSTATUS Status = ::MileCirnoGetVolumeInformation(
            VolumeNameBuffer,
            VolumeNameSize,
            VolumeSerialNumber,
            MaximumComponentLength,
            FileSystemFlags,
            FileSystemNameBuffer,
            FileSystemNameSize,
            DokanFileInfo);
#ifndef NDEBUG
        ::OutputDebugStringW(Mile::FormatWideString(
            L"[Mile.Cirno] "
            L"GetVolumeInformation() = 0x%08X\n",
            Status).c_str());
#endif // !NDEBUG
        return Status;
    };
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
