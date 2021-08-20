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