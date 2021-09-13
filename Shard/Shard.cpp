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
#include "Fortnite.h"

namespace Shard
{
    static HANDLE hConsole;

    bool GrantedCharacterParts = false;
    bool DestroyedHLODs = false;
    bool Isskydiving;

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

            if (ingame == true) {

                if (Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController)
                {
                    if (Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn && GrantedCharacterParts == false)
                    {
                        auto KismetLib = Unreal::FindObjectJake(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary");
                        auto fn = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization");

                        UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
                        params.PlayerState = (UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn->PlayerState;

                        ProcessEvent(KismetLib, fn, &params);
                        GrantedCharacterParts = true;
                        if (DestroyedHLODs == false) 
                        {
                            Fortnite::ExecuteConsoleCommand(L"god");
                            Fortnite::ExecuteConsoleCommand(L"destroyall forthlodsmactor");
                            Fortnite::ExecuteConsoleCommand(L"bugitgo 0 0 0");
                            Fortnite::TeleportToSkyDive(1000);
                           //EquipWeapon(L"");
                            DestroyedHLODs = true;
                            bool Isskydiving = Fortnite::IsSkydiving();
                        }
                    }
                }

                //if (GetAsyncKeyState(VK_SPACE) && Isskydiving == false) {
                //    static auto fn = Unreal::FindObjectJake(L"Function /Script/Engine.Character.Jump");
                //
                //    ProcessEvent((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn, fn, nullptr);
                //}
            }
            if (GetAsyncKeyState(VK_F6)) {


               // Showskin();
               /*
               auto RemoveFromViewport = Unreal::FindObjectJake(L"Function /Script/UMG.UserWidget.RemoveFromViewport");
                ProcessEvent(Globals::WidgetReturnValue, RemoveFromViewport, nullptr);
               
               */ 
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
        Fortnite::GetPlayerController();

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