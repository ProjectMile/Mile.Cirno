/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.cpp
 * PURPOSE:   Implementation for Mile.Cirno Core API
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#include "Mile.Cirno.Core.h"

namespace Mile::Cirno
{
    std::vector<uint8_t> FromUInt8(uint8_t const& Value)
    {
        std::vector<uint8_t> vReturn = { Value };
        return vReturn;
    }

    std::vector<uint8_t> FromUInt16(uint16_t const& Value)
    {
        uint8_t u1 = (Value & 0xff);
        uint8_t u2 = (Value >> 8);
        std::vector<uint8_t> vReturn = { u1, u2 };
        return vReturn;
    }

    std::vector<uint8_t> FromUInt32(uint32_t const& Value)
    {
        uint8_t u1 = (Value & 0xff);
        uint8_t u2 = ((Value & 0xff00) >> 8);
        uint8_t u3 = ((Value & 0xff0000) >> 16);
        uint8_t u4 = ((Value & 0xff000000) >> 24);
        std::vector<uint8_t> vReturn = { u1, u2, u3, u4 };
        return vReturn;
    }

    std::vector<uint8_t> FromUInt64(uint64_t const& Value)
    {
        uint8_t u1 = (Value & 0xff);
        uint8_t u2 = ((Value & 0xff00) >> 8);
        uint8_t u3 = ((Value & 0xff0000) >> 16);
        uint8_t u4 = ((Value & 0xff000000) >> 24);
        uint8_t u5 = ((Value & 0xff00000000) >> 32);
        uint8_t u6 = ((Value & 0xff0000000000) >> 40);
        uint8_t u7 = ((Value & 0xff000000000000) >> 48);
        uint8_t u8 = ((Value & 0xff00000000000000) >> 56);
        std::vector<uint8_t> vReturn = { u1, u2, u3, u4, u5, u6, u7, u8 };
        return vReturn;
    }

    std::vector<uint8_t> FromHeader(
        Header const& Value)
    {
        std::vector<uint8_t> vReturn;
        std::vector<uint8_t> Temp;

        Temp = FromUInt32(Value.Size);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt8(Value.Type);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt16(Value.Tag);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        return vReturn;
    }

    std::vector<uint8_t> FromString(
        std::string const& Value)
    {
        std::vector<uint8_t> vReturn = { Value.begin(), Value.end() };
        return vReturn;
    }

    std::vector<uint8_t> FromQid(
        Qid const& Value)
    {
        std::vector<uint8_t> vReturn;
        std::vector<uint8_t> Temp;

        Temp = FromUInt8(Value.Type);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.Version);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt64(Value.Path);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        return vReturn;
    }

    std::vector<uint8_t> FromDirectoryEntry(
        DirectoryEntry const& Value)
    {
        std::vector<uint8_t> vReturn;
        std::vector<uint8_t> Temp;

        Temp = FromQid(Value.UniqueId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt64(Value.Offset);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt8(Value.Type);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.Name);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());

        return vReturn;
    }

    std::vector<uint8_t> FromStat(
        Stat const& Value)
    {
        std::vector<uint8_t> vReturn;
        std::vector<uint8_t> Temp;

        Temp = FromUInt16(Value.Type);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.Dev);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromQid(Value.UniqueId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.Mode);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.LastAccessTime);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.LastWriteTime);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt64(Value.FileSize);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.FileName);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.OwnerUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.GroupId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.LastWriteUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());

        return vReturn;
    }

    std::vector<uint8_t> FromUnixStat(
        UnixStat const& Value)
    {
        std::vector<uint8_t> vReturn;
        std::vector<uint8_t> Temp;

        Temp = FromUInt16(Value.Type);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.Dev);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromQid(Value.UniqueId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.Mode);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.LastAccessTime);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.LastWriteTime);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt64(Value.FileSize);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.FileName);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.OwnerUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.GroupId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.LastWriteUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromString(Value.UnixExtension);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.NumericOwnerUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.NumericGroupId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());
        Temp = FromUInt32(Value.NumericLastWriteUserId);
        vReturn.insert(vReturn.end(), Temp.begin(), Temp.end());

        return vReturn;
    }

}
