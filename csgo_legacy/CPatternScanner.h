#pragma once
#include "CMemory.h"

#define FindA(szModuleName, pattern) Find("csgo.exe", szModuleName, pattern)

class CPatternScanner
{
public:
    void Initialize()
    {
        addrLockStudioHdr = FindA("client.dll", "55 8B EC 51 53 8B D9 56 57 8D B3"); //"E9 ? ? ? ? 32 C0 5D"
        addrGlobalsBase = FindA("client.dll", "A1 ? ? ? ? 56 8B F1 FF 70");
        addrComputeHitboxSurroundingBox = FindA("client.dll", "E9 ? ? ? ? 32 C0 5D");
    }

    DWORD Find(const char* szProcessName, const char* szModuleName, const char* pattern);

    DWORD GetLockStudioHdr() { return addrLockStudioHdr; };
    DWORD GetHitboxSurroundingBox() { return addrComputeHitboxSurroundingBox; };
    DWORD GetGlobalVarsBase() { return addrGlobalsBase; };
private:
    DWORD addrLockStudioHdr = NULL;
    DWORD addrGlobalsBase = NULL;
    DWORD addrComputeHitboxSurroundingBox = NULL;
};

//"\xE8\xCC\xCC\xCC\xCC\xB0\xCC\x59\x5D\xC2"

extern CPatternScanner g_Pattern;