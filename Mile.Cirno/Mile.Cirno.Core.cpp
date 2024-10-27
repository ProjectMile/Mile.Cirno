/*
 * PROJECT:   Mouri Internal Library Essentials
 * FILE:      Mile.Cirno.Core.cpp
 * PURPOSE:   Implementation for Mile.Cirno Core Infrastructures
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 *             per1cycle (pericycle.cc@gmail.com)
 */

#include "Mile.Cirno.Core.h"

#include "Mile.Cirno.Protocol.Parser.h"

#include <mutex>
#include <set>

namespace
{
    std::mutex g_TagAllocationMutex;
    std::uint16_t g_UnallocatedTagStart = 0;
    std::set<std::uint16_t> g_ReusableTags;
}

std::uint16_t Mile::Cirno::AllocateTag()
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

void Mile::Cirno::FreeTag(
    std::uint16_t const& Tag)
{
    std::lock_guard<std::mutex> Guard(g_TagAllocationMutex);

    g_ReusableTags.insert(Tag);

    while (!g_ReusableTags.empty()
        && *g_ReusableTags.rbegin() == g_UnallocatedTagStart - 1)
    {
        g_ReusableTags.erase(--g_UnallocatedTagStart);
    }
}
