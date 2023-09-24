#include "CMemory.h"

HANDLE CMemory::GetHandle(const char* szProcessName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (strcmp(entry.szExeFile, szProcessName) == FALSE) {
                HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                CloseHandle(snapshot);

                return hHandle;
            }
        }
    }

    CloseHandle(snapshot);

    return NULL;
}

HMODULE CMemory::GetModule(const char* module, HANDLE hHandle) {
    HMODULE modules[512];
    DWORD cbNeeded;

    if (EnumProcessModules(hHandle, modules, sizeof(modules), &cbNeeded)) {
        for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            TCHAR szModuleName[MAX_PATH];

            if (GetModuleBaseName(hHandle, modules[i], szModuleName, sizeof(szModuleName) / sizeof(TCHAR))) {
                if (strcmp(module, szModuleName) == FALSE) {
                    return modules[i];
                }
            }
        }
    }

    CloseHandle(hHandle);
}

MODULEINFO CMemory::GetInformation(const char* szProcessName, const char* szModuleName)
{
    HANDLE hHandle = GetHandle(szProcessName);
    HMODULE hModule = GetModule(szModuleName, hHandle);

    MODULEINFO info;
    GetModuleInformation(hHandle, hModule, &info, sizeof(MODULEINFO));

    return info;
}

CMemory* g_pMemory;