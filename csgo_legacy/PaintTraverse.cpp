#include "PaintTraverse.h"
#include "DrawManager.h"

void CPaintTraverse::DrawBox(CBasePlayer* pPlayer, int yoffset)
{
	Vector posMin, posMax;
	Vector mins, maxs;

	ICollideable* pCollideable = pPlayer->GetCollideable();

	mins = pCollideable->OBBMins();
	maxs = pCollideable->OBBMaxs();

	if ((I.g_pDebugOverlay->ScreenPosition(mins, posMin) != 1) && (I.g_pDebugOverlay->ScreenPosition(maxs, posMax) != 1))
	{
		g_pDraw->DrawFilledRect((int)posMin.x, (int)posMax.y, (int)posMax.x, (int)posMin.y, 255, 0, 0, 20);

		g_pDraw->FastDrawText(std::to_string(posMin.x), 200, 450, g_Vars.fontCourierNew, 230, 230, 230, 220, false);
		g_pDraw->FastDrawText(std::to_string(posMax.y), 200, 480, g_Vars.fontCourierNew, 230, 230, 230, 220, false);
		g_pDraw->FastDrawText(std::to_string(posMax.x), 200, 510, g_Vars.fontCourierNew, 230, 230, 230, 220, false);
		g_pDraw->FastDrawText(std::to_string(posMin.y), 200, 540, g_Vars.fontCourierNew, 230, 230, 230, 220, false);
	}
}

void CPaintTraverse::DrawName(CBasePlayer* pPlayer, int yoffset)
{
	player_info_t info;

	if (!I.g_pEngineClient->GetPlayerInfo(pPlayer->entindex(), &info))
		return;

	Vector view;
	Vector origin = pPlayer->GetAbsOrigin();

	if (I.g_pDebugOverlay->ScreenPosition(origin, view) != 1)
	{
		g_pDraw->FastDrawText(info.name, (int)view.x, (int)view.y + yoffset, g_Vars.espFont, 230, 230, 230, 220, true);
	}
}

void CPaintTraverse::DrawHealth(CBasePlayer* pPlayer, int yoffset)
{
	Vector view;
	Vector origin = pPlayer->GetAbsOrigin();

	int nHealth = pPlayer->GetHealth();

	std::string strHealth = std::to_string(nHealth) + " HP";

	if (I.g_pDebugOverlay->ScreenPosition(origin, view) != 1)
	{
		g_pDraw->FastDrawText(strHealth, (int)view.x, (int)view.y + yoffset, g_Vars.espFont, 230, 230, 230, 220, true);
	}
}

void CPaintTraverse::Hook()
{
	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		CBasePlayer* pPlayer = reinterpret_cast<CBasePlayer*>(I.g_pClientEntityList->GetClientEntity(i));
		CBasePlayer* pLocalPlayer = GetLocal();

		if (!I.g_pEngineClient->IsInGame() || !pPlayer || pPlayer->GetHealth() <= 0 || pPlayer->IsDormant() || (pPlayer->GetTeamNum() == pLocalPlayer->GetTeamNum()))
			continue;

		DrawName(pPlayer, 0);
		DrawHealth(pPlayer, 15);
		DrawBox(pPlayer);
	}
}

CPaintTraverse* g_pPaintTraverse;