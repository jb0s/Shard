#include "Shard.h"

#include <stdio.h>
#include <iostream>
#include <Windows.h>

#include "Fixes.h"
#include "Logger.h"
#include "Memory.h"
#include "Unreal.h"
#include "Startup.h"
#include "Console.h"
#include "Failsafe.h"

namespace Shard
{
    static HANDLE hConsole;

    DWORD WINAPI Main(LPVOID param)
    {
        while (true)
        {
            Sleep(1000 / 60);
            if (Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController)
            {
                UObject** PCCheatManager = reinterpret_cast<UObject**>((uintptr_t)(Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController) + 0x338);
                if (*PCCheatManager == nullptr)
                {
                    Console::GrantCheatmanager();
                }
            }
        }
    }

    void Shard::Initialize()
    {       
        AllocateConsole();
        Logger::Log("Welcome to Shard v1.0.0");

        // Start initialization process
        Startup::Initialize();
        Hook::Initialize();
        Memory::Initialize();
        Unreal::Initialize();
        Fixes::Initialize();

        // Verify if everything went right, if not, immediately stop before damage is done.
        if (!FailsafeUtils::Verify())
            return;

        // Initialize console
        Console::Initialize();
        Console::GrantCheatmanager();
        CreateThread(0, 0, Main, 0, 0, 0);
    }

    void Shard::AllocateConsole()
    {
        AllocConsole();

        FILE* pFile;
        freopen_s(&pFile, "CONIN$", "r", stdout);
        freopen_s(&pFile, "CONOUT$", "w", stderr);
        freopen_s(&pFile, "CONOUT$", "w", stdout);
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    }
}