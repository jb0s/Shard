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
			SpawnActorLong = reinterpret_cast<decltype(SpawnActorLong)>(Memory::FindPattern(SpawnActorSig));
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
		}

		static UObject* toggledebugcamera() {
			struct ExecuteConsoleCommands
			{
				class UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
				struct FString                                     Command;                                                  // (Parm, ZeroConstructor)
				class APlayerController* SpecificPlayer;                                           // (Parm, ZeroConstructor, IsPlainOldData)
			};

			ExecuteConsoleCommands params;

			params.WorldContextObject = reinterpret_cast<UObject*>(Globals::PlayerController);
			params.Command = L"showdebug";
			params.SpecificPlayer = reinterpret_cast<APlayerController*>(Globals::PlayerController);
			auto executeconsolecommand = Unreal::FindObjectJake(L"Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand");
			auto kismet = Unreal::FindObjectJake(L"Default__KismetSystemLibrary");

			ProcessEvent(kismet, executeconsolecommand, &params);
		}
    };
}