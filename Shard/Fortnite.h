#pragma once

#include "Hook.h"
#include "Unreal.h"
#include "Structs.h"

namespace Shard
{
    class Fortnite
    {
	public:
		static void DropLoading()
		{
			GetPlayerControllerParams gpcParams;
			GetGameModeParams ggmParams;

			// Set gamemode parameters
			gpcParams.PlayerIndex = 0;
			gpcParams.WorldContextObject = (UObject*)Globals::UWorld;
			ggmParams.WorldContextObject = (UObject*)Globals::UWorld;

			// Find objects
			auto playerController = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.GetPlayerController");
			auto serverReadyToStartMatch = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
			auto startMatch = Unreal::FindObjectJake(L"Function /Script/Engine.GameMode.StartMatch");
			auto getGameMode = Unreal::FindObjectJake(L"Function /Script/Engine.GameplayStatics.GetGameMode");

			ProcessEvent(Globals::GameplayStatics, playerController, &gpcParams);
			ProcessEvent((UObject*)gpcParams.ReturnValue, serverReadyToStartMatch, nullptr);
			ProcessEvent(Globals::GameplayStatics, getGameMode, &ggmParams);
			ProcessEvent((UObject*)ggmParams.ReturnValue, startMatch, nullptr);
		}
    };
}