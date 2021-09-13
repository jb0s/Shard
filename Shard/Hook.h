#pragma once

#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"
#include <iostream>
#include <detours.h>

#include <MinHook.h>
#include "Unreal.h"
#include "Memory.h"
//#include "Fortnite.h"
#pragma comment(lib, "libMinHook.x64.lib")
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))

namespace Shard
{

    bool ingame;
    bool CP = false;
    DWORD WINAPI DumpObjectThread(LPVOID param)
    {
        Unreal::DumpObjects();
        system("notepad \"Dump.txt\"");
        return NULL;
    }

    typedef void* (__fastcall* fProcessEvent)(UObject* pObject, UObject* pFunction, void* pParams);
    typedef void* (__fastcall* fspawnactor  )(UClass* pClass, FVector pLocation, FRotator pRotation);
    fProcessEvent ProcessEventHooked;
    fspawnactor SpawnActor;

    static UObject* SpawnActorHook(UObject* InWorld, UClass* Class, FVector* Location, FRotator* Rotation, FActorSpawnParameters& SpawnParameters)
    {
        // Scuff Way To get ingame this is a NEED TO FIX!!!
        //Globals::gSpawnParams = &SpawnParameters;
        //Globals::gSpawnActorWorld = InWorld;
        if (Unreal::GetObjectFullName((UObject*)Class).find(L"DefaultPawn") != std::string::npos) {
            Class = (UClass*)Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Abilities/Player/Pawns/PlayerPawn_Generic.PlayerPawn_Generic_C");
        }
        if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortPlayerController") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
        {
            Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortPlayerControllerZone");
        }
        if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortGameState") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
        {
            //MessageBox(NULL, L"Gamestate Swapped", L"Gamestate Swapped",0);
            Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortGameStateZone");
        }
        if (Unreal::GetObjectFullName((UObject*)Class).find(L"FortPlayerState") != std::string::npos && !((Unreal::GetObjectFullName((UObject*)Class).find(L"Frontend") != std::string::npos) || (Unreal::GetObjectFullName((UObject*)Class).find(L"Zone") != std::string::npos)))
        {
            Class = (UClass*)Unreal::FindObjectJake(L"Class /Script/FortniteGame.FortPlayerStateZone");
        }
        return SpawnActorLong(InWorld, Class, Location, Rotation, SpawnParameters);
    }


    inline bool ProcessEvent(UObject* pObject, UObject* pFunction, void* pParams) {

        /// Main Process Event
        /// Why I switched to this?
        /// Incase the sig changes and I still wanna go ingame this will let me do almost everything without needing to hook shit.
        /// The Hook is mainly for Cheatscript and on thread getasync keystate etc.
        /// DO NOT REMOVE THIS!

        UObject* addr = reinterpret_cast<UObject*>(pObject);
        auto vtable = *reinterpret_cast<void***>(addr);
        auto ProcesseventVtable = static_cast<void(*)(void*, void*, void*)>(vtable[0x44]); if (!ProcesseventVtable) return false;
        ProcesseventVtable((void*)addr, (void*)pFunction, (void*)pParams);
        return true;
    }

    static UObject* SpawnActorBP(UObject* ActorClass, FVector loc)
    {
        auto spawnfunc1 = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.BeginDeferredActorSpawnFromClass");
        auto spawnfunc2 = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.FinishSpawningActor");
        UGameplayStatics_BeginDeferredActorSpawnFromClass_Params bdasfc;
        UGameplayStatics_FinishSpawningActor_Params fsap;
        bdasfc.ActorClass = (UClass*)ActorClass;
        bdasfc.CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        bdasfc.SpawnTransform.Rotation = FQuat{ 0,0,0,0 };
        bdasfc.SpawnTransform.Scale3D = FVector{ 1,1,1 };
        bdasfc.SpawnTransform.Translation = loc;
        bdasfc.WorldContextObject = (UObject*)Globals::UWorld;
        bdasfc.Owner = nullptr;

        std::wcout << Unreal::GetObjectFullName(Globals::GameplayStatics) << std::endl;
        std::wcout << Unreal::GetObjectFullName(spawnfunc1) << std::endl;

        ProcessEvent(Globals::GameplayStatics, spawnfunc1, &bdasfc);

        fsap.Actor = (UObject*)bdasfc.ReturnValue;
        fsap.SpawnTransform = bdasfc.SpawnTransform;

        ProcessEvent(Globals::GameplayStatics, spawnfunc2, &fsap);

        return (UObject*)fsap.ReturnValue;
    }

