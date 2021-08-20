#pragma once

#include "Hook.h"
#include "Globals.h"
#include "Structs.h"

namespace Shard
{
    class Console
    {
	public:
		static void Initialize()
		{
			SpawnObjectParams params{(UClass*)(Globals::ConsoleClass), (UObject*)(Globals::FortGameViewportClient)};
			UObject** ViewportConsole = reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::FortGameViewportClient) + 0x40);

			ProcessEvent(Globals::GameplayStatics, Globals::SpawnObject, &params);

			*ViewportConsole = params.ReturnValue;
		}


		static UObject* SpawnActor(UClass* Class, FVector loc, FQuat rot) {


			struct UGameplayStatics_GetPlayerController_Params
			{
				class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
				int                                                PlayerIndex;                                              // (Parm, ZeroConstructor, IsPlainOldData)
				class APlayerController* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
			};

			UGameplayStatics_GetPlayerController_Params params1;

			params1.PlayerIndex = 0;
			params1.WorldContextObject = (UObject*)Globals::UWorld;


			auto getplayercontroller = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.GetPlayerController");
			ProcessEvent(Globals::GameplayStatics, getplayercontroller, &params1);

			struct BeginSpawnActorFromClass
			{
				class UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
				class UClass* ActorClass;                                               // (Parm, ZeroConstructor, IsPlainOldData)
				struct FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
				bool                                               bNoCollisionFail;                                         // (Parm, ZeroConstructor, IsPlainOldData)
				class AActor* Owner;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
				class AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
			};
			BeginSpawnActorFromClass params;
			params.WorldContextObject = reinterpret_cast<UObject*>(params1.ReturnValue);
			params.ActorClass = Class;

			FTransform transform;
			transform.Rotation = rot;
			transform.Translation = loc;
			transform.Scale3D = FVector{ 0,0,0 };
			params.SpawnTransform = transform;
			params.Owner = nullptr;

			auto startspawnactor = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.BeginSpawningActorFromClass");
			ProcessEvent(Globals::GameplayStatics, startspawnactor, &params);


			/// Next one

			struct FinishSpawningActor
			{
				class AActor* Actor;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
				struct FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
				class AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
			};

			auto finishspawningactor = Unreal::FindObjectJake(L"/Script/Engine.GameplayStatics.BeginSpawningActorFromClass");

			FinishSpawningActor params2;
			params2.SpawnTransform = transform;
			params2.ReturnValue;
			params2.Actor = params.ReturnValue;

			ProcessEvent(Globals::GameplayStatics, finishspawningactor, &params2);


		}

		static void GrantCheatmanager()
		{
			Logger::Log("Attempting to grant CheatManager");

			if (Globals::UWorld != nullptr) {
				if (Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController != nullptr) {
					SpawnObjectParams Params{ (UClass*)(Globals::CheatManager), (UObject*)(Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController) };
					ProcessEvent(Globals::GameplayStatics, Globals::SpawnObject, &Params);
					Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->CheatManager = Params.ReturnValue;
					return;
				}

				Logger::Log("Granting cheatmanager failed! PlayerController is null.");
				return;
			}

			Logger::Log("Granting cheatmanager failed! UWorld is null.");
		}

		static bool HasCheatmanager()
		{
			// Verify if all data is correct before continuing to prevent a crash.
			if (!FailsafeUtils::Verify())
				return false;

			return Globals::UWorld->GameInstance->LocalPlayers[0]->PlayerController->CheatManager != nullptr;
		}
    };
}