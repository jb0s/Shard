#pragma once

#include "Hook.h"
#include "Unreal.h"
#include "Structs.h"

namespace Shard
{
    class Fortnite
    {
	public:



		static std::string GetBuildVersion()
		{
			auto GetGameVersion = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortRuntimeOptions.GetGameVersion");
			auto RuntimeOptions = Unreal::FindObjectJake(L"FortRuntimeOptions /Script/FortniteGame.Default__FortRuntimeOptions");
			auto RuntimeOptionsFunction = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortRuntimeOptions.GetRuntimeOptions");
			UObject* RuntimeOptionsReturnValue;
			FString VersionReturnValue;

			ProcessEvent(RuntimeOptions, RuntimeOptionsFunction, &RuntimeOptionsReturnValue);
			ProcessEvent(RuntimeOptionsReturnValue, GetGameVersion, &VersionReturnValue);
 			return VersionReturnValue.ToString();
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
			auto playerController = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.GetPlayerController");
			auto serverReadyToStartMatch = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortPlayerController.ServerReadyToStartMatch");
			auto getgamestate = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.GetGameState");

			ProcessEvent(Globals::GameplayStatics, playerController, &gpcParams);
			ProcessEvent(Globals::GameplayStatics, getgamestate, &ggsParams);
			//Sets Gamephase
			auto CurrentPlaylistInfo = reinterpret_cast<FPlaylistPropertyArray*>((uintptr_t)ggsParams.ReturnValue + 0x1e88);
			auto PlaylistFInder = Unreal::FindObjectJake(L"FortPlaylistAthena /Game/Athena/Playlists/BattleLab/Playlist_BattleLab.Playlist_BattleLab");
			CurrentPlaylistInfo->BasePlaylist = PlaylistFInder;
			CurrentPlaylistInfo->OverridePlaylist = PlaylistFInder;
			auto fn = Unreal::FindObjectJake(L"Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo");
			ProcessEvent((UObject*)ggsParams.ReturnValue, fn, nullptr);
			ProcessEvent((UObject*)gpcParams.ReturnValue, serverReadyToStartMatch, nullptr);
			*reinterpret_cast<EAthenaGamePhase*>((uintptr_t)ggsParams.ReturnValue + 0x1e68) = EAthenaGamePhase::Setup;
		}

		static void WidgetSpawner() {
			auto widget = Unreal::FindObjectJake(L"WidgetBlueprintGeneratedClass /Game/UI/Foundation/Watermark/BuildWatermark.BuildWatermark_C");
			SpawnObjectParams Params{ (UClass*)(widget), (UObject*)(Globals::UWorld) };
			ProcessEvent(Globals::GameplayStatics, Globals::SpawnObject, &Params);
			addtoviewport params2;
			params2.ZOrder = 0;
			auto addtoviewport = Unreal::FindObjectJake(L"Function /Script/UMG.UserWidget.AddToViewport");
			ProcessEvent(Params.ReturnValue, addtoviewport, &params2);
		}
    };
}