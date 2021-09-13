#pragma once

#include <fstream>
#include <Psapi.h>
#include <Windows.h>
#include <winscard.h>

#include "Memory.h"
#include "Define.h"
#include "Globals.h"
#include "Structs.h"
#include "StringUtils.h"
#include "FortUpdater/FortUpdater.h"
#include <string>
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
namespace Shard
{
    class Unreal
    {
	public:
		static UObject* (*SpawnActor)(UObject* UWorld, UClass* Class, FTransform const* UserTransformPtr,
			const FActorSpawnParameters& SpawnParameters);


		static void Initialize()
		{
			Globals::GameplayStatics = FindObjectJake(L"Default__GameplayStatics");
			Globals::SpawnObject = FindObjectJake(L"/Script/Engine.GameplayStatics.SpawnObject");
			Globals::CheatManager = FindObjectJake(L"/Script/Engine.CheatManager");
			Globals::ConsoleClass = FindObjectJake(L"/Script/Engine.Console");
			Globals::FortGameViewportClient = FindObjectJake(L"/Engine/Transient.FortEngine.FortGameViewportClient");
			Globals::Say = FindObjectJake(L"Function /Script/Engine.GameMode.Say");
			Globals::JonLHack = FindObjectJake(L"Function /Script/FortniteGame.FortKismetLibrary.JonLHack_GetAllObjectsOfClassFromPath");
		}

		static std::wstring GetObjectName(UObject* Object)
		{
			std::wstring sName(L"");

			for (auto i = 0; Object; Object = Object->Outer, ++i)
			{
				FString objName;
				int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);
				Globals::GetNameByIndex(&index, &objName);

				if (objName.IsValid())
				{
					auto shit = objName.c_str();

					sName = shit + std::wstring(i > 0 ? L"." : L"") + sName;

					Globals::Free((__int64)objName.c_str());
				}
			}

