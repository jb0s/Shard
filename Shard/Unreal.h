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

namespace Shard
{
    class Unreal
    {
	public:
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
    };
}