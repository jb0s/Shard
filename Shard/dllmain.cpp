// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Shard.h"
#include <MinHook.h>

BOOL APIENTRY DllMain(HMODULE hModule,  DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        auto shard = new Shard::Shard;
        shard->Initialize();
    }
    return TRUE;
}

