#include "NetvarManager.h"
#include "Interfaces.h"

void NetvarManager::Initialize()
{
	for (ClientClass* cclass = I.g_pBaseClientDLL->GetAllClasses(); cclass; cclass = cclass->m_pNext)
		if (cclass->m_pRecvTable)
			DumpFromTable(cclass->m_pRecvTable);
}

void NetvarManager::DumpFromTable(RecvTable* pTable)
{
	for (int i = 0; i < pTable->GetNumProps(); i++)
	{
		RecvProp* pProp = pTable->GetPropA(i);

		if (!pProp || std::isdigit(pProp->GetName()[0]))
			continue;

		if (!Q_strcmp(pProp->GetName(), "baseclass"))
			continue;

		std::string strName = pTable->GetName();
		strName += ":";
		strName += pProp->GetName();

		char* toHash = _strdup(strName.c_str());

		propMap[crc32(toHash)] = pProp;

		g_Vars.logs << "Netvar | " << toHash << "\n";

		delete(toHash);
	}
}

NetvarManager g_Netvar;