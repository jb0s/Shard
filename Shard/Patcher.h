#pragma once

#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"
#include <iostream>  
#include <MinHook.h>
#include "Memory.h"
#include "Logger.h"

namespace Patcher {



    // PATCHES OUT TERMINATE PROCESS
    typedef void* (__fastcall* fExitBypass)(
        __int64 arg1
        );
    fExitBypass ExitBypass;

    void* __fastcall ExitBypassHook(__int64 a1)
    {
        /*
                xor     ecx, ecx
                mov     [rsp+48h+var_28], rax
                call    75C7C7B8
        */
        return NULL;
    }




    // PATCHES OUT UNREAL CRASH
    typedef void* (__fastcall* fCrashBypass)(
        __int64 arg1,
        __int64 arg2
        );
    fCrashBypass CrashBypass;

    void* __fastcall CrashHook(__int64 a1, __int64 a2)
    {
        return NULL;
    }




    // PATCHES REQUEST EXIT
    typedef void* (__fastcall* fRequestExit)(
        unsigned __int8 arg1,
        unsigned __int8 arg2
        );
    fRequestExit RequestExitBypass;

    void* __fastcall RequestExitBypassHook(unsigned __int8 a1, unsigned __int8 a2)
    {
        return NULL;
    }





    // PATCHES Unreal Notification
    typedef void* (__fastcall* fNotiBypass)(
        wchar_t** arg1,
        unsigned __int8 arg2,
        __int64 arg3,
        char arg4
        );
    fNotiBypass NotiBypass;

    void* __fastcall NotificationHook(wchar_t** a1, unsigned __int8 a2, __int64 a3, char a4)
    {
        return NULL;
    }


	static void patch() {


        Shard::Logger::Log("Applying Patches");
        /// ONLY ENABLE NOTI HOOK IF U DONT WANNA GO INGAME LOL
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