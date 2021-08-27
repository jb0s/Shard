#pragma once

#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"
#include <iostream>
#include <detours.h>

#include <MinHook.h>
#include "Unreal.h"
#pragma comment(lib, "libMinHook.x64.lib")
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))

namespace Shard
{



    DWORD WINAPI DumpObjectThread(LPVOID param)
    {
        Unreal::DumpObjects();
        system("notepad \"Dump.txt\"");
        return NULL;
    }

    typedef void* (__fastcall* fProcessEvent)(UObject* pObject, UObject* pFunction, void* pParams);
    typedef void* (__fastcall* fspawnactor)(UClass* pClass, FVector pLocation, FRotator pRotation);
    fProcessEvent ProcessEvent;
    fspawnactor SpawnActor;




   static UObject* SpawnActorHook(UObject* InWorld,UClass* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters)
    {

       // Scuff Way To get ingame this is a NEED TO FIX!!!

    

    
    
   
   
   if (Unreal::GetObjectFullName((UObject*)Class).find(L"DefaultPawn") != std::string::npos) {
           //BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C
           Class = (UClass*)Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C");
       }

       if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortPlayerController") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
       {
           Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortPlayerControllerZone");
       }
       if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortGameState") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
       {
           Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortGameStateZone");
       }
       if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortPlayerState") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
       {
           Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortPlayerStateZone");
       }
       if (GetAsyncKeyState(VK_F10)) {
           if (Unreal::GetObjectFullName((UObject*)Class).find(L"PlayerPawn_Generic_C") != std::string::npos) {
               //BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C
               Class = (UClass*)Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C");
           }

       }
       return SpawnActorLong(InWorld, Class, Location, Rotation, SpawnParameters);
    }

   static void DropLoading()
   {
       GetPlayerControllerParams gpcParams;
       UGameplayStatics_GetGameState_Params ggsParams;

       // Set gamemode parameters
       gpcParams.PlayerIndex = 0;
       gpcParams.WorldContextObject = (UObject*)Globals::UWorld;
       ggsParams.WorldContextObject = (UObject*)Globals::UWorld;

       // Find objects
       auto serverReadyToStartMatch = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
       auto startmatch = Unreal::FindObjectJake(L"Function /Script/Engine.GameMode.StartMatch");
       auto getgamestate = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.GetGameState");


       // Process Event Shit
       ProcessEvent(Globals::GameplayStatics, getgamestate, &ggsParams);
       ProcessEvent(Globals::PlayerController, serverReadyToStartMatch, nullptr);
       ProcessEvent(Globals::PlayerController, startmatch, nullptr);

       //Update these every update 
       reinterpret_cast<bools*>(Globals::PlayerController)->bHasInitiallySpawned = true;
       reinterpret_cast<bools*>(Globals::PlayerController)->bAssignedStartSpawn = true;
       reinterpret_cast<bools*>(Globals::PlayerController)->bReadyToStartMatch = true;
       reinterpret_cast<bools*>(Globals::PlayerController)->bClientPawnIsLoaded = true;
       *reinterpret_cast<bool*>(Globals::PlayerController + 0x928) = true; //bHasClientFinishedLoading
       *reinterpret_cast<bool*>(Globals::PlayerController + 0x929) = true; //bHasServerFinishedLoading
   }


