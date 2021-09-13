#pragma once

#define UWORLD "48 8B 05 ? ? ? ? 4D 8B C2"
#define GOBJECTS "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"
#define FREE "48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"
#define GET_NAME_BY_INDEX "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 8B F2 44 39 71 04 0F 85 ? ? ? ? 8B 19 0F B7 FB E8 ? ? ? ? 8B CB 48 8D 54 24"
#define PROCESS_EVENT "40 55 53 56 57 41 54 41 56 41 57 48 81 EC"
#define ENVIRONMENT_POPUP "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 F0"
#define GENGINE "E8 ? ? ? ? 44 38 35 ? ? ? ? 74 EB"
#define PROCESS_EVENT_TABLE 0x44
#define ENGINE_OFFSET 0x965E3E8
#define ExitSig "48 83 EC 48 8A 15 ? ? ? ?" // Engine exit requested
#define Notification "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 73 F0" // Notfication
#define FNCrashHook "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 40 45 33 FF 4C 8D 35 ? ? ? ?" // Unreal Crash
#define RequestExit "E8 ? ? ? ? 38 1D ? ? ? ? 75 72" // RequestExitWithStatus
//#define SpawnActorSig "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 70 45 33 ED"
#define SpawnActorSig "E8 ? ? ? ? 48 89 87 ? ? ? ? 4C 39 B7 ? ? ? ? 75 0C"
//#define ATHENA_GAMEMODE_CRASH "48 83 EC 28 48 8B 05 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 83 C4"
#define ATHENA_GAMEMODE_CRASH "E8 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 45 33 C0"
#define ATHENA_GAMEMODE_CRASH2 "48 83 EC 28 B1 01 E8 ? ? ? ? B0 01"
#define REQUEST_EXIT_GAMEMODE "E8 ? ? ? ? 49 8B FD 49 89 1E"
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
