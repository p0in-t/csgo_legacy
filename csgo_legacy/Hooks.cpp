#include "Hooks.h"
#include "CBaseEntity.h"
#include "PaintTraverse.h"
#include <thread>
#include <chrono>
#include <string>

CHooks H;

float rad;
float oldY, curY;
float deltaY;
float cosD;
float sinD;

void __fastcall HookedPaintTraverse(void* pThis, void* ecx, uint32_t panel, bool forceRepaint, bool allowForce)
{
	H.m_VirtualData.OGPaintTraverse(pThis, panel, forceRepaint, allowForce);

	if (!strcmp(I.g_pPanel->GetName(panel), "Engine Tools"))
	{
		//CBasePlayer* pPlayer = reinterpret_cast<CBasePlayer*>(I.g_pClientEntityList->GetClientEntity(I.g_pEngineClient->GetLocalPlayer()));

		g_pDraw->FastDrawText("csgo legacy v1", 400, 400, g_Vars.fontCourierNew, NULL, false);

		//g_pDraw->FastDrawText(std::to_string(pPlayer->GetHealth()), 400, 440, g_Vars.fontCourierNew, NULL);

		g_pPaintTraverse->Hook();
		
	}
}

bool __fastcall HookedCreateMove(void* pThis, void* ecx, float flInputSampleTime, CUserCmd* cmd)
{
	bool bReturn = H.m_VirtualData.OGCreateMove(pThis, flInputSampleTime, cmd);

	if (!cmd || !cmd->command_number)
		return bReturn;

	/*QAngle ang;
	float oldY, curY;
	float deltaY;

	I.g_pEngineClient->GetViewAngles(ang);
	oldY = ang.y;

	cmd->viewangles.y = 0.f;

	curY = cmd->viewangles.y;

	if ((curY >= 0 && oldY >= 0) || (curY <= 0 && oldY <= 0))
		deltaY = abs((abs(curY) - abs(oldY)));
	else
	{
		deltaY = abs((180 - oldY) - (180 - curY));
	}

	rad = DEG2RAD(deltaY);

	cosD = std::cos(rad);
	
	sinD = std::sin(rad);*/

	CBasePlayer* pLocal = GetLocal();

	Vector origin;

	origin = pLocal->GetBonePosition(8); //fix pattern scanner and some other grabgage

	*g_Vars.pCmd = *cmd;

	*cmd = *g_Vars.pCmd;

	return bReturn;
}

template <typename T>
void CHooks::Hook(void* pInterface, void* Fn, T& Og, int id, int size) //fucking stupid stored table, barely works
{
	DWORD* dwCurrVTable = I.GetVTable(pInterface);
	std::vector<DWORD> StoredVTable(size, NULL);
	DWORD oldProtection;

	std::memcpy(StoredVTable.data(), dwCurrVTable, size * sizeof(DWORD));

	dwCurrVTable[id] = reinterpret_cast<DWORD>(Fn);

	Og = reinterpret_cast<T>(StoredVTable.at(id));

	if (H.m_VirtualData.tableMap.find(dwCurrVTable) == H.m_VirtualData.tableMap.end())
	{
		H.m_VirtualData.tableMap[dwCurrVTable] = [dwCurrVTable, StoredVTable, size]() -> void {
			DWORD oldProtection;
			std::memcpy(dwCurrVTable, StoredVTable.data(), size * sizeof(DWORD));
		};
	}
}

void CHooks::SetupHooks()
{
	Hook<PaintTraverseFn>(I.g_pPanel, HookedPaintTraverse, H.m_VirtualData.OGPaintTraverse, PAINTTRAVERSE_ID, PANEL_VTABLE_SIZE);
	Hook<CreateMoveFn>(I.g_pClientMode, HookedCreateMove, H.m_VirtualData.OGCreateMove, CLIENTMODE_ID, CLIENTMODE_VTABLE_SIZE);
}

void CHooks::UnhookAll()
{
	for (auto i : H.m_VirtualData.tableMap)
	{
		i.second();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //needed or this stupid mfer crashes
}