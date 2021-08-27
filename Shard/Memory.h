#pragma once

#include <Windows.h>
#include <winscard.h>
#include <Psapi.h>
#include <fstream>

#include "Define.h"
#include "Globals.h"
#include "Structs.h"

namespace Shard
{
    class Memory
    {
	public:

		static void Initialize()
		{
			auto UObjectPtr = Memory::FindPattern(GOBJECTS);
			Globals::Objects = decltype(Globals::Objects)(RELATIVE_ADDR(UObjectPtr, 7));
		}
		static PBYTE FindPattern(uintptr_t pModuleBaseAddress, const char* sSignature, size_t nSelectResultIndex = 0)
		{
			static auto patternToByte = [](const char* pattern)
			{
				auto       bytes = std::vector<int>{};
				const auto start = const_cast<char*>(pattern);
				const auto end = const_cast<char*>(pattern) + strlen(pattern);

				for (auto current = start; current < end; ++current)
				{
					if (*current == '?')
					{
						++current;
						if (*current == '?')
							++current;
						bytes.push_back(-1);
					}
					else
						bytes.push_back(strtoul(current, &current, 16));
				}
				return bytes;
			};

			const auto dosHeader = (PIMAGE_DOS_HEADER)pModuleBaseAddress;
			const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModuleBaseAddress + dosHeader->e_lfanew);

			const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
			auto       patternBytes = patternToByte(sSignature);
			const auto scanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);

			const auto s = patternBytes.size();
			const auto d = patternBytes.data();

			size_t nFoundResults = 0;

			for (auto i = 0ul; i < sizeOfImage - s; ++i)
			{
				bool found = true;

				for (auto j = 0ul; j < s; ++j)
				{
					if (scanBytes[i + j] != d[j] && d[j] != -1)
					{
						found = false;
						break;
					}
				}

				if (found)
				{
					if (nSelectResultIndex != 0)
					{
						if (nFoundResults < nSelectResultIndex)
						{
							nFoundResults++;
							found = false;
						}
						else
							return reinterpret_cast<PBYTE>(&scanBytes[i]);
					}
					else
						return reinterpret_cast<PBYTE>(&scanBytes[i]);
				}
			}

			return NULL;
		}
		static PBYTE FindPattern(const char* sSignature, size_t nSelectResultIndex = 0)
		{
			static MODULEINFO info = { 0 };

			GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));
			return FindPattern((uintptr_t)info.lpBaseOfDll, sSignature, nSelectResultIndex);
		}
    };
}