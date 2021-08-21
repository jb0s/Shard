#pragma once

#include <iostream>  
#include <MinHook.h>

#include "Logger.h"
#include "Memory.h"
#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"

namespace Fixes 
{
    typedef void* (__fastcall* fExitBypass)(__int64 arg1);
    typedef void* (__fastcall* fCrashBypass)(__int64 arg1, __int64 arg2);
    typedef void* (__fastcall* fRequestExit)(unsigned __int8 arg1, unsigned __int8 arg2);
    typedef void* (__fastcall* fNotiBypass)(wchar_t** arg1, unsigned __int8 arg2, __int64 arg3, char arg4);
    fExitBypass ExitBypass;
    fCrashBypass CrashBypass;
    fRequestExit RequestExitBypass;
    fNotiBypass NotiBypass;

    // Patches Terminate Process
    void* __fastcall ExitBypassHook(__int64 a1)
    {
        return NULL;
    }

    // Patches UnrealCrash
    void* __fastcall CrashHook(__int64 a1, __int64 a2)
    {
        return NULL;
    }

    // Patches RequestExit
    void* __fastcall RequestExitBypassHook(unsigned __int8 a1, unsigned __int8 a2)
    {
        return NULL;
    }

    // Patches Unreal Notification
    void* __fastcall NotificationHook(wchar_t** a1, unsigned __int8 a2, __int64 a3, char a4)
    {
        return NULL;
    }

	static void Initialize() 
    {
        Shard::Logger::Log("Initializing fixes...");

        auto ExitAddress = Shard::Memory::FindPattern(ExitSig);
        auto NotificationHookAddress = Shard::Memory::FindPattern(Notification);
        auto FNCrashHookAddress = Shard::Memory::FindPattern(FNCrashHook);
        auto RequestExitAddress = Shard::Memory::FindPattern(RequestExit);

        MH_CreateHook(static_cast<LPVOID>((LPVOID)RequestExitAddress), RequestExitBypassHook, reinterpret_cast<LPVOID*>(&RequestExitBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)RequestExitAddress));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)FNCrashHookAddress), CrashHook, reinterpret_cast<LPVOID*>(&CrashBypass));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&NotiBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)ExitAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&ExitBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)ExitAddress));
	}
}