			return sName;
		}
		static std::wstring GetObjectFirstName(UObject* Object)
		{
			std::wstring sName(L"");
			FString objName;

			int index = *(int*)(reinterpret_cast<uint64_t>(Object) + 0x18);
			Globals::GetNameByIndex(&index, &objName);

			if (objName.IsValid())
			{
				sName = objName.c_str();
				Globals::Free((__int64)objName.c_str());
			}

			return sName;
		}
		static std::wstring GetObjectFullName(UObject* obj)
		{
			std::wstring name;

			if (obj->Class != nullptr)
			{
				name = GetObjectFirstName((UObject*)obj->Class);
				name += L" ";
				name += GetObjectName(obj);
			}

			return name;
		}
		static std::wstring GetClassToken(const std::wstring& objectName)
		{
			std::string fullDelimiter = ".";
			std::string classDelimiter = "_";
			std::string finalDelimiter = " ";

			std::string fullName = std::string(objectName.begin(), objectName.end());

			std::string classType = fullName.substr(0, fullName.find(fullDelimiter));
			std::string classToken = classType.substr(classType.find(classDelimiter), classType.length() - 1);
			classToken = classToken.substr(0, classToken.find(finalDelimiter));

			Logger::Log("Number is " + classToken.substr(1, classToken.length() - 1));

			return std::wstring(classToken.begin(), classToken.end());
		}
		static UObject* FindObjectJake(const std::wstring& name)
		{
			// Fix name
			std::string numName = std::string(name.begin(), name.end());
			auto strings = StringUtils::Split(numName, ".");

			numName = "";
			for (int i = 0; i < strings.size(); i++)
			{
				numName += strings[i] + std::string(Globals::ClassToken.begin(), Globals::ClassToken.end());
				if (i != strings.size() - 1) {
					numName += ".";
				}
			}

			std::cout << numName << std::endl;

			int count = Globals::Objects->Num();
			for (int i = 0; i < count; ++i)
			{
				auto object = Globals::Objects->GetByIndex(i);
				if (object)
				{
					auto objectName = GetObjectFullName(object);

					if (objectName.find(std::wstring(numName.begin(), numName.end())) != std::string::npos)
					{
						// Extract ClassToken if not extracted yet
						if (Globals::ClassToken == L"") {
							Globals::ClassToken = GetClassToken(objectName);
						}

						return object;
					}
				}
			}
			return nullptr;
		}
		static void DumpObjects()
		{
			std::ofstream log("Dump.txt");

			auto TotalObjects = Globals::Objects->Num();

			for (int i = 0; i < TotalObjects; ++i)
			{
				auto CurrentObject = Globals::Objects->GetByIndex(i);

				if (CurrentObject)
				{
					auto name = GetObjectFullName(CurrentObject);
					std::string str(name.begin(), name.end());

					log << str + "\n";

				}
			}
			return;
		}
		struct FName
		{
			int32_t ComparisonIndex;
			int32_t Number;
		};
		static void cFixName(char* Name)
		{
			for (int i = 0; Name[i] != '\0'; i++)
			{
				if (Name[i] == '_')
				{
					if (Name[i + 1] == '0' ||
						Name[i + 1] == '1' ||
						Name[i + 1] == '2' ||
						Name[i + 1] == '3' ||
						Name[i + 1] == '4' ||
						Name[i + 1] == '5' ||
						Name[i + 1] == '6' ||
						Name[i + 1] == '7' ||
						Name[i + 1] == '8' ||
						Name[i + 1] == '9')
						Name[i] = '\0';
				}
			}

			return;
		}
		static char* fGetNameByIndex(int Index)
		{
			if (Index == 0) return (char*)"";

			//auto fGetNameByIdx = reinterpret_cast<FString * (__fastcall*)(int*, FString*)>(Globals::GetNameByIndex);

			FString result;
			//fGetNameByIdx(&Index, &result);
			Globals::GetNameByIndex(&Index, &result);

			if (result.c_str() == NULL) return (char*)"";

			auto tmp = result.ToString();

			char return_string[1024];
			memcpy(return_string, std::string(tmp.begin(), tmp.end()).c_str(), 1024);

			Globals::Free((uintptr_t)result.c_str());

			cFixName(return_string);

			return return_string;
		}
		static std::string ws2s(const std::wstring& s)
		{
			int len;
			int slength = (int)s.length() + 1;
			len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
			char* buf = new char[len];
			WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
			std::string r(buf);
			delete[] buf;
			return r;
		}
		static DWORD FindOffset(const char* Class, const char* varName)
		{
			for (DWORD i = 0x0; i < Globals::Objects->Num(); i++)
			{
				//auto CurrentObject = *(uintptr_t*)(this->UObjectArray + (i * 0x18));
				//auto CurrentObject = *(uintptr_t*)(Globals::Objects->ObjectArray + (i * 0x18));
				auto CurrentObject = Globals::Objects->GetByIndex(i);
		
				if (!CurrentObject) return NULL;
				if (!(*(uintptr_t*)(CurrentObject + 0x50)) || *(DWORD*)(CurrentObject + 0x54) == 0xFFFFFFFF) continue;
				//MessageBox(NULL, L"-0", L"-0", 0);
				const char* CurObjectName = ws2s(GetObjectFirstName((UObject*)CurrentObject->Class)).c_str();
				cFixName(const_cast<char*>(CurObjectName));
				std::cout << "CurObjectName is :" << CurObjectName << std::endl;
				//MessageBox(NULL, L"0", L"0", 0);
				if (!strcmp(CurObjectName, Class)) //Same class
				{
					MessageBox(NULL, L"1", L"1", 0);
					for (auto Property = *(uint64_t*)(CurrentObject + 0x50); !IsBadReadPtr((void*)Property, 8); Property = *(uint64_t*)(Property + 0x20))
					{
						auto Type = *(uint64_t*)(Property + 0x8);
						MessageBox(NULL, L"12", L"12", 0);
						if (!IsBadReadPtr((void*)Type, 8) && Type)
						{
							MessageBox(NULL, L"13", L"13", 0);
							auto Property_FName = *(FName*)(Property + 0x28);
							auto Offset = *(DWORD*)(Property + 0x4C);
		
							if (Offset != 0)
							{
								MessageBox(NULL, L"14", L"14", 0);
								auto Property_idx = Property_FName.ComparisonIndex;
		
								if (Property_idx)
								{
									MessageBox(NULL, L"2", L"2", 0);
									const char* PropertyName = fGetNameByIndex(Property_idx);
									MessageBox(NULL, L"3", L"3", 0);
									std::cout << "PropertyName: " << PropertyName << " varName: " << varName << std::endl;
									if (!strcmp(PropertyName, varName))
									{
										MessageBox(NULL, L"4", L"4", 0);
										return Offset;
									}
								}
							}
						}
		
					}
		
					return NULL; //Return NULL if checked all properties of the class.
				}
			}
			return NULL;
		}
    };
}