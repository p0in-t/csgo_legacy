#include "CPatternScanner.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <functional>

DWORD CPatternScanner::Find(const char* szProcessName, const char* szModuleName, const char* pattern)
{
	HANDLE hHandle = g_pMemory->GetHandle(szProcessName);

	MODULEINFO info = g_pMemory->GetInformation(szProcessName, szModuleName);

	std::vector<std::int8_t> vecBytePattern;

	auto ParsePattern = [](const char* ptrn)
	{
		int length = strlen(ptrn);

		char* first = const_cast<char*>(ptrn);

		char* end = const_cast<char*>(ptrn) + length;

		std::vector<std::int8_t> bytes;

		for (char* chCurrent = first; chCurrent < end; chCurrent++)
		{
			if (*chCurrent == '?')
			{
				chCurrent++;
				bytes.push_back(-1);
			}
			else
			{
				bytes.push_back(strtoul(chCurrent, &chCurrent, 16)); //pass reference to same current char so it saves the end to the same char for iteration
			}
		}

		return bytes;
	};

	int nSize = (int)(info.SizeOfImage);
	int iBegin = (int)(info.lpBaseOfDll);

	std::int8_t* bytes = reinterpret_cast<std::int8_t*>(info.lpBaseOfDll);
	vecBytePattern = ParsePattern(pattern);

	for (DWORD i = 0; i < nSize - vecBytePattern.size(); i++)
	{
		bool bFound = true;

		for (unsigned int j = 0; j < vecBytePattern.size(); j++)
		{
			if (vecBytePattern.at(j) != -1 && bytes[i + j] != vecBytePattern.at(j))
			{
				bFound = false;
				break;
			}
		}

		if (bFound)
			return reinterpret_cast<DWORD>(&bytes[i]);
	}

	return NULL;
}

//"55 8B EC 51 53 8B D9 56 57 8D B3"

CPatternScanner g_Pattern;