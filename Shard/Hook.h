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
       return SpawnActorLong(InWorld, Class, Location, Rotation, SpawnParameters);
    }

   static void DropLoading()
   {
       GetPlayerControllerParams gpcParams;
       UGameplayStatics_GetGameState_Params ggsParams;

       // Set gamemode parameters
       gpcParams.PlayerIndex = 0;
       gpcParams.WorldContextObject = (UObject*)Globals::UWorld;

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


   bool bPressed = false;




   void* __fastcall ProcessEventHook(UObject* pObject, UObject* pFunction, void* pParams)
   {
        std::wstring fullName = Unreal::GetObjectFullName(pFunction);
        std::wstring objectName = Unreal::GetObjectFullName(pObject);

        // Character Parts
        if (Unreal::GetObjectFullName(pFunction).find(L"Tick") != std::string::npos) {
            if (GetAsyncKeyState(VK_SPACE))
            {
                    bPressed = true;
                    auto GInstance = ReadPointer(Globals::UWorld, 0x180);
                    if (!GInstance) printf("GameInstance");

                    auto Players = ReadPointer(GInstance, 0x38);
                    if (!Players) printf("Players");

                    auto Player = ReadPointer(Players, 0x0); // Gets the first user in the array (LocalPlayers[0]).
                    if (!Player) printf("Player");

                    auto PlayerController = ReadPointer(Player, 0x30);
                    if (!PlayerController) printf("PlayerController");

                    auto Pawn = ReadPointer(PlayerController, 0x2A0); // Gets the user LocalPawn (Only if in-game).
                    if (!Pawn) printf("Pawn");

                    auto fn = Unreal::FindObjectJake(L"Function /Script/Engine.Character.Jump");

                    ProcessEvent((UObject*)Pawn, fn, nullptr);
            }

            if (GetAsyncKeyState(VK_F5) )
            {
                if (!bPressed) {
                    bPressed = true;
                    UObject* PlayerState = reinterpret_cast<UObject*>(Globals::PlayerController + 0x228);


                    UObject* DefaultHead = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA");
                    UObject* DefaultBody = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA");

                    DWORD CharacterDataOffset = 0x4F0;
                    DWORD PartsOffset = 0x08;

                    UObject** HeadPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset));
                    UObject** BodyPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset) + __int64(8));
                    *HeadPart = DefaultHead;
                    *BodyPart = DefaultBody;

                    UObject* OnRep_CharacterDataFunc = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterData");

                    ProcessEvent(PlayerState, OnRep_CharacterDataFunc, nullptr);
                }
            } else if (GetAsyncKeyState(VK_F3)) {
                //  DropLoading();
                auto switchlevel = Unreal::FindObjectJake(L"Function /Script/Engine.PlayerController.SwitchLevel");
                switchlevel_params switchparams;
                switchparams.URL = L"apollo_terrain?game=/Script/FortniteGame.FortGameModeEmptyDedicated";
                ProcessEvent(Globals::PlayerController, switchlevel, &switchparams);
            }
        }


            // Getting Ingame
              
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

            if (strings[0] == "CP") {

                auto GInstance = ReadPointer(Globals::UWorld, 0x180);
                if (!GInstance) printf("GameInstance");

                auto Players = ReadPointer(GInstance, 0x38);
                if (!Players) printf("Players");

                auto Player = ReadPointer(Players, 0x0); // Gets the first user in the array (LocalPlayers[0]).
                if (!Player) printf("Player");

                auto PlayerController = ReadPointer(Player, 0x30);
                if (!PlayerController) printf("PlayerController");

                auto Pawn = ReadPointer(PlayerController, 0x2A0); // Gets the user LocalPawn (Only if in-game).
                if (!Pawn) printf("Pawn");

                auto PlayerState = ReadPointer(Pawn, 0x240); // Gets the user LocalPawn (Only if in-game).
                if (!PlayerState) printf("PlayerState");

                auto KismetLib = Unreal::FindObjectJake(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary");
                auto fn = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortKismetLibrary.UpdatePlayerCustomCharacterPartsVisualization");

                UFortKismetLibrary_UpdatePlayerCustomCharacterPartsVisualization_Params params;
                params.PlayerState = (UObject*)PlayerState;

                ProcessEvent(KismetLib, fn, &params);

             /*
             UObject* DefaultHead = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_Skirmish.CP_Body_Commando_F_Skirmish");
                UObject* DefaultBody = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_Skirmish.CP_Head_F_Skirmish");
                UObject* DefaultFaceacc = Unreal::FindObjectJake(L"CustomCharacterPart /Game/Characters/CharacterParts/FaceAccessories/CP_F_MED_Skirmish_FaceAcc.CP_F_MED_Skirmish_FaceAcc");

                DWORD CharacterDataOffset = 0x4F0;
                DWORD PartsOffset = 0x08;

                UObject** HeadPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset));
                UObject** FaceAcc = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset) + __int64(7));
                UObject** BodyPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset) + __int64(8));
                *HeadPart = DefaultHead;
                *BodyPart = DefaultBody;
                *FaceAcc = DefaultFaceacc;

                UObject* OnRep_CharacterDataFunc = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterData");

                ProcessEvent((UObject*)PlayerState, OnRep_CharacterDataFunc, nullptr);
             
             
             */ 
            }
            if (strings[0] == "weapon") {

                std::wstring converted(strings[1].begin(), strings[1].end());
                auto GInstance = ReadPointer(Globals::UWorld, 0x180);
                if (!GInstance) printf("GameInstance");

                auto Players = ReadPointer(GInstance, 0x38);
                if (!Players) printf("Players");

                auto Player = ReadPointer(Players, 0x0); // Gets the first user in the array (LocalPlayers[0]).
                if (!Player) printf("Player");

                auto PlayerController = ReadPointer(Player, 0x30);
                if (!PlayerController) printf("PlayerController");

                auto Pawn = ReadPointer(PlayerController, 0x2A0); // Gets the user LocalPawn (Only if in-game).
                if (!Pawn) printf("Pawn");

                auto WeaponToEquip = Unreal::FindObjectJake(converted);

                struct
                {
                    UObject* WeaponData;
                    FGuid ItemEntryGuid;
                    UObject* ReturnValue;
                } EquipWeaponDefinitionParams;

                EquipWeaponDefinitionParams.WeaponData = WeaponToEquip;
                FGuid NewGUID;
                NewGUID.A = rand() % 1000;
                NewGUID.B = rand() % 1000;
                NewGUID.C = rand() % 1000;
                NewGUID.D = rand() % 1000;
                EquipWeaponDefinitionParams.ItemEntryGuid = NewGUID;

                auto EquiptWeaponFunc = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPawn.EquipWeaponDefinition");

                ProcessEvent((UObject*)Pawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);

                int** AmmoCount = reinterpret_cast<int**>(__int64(Pawn) + __int64(0x600) + __int64(0xa48));
                int a = 999;
                *AmmoCount = &a;

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
            SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
            MH_Initialize();
            MH_CreateHook(static_cast<LPVOID>((LPVOID)processEventOffset), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
            MH_EnableHook(static_cast<LPVOID>((LPVOID)processEventOffset));
            DetourTransactionBegin(); DetourAttach(&(PVOID&)SpawnActorLong, SpawnActorHook); DetourTransactionCommit();
            
        }
    };
}
