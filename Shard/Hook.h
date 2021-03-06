#pragma once

#include "Define.h"
#include "Failsafe.h"
#include "StringUtils.h"
#include <iostream>

#include <MinHook.h>
#pragma comment(lib, "libMinHook.x64.lib")
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))

namespace Shard
{
    DWORD WINAPI DumpObjectThread(LPVOID param)
    {
        Unreal::DumpObjects();
        system("notepad \"Dump.txt\"");
        return NULL;
    }

    typedef void* (__fastcall* fProcessEvent)(UObject* pObject, UObject* pFunction, void* pParams);
    fProcessEvent ProcessEvent;
    void* __fastcall ProcessEventHook(UObject* pObject, UObject* pFunction, void* pParams)
    {
        std::wstring fullName = Unreal::GetObjectFullName(pFunction);
        std::wstring objectName = Unreal::GetObjectFullName(pObject);

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

            if (strings[0] == "jonl") {
                auto path = strings[1]; // folder path
                auto classPath = strings[2]; // class path
                auto kismet = Unreal::FindObjectJake(L"FortKismetLibrary /Script/FortniteGame.Default__FortKismetLibrary"); // find kismet
                UClass* classInstance = reinterpret_cast<UClass*>(Unreal::FindObjectJake(std::wstring(classPath.begin(), classPath.end()))); // find the class from clasPath parameter

                JonLHack_GetAllObjectsOfClassFromPathParams Params{ std::wstring(path.begin(), path.end()).c_str(), classInstance }; // set up parameters
                ProcessEvent(kismet, Globals::JonLHack, &Params);
            }
        }

        return ProcessEvent(pObject, pFunction, pParams);
    }

    class Hook
    {
    public:
        static void Initialize()
        {
            auto processEventOffset = Memory::FindPattern(PROCESS_EVENT);
            MH_Initialize();
            MH_CreateHook(static_cast<LPVOID>((LPVOID)processEventOffset), ProcessEventHook, reinterpret_cast<LPVOID*>(&ProcessEvent));
            MH_EnableHook(static_cast<LPVOID>((LPVOID)processEventOffset));
        }
    };
}