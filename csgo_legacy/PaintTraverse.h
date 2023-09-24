#pragma once
#include "CBaseEntity.h"

class CPaintTraverse
{
public:
	void DrawBox(CBasePlayer* pPlayer, int yoffset = 0);
	void DrawName(CBasePlayer* pPlayer, int yoffset = 0);
	void DrawHealth(CBasePlayer* pPlayer, int yoffset = 0);
	void Hook();
};

extern CPaintTraverse* g_pPaintTraverse;

