#include "Interfaces.h"

CInterfaces I;

DWORD* CInterfaces::GetVTable(void* pThis)
{
	DWORD** pVTable = (DWORD**)(pThis);
	DWORD* VTable   = *pVTable;

	return VTable;
}

template <typename T>
T CInterfaces::GetVFunc(void* pThis, int iIndex)
{
	DWORD** pVTable = (DWORD**)(pThis);
	DWORD* VTable   = *pVTable;

	return reinterpret_cast<T>(VTable[iIndex]);
}

CreateInterfaceFn CInterfaces::Sys_GetFactory(HMODULE* pModule)
{
	if (!pModule)
		return NULL;

	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(*pModule, CREATEINTERFACE_PROCNAME));
}

IMemAlloc* CInterfaces::GetMemAlloc()
{
	HMODULE hTier0 = GetModuleHandle("tier0.dll");

	if (!hTier0)
		return NULL;

	return reinterpret_cast<IMemAlloc*>(GetProcAddress(hTier0, "g_pMemAlloc")); //exported
}

//B7B

IClientMode* CInterfaces::GetClientMode()
{
	DWORD* pBaseClientDLLVTable = GetVTable(g_pBaseClientDLL);

	IClientMode* pClientMode = **reinterpret_cast<IClientMode***>( pBaseClientDLLVTable[10] + 5 );

	return pClientMode;
}

//C_PlayerResource* CInterfaces::GetResource()
//{
//	DWORD* pClientModeVTable = GetVTable(g_pClientMode);
//
//	C_PlayerResource* pPlayerResource = **reinterpret_cast<C_PlayerResource***>(pClientModeVTable[10] + 0xB7B + 2);
//
//	return pPlayerResource;
//}

void CInterfaces::GetInterfaces()
{
	HMODULE hVGui			= GetModuleHandle("vguimatsurface.dll");
	HMODULE hVGui2			= GetModuleHandle("vgui2.dll");
	HMODULE hInputSystem	= GetModuleHandle("inputsystem.dll");

	CreateInterfaceFn clientSystemFactory = Sys_GetFactory(&g_Vars.hClient);
	CreateInterfaceFn engineSystemFactory = Sys_GetFactory(&g_Vars.hEngine);
	CreateInterfaceFn vguiSystemFactory   = Sys_GetFactory(&hVGui);
	CreateInterfaceFn vgui2SystemFactory  = Sys_GetFactory(&hVGui2);
	CreateInterfaceFn inputSystemFactory  = Sys_GetFactory(&hInputSystem);

	g_pBaseClientDLL	= (IBaseClientDLL*)clientSystemFactory(CLIENT_DLL_INTERFACE_VERSION, NULL);
	g_pEngineClient		= (IVEngineClient*)engineSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION, NULL);
	g_pSurface			= (ISurface*)vguiSystemFactory(VGUI_SURFACE_INTERFACE_VERSION, NULL);
	g_pPanel			= (IPanel*)vgui2SystemFactory(VGUI_PANEL_INTERFACE_VERSION, NULL);
	g_pClientEntityList = (IClientEntityList*)clientSystemFactory(VCLIENTENTITYLIST_INTERFACE_VERSION, NULL);
	g_pClientMode		= GetClientMode();
	g_pDebugOverlay		= (IVDebugOverlay*)engineSystemFactory(VDEBUG_OVERLAY_INTERFACE_VERSION, NULL);
	g_pInputSystem	    = (IInputSystem*)inputSystemFactory(INPUTSYSTEM_INTERFACE_VERSION, NULL);
	g_pMemAlloc			= GetMemAlloc();
	g_pGlobals			= **reinterpret_cast<CGlobalVarsBase***>(g_Pattern.GetGlobalVarsBase() + 1);	
}