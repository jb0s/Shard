#pragma once

#include <iostream>  
#include <MinHook.h>

#include "Logger.h"
#include "Memory.h"
#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"
#include "detours.h"

namespace Fixes 
{
    //__int64 __fastcall sub_7FF7F46632C0(__int64 a1, int a2)
    typedef __int64(__fastcall* fAthenaCrashFunc)(__int64 a1, int a2);
    typedef char(__fastcall* fAthenaCrash2Func)(__int64 a1);
    typedef void* (__fastcall* fExitBypass)(__int64 arg1);
    typedef __int64(__fastcall* fRequestExitForGamemode)(__int64 a1);
    typedef void* (__fastcall* fCrashBypass)(__int64 arg1, __int64 arg2);
    typedef void* (__fastcall* fRequestExit)(unsigned __int8 arg1, unsigned __int8 arg2);
    typedef void* (__fastcall* fNotiBypass)(wchar_t** arg1, unsigned __int8 arg2, __int64 arg3, char arg4);
    fExitBypass ExitBypass;
    fCrashBypass CrashBypass;
    fRequestExit RequestExitBypass;
    fRequestExit RequestExitBypass2;
    fNotiBypass NotiBypass;
    fAthenaCrashFunc AthenaCrashFunc;
    fAthenaCrash2Func AthenaCrash2Func;
    fRequestExitForGamemode RequestExitForGamemode;

    __int64 __fastcall RequestExitForGamemodeHook(__int64 a1)
    {
        return NULL;
    }

    __int64 __fastcall CrashFuncHook(__int64 a1, int a2)
    {
        return NULL;
    }

    char __fastcall CrashFunc2Hook(__int64 a1)
    {
        return NULL;
    }

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
    void* __fastcall RequestExitBypassHook2(unsigned __int8 a1, unsigned __int8 a2)
    {
        return NULL;
    }

    void* __fastcall RequestExitBypassHook(unsigned __int8 a1, unsigned __int8 a2)
    {
        return NULL;
    }

    // Patches Unreal Notification
    void* __fastcall NotificationHook(wchar_t** a1, unsigned __int8 a2, __int64 a3, char a4)
    {
        return NULL;
    }

    void stub(void* a1, void* a2)
    {

    }

	static void Initialize() 
    {

        Shard::Logger::Log("Initializing fixes...");


        auto ExitAddress = Shard::Memory::FindPattern(ExitSig);
        auto NotificationHookAddress = Shard::Memory::FindPattern(Notification);
        auto FNCrashHookAddress = Shard::Memory::FindPattern(FNCrashHook);
        auto RequestExitAddress = Shard::Memory::FindPattern(RequestExit);
        auto RequestExitGameModeAddress = Shard::Memory::FindPattern(REQUEST_EXIT_GAMEMODE);
        auto CrashPatchAddress = Shard::Memory::FindPattern(ATHENA_GAMEMODE_CRASH);
        AthenaCrash2Func = (fAthenaCrash2Func)Shard::Memory::FindPattern(ATHENA_GAMEMODE_CRASH2);

        //Crash Fix //not actually
        //auto base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
        //RequestExitBypass2 = (fRequestExit)(base_address + 0x14DC936);
        //
        //auto unknownfunc = base_address + 0x5590140;
        //auto unknownfunc2 = base_address + 0x10EEC3A; //.text:00007FF676D4EC3C fortniteclient-win64-shipping.exe:$10EEC3C #10EE23C
        

        if (!ExitAddress) {
            Shard::Logger::Log("ExitAddress Signuature Is Invalid");
        }
        if (!NotificationHookAddress) {
            Shard::Logger::Log("NotificationHookAddress Signuature Is Invalid");
        }
        if (!FNCrashHookAddress) {
            Shard::Logger::Log("FNCrashHookAddress Signuature Is Invalid");
        }
        if (!RequestExitAddress) {
            Shard::Logger::Log("RequestExitAddress Signuature Is Invalid");
        }//
        if (!CrashPatchAddress) {
            Shard::Logger::Log("CrashPatchAddress Signuature Is Invalid");
        }//
        if (!RequestExitGameModeAddress) {
            Shard::Logger::Log("CrashPatchAddress Signuature Is Invalid");
        }//
        AthenaCrashFunc = reinterpret_cast<fAthenaCrashFunc>(CrashPatchAddress + 5 + *reinterpret_cast<int32_t*>(CrashPatchAddress + 1));
        RequestExitForGamemode = reinterpret_cast<fRequestExitForGamemode>(RequestExitGameModeAddress + 5 + *reinterpret_cast<int32_t*>(RequestExitGameModeAddress + 1));


        MH_CreateHook(static_cast<LPVOID>((LPVOID)RequestExitAddress), RequestExitBypassHook, reinterpret_cast<LPVOID*>(&RequestExitBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)RequestExitAddress));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)FNCrashHookAddress), CrashHook, reinterpret_cast<LPVOID*>(&CrashBypass));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&NotiBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)NotificationHookAddress));
        MH_CreateHook(static_cast<LPVOID>((LPVOID)ExitAddress), ExitBypassHook, reinterpret_cast<LPVOID*>(&ExitBypass));
        MH_EnableHook(static_cast<LPVOID>((LPVOID)ExitAddress));
        DetourTransactionBegin(); DetourAttach(&(PVOID&)AthenaCrashFunc, CrashFuncHook); DetourTransactionCommit();
        DetourTransactionBegin(); DetourAttach(&(PVOID&)AthenaCrash2Func, CrashFunc2Hook); DetourTransactionCommit();
        DetourTransactionBegin(); DetourAttach(&(PVOID&)RequestExitForGamemode, RequestExitForGamemodeHook); DetourTransactionCommit();
        //DetourTransactionBegin(); DetourAttach(&(PVOID&)RequestExitBypass2, RequestExitBypassHook2); DetourTransactionCommit();
        //DetourTransactionBegin(); DetourAttach(&(PVOID&)unknownfunc, stub); DetourTransactionCommit();
        //DetourTransactionBegin(); DetourAttach(&(PVOID&)unknownfunc2, stub); DetourTransactionCommit();
	}
}