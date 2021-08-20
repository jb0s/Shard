#pragma once

#include "Logger.h"
#include "Globals.h"

namespace Shard
{
    class FailsafeUtils
    {
    public:
        static bool Verify()
        {
            Logger::Log("Verifying data.");

            if (Globals::GameplayStatics == nullptr) {
                Logger::Log("Verification failed: GameplayStatics is null!");
                return false;
            }
            if (Globals::SpawnObject == nullptr) {
                Logger::Log("Verification failed: SpawnObject is null!!");
                return false;
            }
            if (Globals::CheatManager == nullptr) {
                Logger::Log("Verification failed: CheatManager is null!!");
                return false;
            }
            if (Globals::ConsoleClass == nullptr) {
                Logger::Log("Verification failed: ConsoleClass is null!!");
                return false;
            }
            if (Globals::FortGameViewportClient == nullptr) {
                Logger::Log("Verification failed: FortGameViewportClient is null!!");
                return false;
            }
            if (Globals::Say == nullptr) {
                Logger::Log("Verification failed: Say is null!!");
                return false;
            }
            if (Globals::JonLHack == nullptr) {
                Logger::Log("Verification failed: JonLHack is null!!");
                return false;
            }

            Logger::Log("No issues detected.");
            return true;
        }
    };
}