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
            if (GetAsyncKeyState(VK_F12)) 
            {
                DropLoading();
                UObject* PlayerState = reinterpret_cast<UObject*>(Globals::PlayerController + 0x228); // Class Engine.Controller PlayerState 
                auto DefaultHead = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA");
                auto defaultbody = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA");
                reinterpret_cast<FCustomCharacterData*>(PlayerState + 0x4f0)->Parts[0] = (UObject*) DefaultHead;
                reinterpret_cast<FCustomCharacterData*>(PlayerState + 0x4f0)->Parts[1] = (UObject*) defaultbody;
                auto onrepcp = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterData");
                ProcessEvent(PlayerState, onrepcp, nullptr);
            }


            if (GetAsyncKeyState(VK_F6)) {

                Fortnite::WidgetSpawner();
               /*
               auto RemoveFromViewport = Unreal::FindObjectJake(L"Function /Script/UMG.UserWidget.RemoveFromViewport");
                ProcessEvent(Globals::WidgetReturnValue, RemoveFromViewport, nullptr);
               
               */ 

                auto LocalPawn = ReadPointer(Globals::PlayerController, 0x2A0);

                auto ReviveFromDBNOTime = *(float*)((PBYTE)LocalPawn + 0x3490);
                ReviveFromDBNOTime = 0.101;
                *(float*)((PBYTE)LocalPawn + 0x3490) = 0.101;
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