    void* __fastcall ProcessEventHook(UObject* pObject, UObject* pFunction, void* pParams)
    {
        std::wstring fullName = Unreal::GetObjectFullName(pFunction);
        std::wstring objectName = Unreal::GetObjectFullName(pObject);


        // Character Parts
        if (Unreal::GetObjectFullName(pFunction).find(L"Tick") != std::string::npos) {
            if (GetAsyncKeyState(VK_F5))
            {

                auto Hero = Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortHero");
                auto CharacterParts = reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<uintptr_t>(Hero) + 0x220);
                CharacterParts->operator[](1) = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Characters/CharacterParts/Male/Medium/Heads/CP_Athena_Head_M_AshtonMilo.CP_Athena_Head_M_AshtonMilo");
                CharacterParts->operator[](0) = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Athena_Body_M_AshtonMilo.CP_Athena_Body_M_AshtonMilo");
                auto KismetLib = Unreal::FindObjectJake(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary");
                auto fn = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortKismetLibrary.ApplyCharacterCosmetics");
                UObject* PlayerState = reinterpret_cast<UObject*>(Globals::PlayerController + 0x240);
                UFortKismetLibrary_ApplyCharacterCosmetics_Params params;
                params.CharacterParts = *CharacterParts;
                params.PlayerState = PlayerState;
                params.WorldContextObject = (UObject*)Globals::UWorld;

            }
            // Getting Ingame
            if (GetAsyncKeyState(VK_F3))
            {
              //  DropLoading();
                auto switchlevel = Unreal::FindObjectJake(L"Function /Script/Engine.PlayerController.SwitchLevel");
                switchlevel_params switchparams;
                switchparams.URL = L"ds_buildergridplane?game=/Script/FortniteGame.FortGameModeEmptyDedicated";
                ProcessEvent(Globals::PlayerController, switchlevel, &switchparams);
            }
        }




        if (fullName.find(L"SendClientHello") != std::string::npos ||
            fullName.find(L"SendPacketToServer") != std::string::npos ||
            fullName.find(L"SendPacketToClient") != std::string::npos)
        {
            return NULL;
        }

        if (fullName.find(L"CheatScript") != std::string::npos)
        {
            auto params = reinterpret_cast<CheatScriptParams*>(pParams);
            auto fstring = params->ScriptName;
            auto string = params->ScriptName.ToString();
            auto strings = StringUtils::Split(string, " ");

            if (strings[0] == "exec") {
                auto obj = Unreal::FindObjectJake(std::wstring(strings[1].begin(), strings[1].end()));
                auto func = Unreal::FindObjectJake(std::wstring(strings[2].begin(), strings[2].end()));

                ProcessEvent(obj, func, nullptr);
            }

            if (strings[0] == "play") {
                auto func = Unreal::FindObjectJake(L"Function /Script/MovieScene.MovieSceneSequencePlayer.Play");
                auto obj = Unreal::FindObjectJake(std::wstring(strings[1].begin(), strings[1].end()));

                ProcessEvent(obj, func, nullptr);
            }

            if (strings[0] == "dump") {
                CreateThread(0, 0, DumpObjectThread, 0, 0, 0);
            }

            if (strings[0] == "test") {
                auto LocalPawn = ReadPointer(Globals::PlayerController, 0x2A0);

              auto ReviveFromDBNOTime = *(float*)((PBYTE)LocalPawn + 0x3490);
                ReviveFromDBNOTime = 0.101;
                *(float*)((PBYTE)LocalPawn + 0x3490) = 0.101;
            }

            if (strings[0] == "jonl") {
                auto path = strings[1]; // folder path
                auto classPath = strings[2]; // class path
                auto kismet = Unreal::FindObjectJake(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"); // find kismet
                UClass* classInstance = reinterpret_cast<UClass*>(Unreal::FindObjectJake(std::wstring(classPath.begin(), classPath.end()))); // find the class from clasPath parameter

                JonLHack_GetAllObjectsOfClassFromPathParams Params{ std::wstring(path.begin(), path.end()).c_str(), classInstance }; // set up parameters
                ProcessEvent(kismet, Globals::JonLHack, &Params);
            }
        }

        return ProcessEvent(pObject, pFunction, pParams);
    }

    class Hook
    {
    public:
        static void Initialize()
        {
            auto processEventOffset = Memory::FindPattern(PROCESS_EVENT);
            auto processEventAddress = processEventOffset + 5 + *reinterpret_cast<int32_t*>(processEventOffset + 1);
            SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
            MH_Initialize();
            MH_CreateHook(static_cast<LPVOID>((LPVOID)processEventAddress), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
            MH_EnableHook(static_cast<LPVOID>((LPVOID)processEventAddress));
            DetourTransactionBegin(); DetourAttach(&(PVOID&)SpawnActorLong, SpawnActorHook); DetourTransactionCommit();
            
        }
    };
}