    static void DropLoading()
    {
        Unreal::Initialize();
        auto worldPattern = Memory::FindPattern(UWORLD);
        auto worldAddress = RELATIVE_ADDR(worldPattern, 7);
        Globals::UWorld = (cUWorld*)(*(uintptr_t*)worldAddress);
        Globals::GameplayStatics = Unreal::FindObjectJake(L"Default__GameplayStatics");

        GetPlayerControllerParams gpcParams;
        UObject* GameplayStatics = Unreal::FindObjectJake(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
        UObject* GetPlayerController = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetPlayerController");

        GetPlayerControllerParams params;
        params.WorldContextObject = (UObject*)Globals::UWorld;
        params.PlayerIndex = 0;

        ProcessEvent(GameplayStatics, GetPlayerController, &params);
        Globals::PlayerController = (UObject*)params.ReturnValue;

        // Set gamemode parameters
        gpcParams.PlayerIndex = 0;
        gpcParams.WorldContextObject = (UObject*)Globals::UWorld;

        // Find objects
        auto serverReadyToStartMatch = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
        auto startmatch = Unreal::FindObjectJake(L"Function /Script/Engine.GameMode.StartMatch");
        auto getgamemode = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetGameMode");
        auto getgamestate = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetGameState");
        UGameplayStatics_GetGameState_Params ggsParams;
        ggsParams.WorldContextObject = (UObject*)Globals::UWorld;
        // Function Engine.GameplayStatics.GetGameMode
        struct UGameplayStatics_GetGameMode_Params
        {
            class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
            class AGameModeBase* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
        };
        UGameplayStatics_GetGameMode_Params ggmparams;
        ggmparams.WorldContextObject = Globals::PlayerController;
        
        // Process Event Shit
        ProcessEvent(Globals::GameplayStatics, getgamestate, &ggsParams);
        // Enum FortniteGame.EAthenaGamePhase
        enum class EAthenaGamePhase : uint8_t
        {
            None,
            Setup,
            Warmup,
            Aircraft,
            SafeZones,
            EndGame,
            Count,
            EAthenaGamePhase_MAX,
        };
        ProcessEvent(Globals::PlayerController, serverReadyToStartMatch, nullptr);
        *reinterpret_cast<EAthenaGamePhase*>((uintptr_t)ggsParams.ReturnValue + 0x1e68) = EAthenaGamePhase::Warmup;
        //0x1e18
        auto CurrentPlaylistInfo = reinterpret_cast<FPlaylistPropertyArray*>((uintptr_t)ggsParams.ReturnValue + 0x1e88);
        auto PlaylistFInder = Unreal::FindObjectJake(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab");
        CurrentPlaylistInfo->BasePlaylist = PlaylistFInder;
        CurrentPlaylistInfo->OverridePlaylist = PlaylistFInder;
        auto fn = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
        ProcessEvent((UObject*)ggsParams.ReturnValue, fn, nullptr);

        auto PawnClass = Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
        FVector loc{ 0,0,0 };
        FRotator rot{ 0,0,0 };
        FActorSpawnParameters sparams{};
        //SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
        //auto spawnedpawn = SpawnActorLong((UObject*)Globals::UWorld, PawnClass, &loc, &rot, sparams);
        auto spawnedpawn = SpawnActorBP(PawnClass,loc);
        std::wcout << Unreal::GetObjectFullName(spawnedpawn) << std::endl;
        //MessageBox(NULL,L"",L"",0);

        auto PossessFunc = Unreal::FindObjectJake(L"Function /Script/Engine.Controller.Possess");
        struct
        {
            class UObject* InPawn;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
        } possesparams;

        possesparams.InPawn = spawnedpawn;

        ProcessEvent(Globals::PlayerController, PossessFunc, &possesparams);

        //ProcessEvent((UObject*)ggmparams.ReturnValue, startmatch, nullptr);

        //Update these every update 
        //reinterpret_cast<bools*>(Globals::PlayerController)->bHasInitiallySpawned = true;
        //reinterpret_cast<bools*>(Globals::PlayerController)->bAssignedStartSpawn = true;
        //reinterpret_cast<bools*>(Globals::PlayerController)->bReadyToStartMatch = true;
        //reinterpret_cast<bools*>(Globals::PlayerController)->bClientPawnIsLoaded = true;
        //*reinterpret_cast<bool*>(Globals::PlayerController + 0x928) = true; //bHasClientFinishedLoading
        //*reinterpret_cast<bool*>(Globals::PlayerController + 0x929) = true; //bHasServerFinishedLoading
    }


    bool bPressed = false;


    void EquipWeapon(std::wstring Weapon) {
        auto WeaponToEquip = Unreal::FindObjectJake(Weapon);

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

        ProcessEvent((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);

        int** AmmoCount = reinterpret_cast<int**>(__int64((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn) + __int64(0x600) + __int64(0xa48));
        int a = 999;
        *AmmoCount = &a;
    }
    void* __fastcall ProcessEventHook(UObject* pObject, UObject* pFunction, void* pParams)
    {
        std::wstring fullName = Unreal::GetObjectFullName(pFunction);
        std::wstring objectName = Unreal::GetObjectFullName(pObject);

        if (Unreal::GetObjectFullName(pFunction).find(L"Tick") != std::string::npos) {

            if (GetAsyncKeyState(VK_F4)) 
            {
                auto switchlevel = Unreal::FindObjectJake(L"Function /Script/Engine.PlayerController.SwitchLevel");
                switchlevel_params switchparams;
                switchparams.URL = L"ds_buildergridplane?game=/game/athena/athena_gamemode.athena_gamemode_c";
                ProcessEvent(Globals::PlayerController, switchlevel, &switchparams);
                //auto crashpatchAddress = Memory::FindPattern(ATHENA_GAMEMODE_CRASH);
                //if (crashpatchAddress)
                //{
                //    Logger::Log("crashpatchAddress Signuature Is Valid");
                //    DWORD dwProtection;
                //    VirtualProtect(crashpatchAddress, 21, PAGE_EXECUTE_READWRITE, &dwProtection);
                //
                //    for (int i = 0; i < 22; ++i)
                //    {
                //        printf("Index %i Is: %x\n",i, *(char*)(crashpatchAddress + i));
                //        //std::cout << "Index: " << i << " Is" << *(char*)(crashpatchAddress + i) << "\n";
                //    }
                //    //*(char*)(crashpatchAddress + 15) = 0x85;
                //    //memset(crashpatchAddress, 0x85, (size_t)15);
                //    for (int i = 0; i < 22; ++i)
                //    {
                //        if (*(char*)(crashpatchAddress + i) == 0x84)
                //            *(char*)(crashpatchAddress + i) = 0x85;
                //
                //        printf("Index %i Is: %x\n", i, *(char*)(crashpatchAddress + i));
                //        //std::cout << "Index: " << i << " Is" << *(char*)(crashpatchAddress + i) << "\n";
                //    }
                //    //
                //    //memset(crashpatchAddress, 0x85, 15);
                //    ////*reinterpret_cast<char*>(crashpatchAddress) = 0xC3;
                //    //
                //    // *(char*)(crashpatchAddress + 0x11) = 0x85;
                //    DWORD dwTemp;
                //    VirtualProtect(crashpatchAddress, 21, dwProtection, &dwTemp);
                //    auto switchlevel = Unreal::FindObjectJake(L"Function /Script/Engine.PlayerController.SwitchLevel");
                //    switchlevel_params switchparams;
                //    switchparams.URL = L"ds_buildergridplane?game=/game/athena/athena_gamemode.athena_gamemode_c";
                //    ProcessEvent(Globals::PlayerController, switchlevel, &switchparams);
                //}
            }
            if (GetAsyncKeyState(VK_F3)) 
            {
                //SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
                if (!ingame)
                {
                    DropLoading();
                }
                   
               //auto switchlevel = Unreal::FindObjectJake(L"Function /Script/Engine.PlayerController.SwitchLevel");
               //switchlevel_params switchparams;
               //switchparams.URL = L"apollo_terrain?game=/Script/FortniteGame.FortGameModeEmptyDedicated";
               //ProcessEvent(Globals::PlayerController, switchlevel, &switchparams);
               ingame = true;
            }
        }
        //if (fullName.find(L".ReadyToStartMatch") != std::string::npos)
        //{
        //    SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
        //    if (!ingame)
        //    {
        //        DropLoading();
        //    }
        //}

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

            if (strings[0] == "custompart") {

                auto PlayerState = (UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn->PlayerState;
                auto Body = strings[1];  // Body Custom Character Part path
                auto Head = strings[2]; // Head Custom Character Part path
                UObject* DefaultHead = Unreal::FindObjectJake(std::wstring(Head.begin(), Head.end()));
                UObject* DefaultBody = Unreal::FindObjectJake(std::wstring(Body.begin(), Body.end()));
                DWORD CharacterDataOffset = 0x4F0;
                DWORD PartsOffset = 0x08;
                   UObject** HeadPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset));
                   UObject** BodyPart = reinterpret_cast<UObject**>(__int64(PlayerState) + __int64(CharacterDataOffset) + __int64(PartsOffset) + __int64(7));
                   *HeadPart = DefaultHead;
                   *BodyPart = DefaultBody;
                   UObject* OnRep_CharacterDataFunc = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerState.OnRep_CharacterData");
                   ProcessEvent((UObject*)PlayerState, OnRep_CharacterDataFunc, nullptr);
            }
            if (strings[0] == "pawn") 
            {
                Unreal::Initialize();
                auto worldPattern = Memory::FindPattern(UWORLD);
                auto worldAddress = RELATIVE_ADDR(worldPattern, 7);
                Globals::UWorld = (cUWorld*)(*(uintptr_t*)worldAddress);
                Globals::GameplayStatics = Unreal::FindObjectJake(L"Default__GameplayStatics");

                UObject* GameplayStatics = Unreal::FindObjectJake(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
                UObject* GetPlayerController = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetPlayerController");

                GetPlayerControllerParams params;
                params.WorldContextObject = (UObject*)Globals::UWorld;
                params.PlayerIndex = 0;

                ProcessEvent(GameplayStatics, GetPlayerController, &params);
                Globals::PlayerController = (UObject*)params.ReturnValue;

                auto PawnClass = Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
                FVector loc{ 0,0,0 };
                FRotator rot{ 0,0,0 };
                FActorSpawnParameters sparams{};
                //SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
                //auto spawnedpawn = SpawnActorLong((UObject*)Globals::UWorld, PawnClass, &loc, &rot, sparams);
                MessageBox(NULL, L"1", L"1", 0);
                auto spawnedpawn = SpawnActorBP(PawnClass, loc);
                MessageBox(NULL, L"2", L"2", 0);
                std::wcout << Unreal::GetObjectFullName(spawnedpawn) << std::endl;
                MessageBox(NULL, L"3", L"3", 0);

                auto PossessFunc = Unreal::FindObjectJake(L"Function /Script/Engine.Controller.Possess");
                struct
                {
                    class UObject* InPawn;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
                } possesparams;

                possesparams.InPawn = spawnedpawn;

                ProcessEvent(Globals::PlayerController, PossessFunc, &possesparams);

            }

            if (strings[0] == "weapon") {

                std::wstring converted(strings[1].begin(), strings[1].end());

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

                ProcessEvent((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn, EquiptWeaponFunc, &EquipWeaponDefinitionParams);

                int** AmmoCount = reinterpret_cast<int**>(__int64((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn) + __int64(0x600) + __int64(0xa48));
                int a = 999;
                *AmmoCount = &a;

            }

        }

        return ProcessEventHooked(pObject, pFunction, pParams);
    }

    class Hook
    {
    public:
        static void Initialize()
        {
            auto processEventOffset = Memory::FindPattern(PROCESS_EVENT);
            auto SpawnActorAddress = Memory::FindPattern(SpawnActorSig);
            SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(SpawnActorAddress + 5 + *reinterpret_cast<int32_t*>(SpawnActorAddress + 1));
            //SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
            MH_Initialize();
            MH_CreateHook(static_cast<LPVOID>(processEventOffset), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEventHooked));
            MH_EnableHook(static_cast<LPVOID>(processEventOffset));
           // DetourTransactionBegin(); DetourAttach(&(PVOID&)SpawnActorLong, SpawnActorHook); DetourTransactionCommit();

        }
    };
}