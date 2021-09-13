#pragma once

#include "Hook.h"
#include "Unreal.h"
#include "Structs.h"
#include "Define.h"
#include "Memory.h"
#include "Util.h"

namespace Shard

{
    class Fortnite
    {
	public:

		static UObject* SpawnActor(UClass* Class, FVector loc, FRotator rot) {
			FActorSpawnParameters params{};
			auto actor = SpawnActorLong(((UObject*)Globals::UWorld), Class, &loc, &rot, params);
			return actor;
		}

		static void WidgetSpawner() {
			auto widget = Unreal::FindObjectJake(L"WidgetBlueprintGeneratedClass /Game/Sounds/AudioAnalysis/DebugHUD/WB_AudioAnalysis_FloatGraph.WB_AudioAnalysis_FloatGraph_C");
			SpawnObjectParams Params{ (UClass*)(widget), (UObject*)(Globals::UWorld) };
			ProcessEvent(Globals::GameplayStatics, Globals::SpawnObject, &Params);
			addtoviewport params2;
			params2.ZOrder = 0;
			auto addtoviewport = Unreal::FindObjectJake(L"Function /Script/UMG.UserWidget.AddToViewport");
			ProcessEvent(Params.ReturnValue, addtoviewport, &params2);
		}

		static UObject* GetPlayerController() {
			UObject* GameplayStatics = Unreal::FindObjectJake(L"GameplayStatics /Script/Engine.Default__GameplayStatics");
			UObject* GetPlayerController = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetPlayerController");

			GetPlayerControllerParams params;
			params.WorldContextObject = (UObject*)Globals::UWorld;
			params.PlayerIndex = 0;

			ProcessEvent(GameplayStatics, GetPlayerController, &params);
			Globals::PlayerController = (UObject*)params.ReturnValue;
			return (UObject*)params.ReturnValue;
		}


		static UObject* ShowDebug() {
			struct ExecuteConsoleCommands
			{
				class UObject* WorldContextObject;
				struct FString Command;
				class APlayerController* SpecificPlayer;
			};
			ExecuteConsoleCommands params;
			params.WorldContextObject = reinterpret_cast<UObject*>(Globals::PlayerController);
			params.Command = L"showdebug";
			params.SpecificPlayer = reinterpret_cast<APlayerController*>(Globals::PlayerController);
			auto executeconsolecommand = Unreal::FindObjectJake(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand");
			auto kismet = Unreal::FindObjectJake(L"Default__KismetSystemLibrary");

			ProcessEvent(kismet, executeconsolecommand, &params);
		}

		static bool IsSkydiving()
		{
			bool ReturnValue;
			auto iskydiving = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerPawn.IsSkydiving");
			ProcessEvent((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn, iskydiving, &ReturnValue);
			return ReturnValue;
		}

		static void TeleportToSkyDive(float InHeight) { auto TeleportToSkyDiveFunc = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerPawnAthena.TeleportToSkyDive"); ProcessEvent((UObject*)Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn, TeleportToSkyDiveFunc, &InHeight); }

		static void ExecuteConsoleCommand(FString command) {
			struct ExecuteConsoleCommands
			{
				class UObject* WorldContextObject;
				struct FString Command;
				class APlayerController* SpecificPlayer;
			};
			ExecuteConsoleCommands params;
			params.WorldContextObject = reinterpret_cast<UObject*>(Globals::PlayerController);
			params.Command = command;
			params.SpecificPlayer = reinterpret_cast<APlayerController*>(Globals::PlayerController);
			auto executeconsolecommand = Unreal::FindObjectJake(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand");
			auto kismet = Unreal::FindObjectJake(L"Default__KismetSystemLibrary");

			ProcessEvent(kismet, executeconsolecommand, &params);
		}

		static void Summon(const char* ClassToSummon)
		{
			std::string StrClassToSummon(ClassToSummon);
			FString FStrClassToSummon = FString(std::wstring(StrClassToSummon.begin(), StrClassToSummon.end()).c_str());
			auto CheatManager = Unreal::FindObjectJake(L"Class /Script/Engine.CheatManager");
			auto SummonFunc = Unreal::FindObjectJake(L"Function /Script/Engine.CheatManager.Summon");
			ProcessEvent(CheatManager, SummonFunc, &FStrClassToSummon);
		}

		static void Possess(UObject* InController, UObject* InPawn) { auto PossessFunc = Unreal::FindObjectJake(L"Function /Script/Engine.Controller.Possess"); ProcessEvent(InController, PossessFunc, &InPawn); }
		static void SpawnPawn() {

			auto GInstance = ReadPointer(Globals::UWorld, 0x180);
			if (!GInstance) printf("GameInstance");

			auto Players = ReadPointer(GInstance, 0x38);
			if (!Players) printf("Players");

			auto Player = ReadPointer(Players, 0x0); // Gets the first user in the array (LocalPlayers[0]).
			if (!Player) printf("Player");

			auto PlayerController = ReadPointer(Player, 0x30);
			if (!PlayerController) printf("PlayerController");

			//Summon("PlayerPawn_Athena_C");
			auto PawnClass = (UClass*)Unreal::FindObjectJake(L"BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C");
			FVector loc{ 0,0,0 };
			FRotator rot{ 0,0,0 };
			auto spawnedpawn = SpawnActorLong((UObject*)Globals::UWorld,PawnClass,&loc,&rot,*Globals::gSpawnParams);

			Possess((UObject*)PlayerController, spawnedpawn);
		}


		auto SetMovementMode()
		{

			unsigned char CustomMode;

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

			auto Movement = ReadPointer(PlayerController, 0x288);
			if (!Movement) printf("Movement");

			auto fn = Unreal::FindObjectJake(L"Function /Script/Engine.CharacterMovementComponent.SetMovementMode");

			UCharacterMovementComponent_SetMovementMode_Params params;

			params.NewMovementMode = EMovementMode::MOVE_Flying;
			params.NewCustomMode = CustomMode;

			ProcessEvent((UObject*) Movement, fn, &params);
		}

    };
}