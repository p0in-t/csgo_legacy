#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <vector>

class CMemory
{
public:
	HANDLE GetHandle(const char* szProcessName);
	HMODULE GetModule(const char* szProcessName, HANDLE hHandle);
	MODULEINFO GetInformation(const char* szProcessName, const char* szModuleName);
};

extern CMemory* g_pMemory;