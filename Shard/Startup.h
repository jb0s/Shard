#pragma once

#include "Logger.h"
#include "Memory.h"
#include "Define.h"
#include "Globals.h"

namespace Shard
{
    class Startup
    {
    public:
        static void Initialize()
        {
            Logger::Log("Finding offsets...");
            auto worldPattern = Memory::FindPattern(UWORLD);
            auto worldAddress = RELATIVE_ADDR(worldPattern, 7);
            auto engineOffset = (uintptr_t)GetModuleHandle(NULL) + ENGINE_OFFSET;

            Logger::Log("Initializing globals...");
            Globals::GEngine = reinterpret_cast<cEngine*>(engineOffset);
            Globals::UWorld = (cUWorld*)(*(uintptr_t*)worldAddress);
            Globals::GetNameByIndex = decltype(Globals::GetNameByIndex)(Memory::FindPattern(GET_NAME_BY_INDEX));
            Globals::Free = decltype(Globals::Free)(Memory::FindPattern(FREE));
        }
    };
}