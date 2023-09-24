#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>
#include "dt_recv.h"
#include "crc.h"	

#define Netvar(name, type, netvar) type& name()						\
{																	\
	int nOffset = g_Netvar.GetOffset(crc32(_strdup(netvar)));		\
	return *reinterpret_cast<type*>((DWORD)this + nOffset);			\
}	

class NetvarManager
{
public:
	void Initialize();
	int GetOffset(DWORD dwHash) { return propMap[dwHash]->GetOffset(); };
private:
	std::unordered_map<DWORD, RecvProp*> propMap;
	void DumpFromTable(RecvTable* pTable);
};

extern NetvarManager g_Netvar;