#pragma once

#include "Structs.h"

namespace Shard
{
    class Globals
    {
    public:
        typedef FString* (__fastcall* fGetNameByIndex)(int* index, FString* res);
        typedef __int64(__fastcall* fFree)(__int64 buffer);

        static inline cUWorld* UWorld;
        static inline cEngine* GEngine;
        static inline UObject* GameplayStatics;
        static inline UObject* SpawnObject;
        static inline UObject* MakeSelfMoleGhost;
        static inline UObject* CheatManager;
        static inline UObject* MoleCheatmanager;
        static inline UObject* FortGameViewportClient;
        static inline UObject* ConsoleClass;
        static inline UObject* Say;
        static inline UObject* JonLHack;
        static inline UObject* WidgetReturnValue;
        static inline UObject* PlayerController;
        static inline uintptr_t AcknowledgedPawnOFfset = 0x2a0;
        static inline PBYTE processEventOffset;
        static inline fGetNameByIndex GetNameByIndex;
        static inline fFree Free;
        static inline std::wstring ClassToken;
        static inline GObjects* Objects;
        static inline FActorSpawnParameters* gSpawnParams;
        static inline UObject* gSpawnActorWorld;
    };
}
