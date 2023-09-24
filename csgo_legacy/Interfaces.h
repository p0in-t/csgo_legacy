#pragma once
#include "IVEngineClient.h"
#include "ISurface.h"
#include "GlobalVars.h"
#include "IPanel.h"
#include "IBaseClientDLL.h"
#include "IClientMode.h"
#include "IClientEntityList.h"
#include "CPlayerResource.h"
#include "IVDebugOverlay.h"
#include "IInputSystem.h"
#include "IMemAlloc.h"
#include "CPatternScanner.h"

#define CREATEINTERFACE_PROCNAME	            "CreateInterface"
#define VENGINE_CLIENT_RANDOM_INTERFACE_VERSION	"VEngineRandom001"
#define VENGINE_CLIENT_INTERFACE_VERSION		"VEngineClient014"
#define VENGINE_CLIENT_INTERFACE_VERSION_13		"VEngineClient013"
#define VGUI_SURFACE_INTERFACE_VERSION			"VGUI_Surface031"
#define VGUI_PANEL_INTERFACE_VERSION			"VGUI_Panel009"
#define CLIENT_DLL_INTERFACE_VERSION			"VClient018"
#define VCLIENTENTITYLIST_INTERFACE_VERSION		"VClientEntityList003"
#define VDEBUG_OVERLAY_INTERFACE_VERSION		"VDebugOverlay004"
#define INPUTSYSTEM_INTERFACE_VERSION			"InputSystemVersion001"

class CInterfaces
{
public:
	IBaseClientDLL*		g_pBaseClientDLL		= NULL;
	IVEngineClient*		g_pEngineClient			= NULL;
	ISurface*			g_pSurface				= NULL;
	IPanel*				g_pPanel				= NULL;
	IClientMode*		g_pClientMode			= NULL;
	IClientEntityList*  g_pClientEntityList		= NULL;
	C_PlayerResource*	g_pPlayerResource		= NULL;
	IVDebugOverlay*		g_pDebugOverlay			= NULL;
	IInputSystem*		g_pInputSystem			= NULL;
	IMemAlloc*			g_pMemAlloc				= NULL;
	CGlobalVarsBase*	g_pGlobals				= NULL;

	DWORD* pVTable(void* pThis);

	DWORD* GetVTable(void* pThis);

	template <typename T>
	T GetVFunc(void* pThis, int iIndex);

	IClientMode* GetClientMode();

	C_PlayerResource* GetResource();

	CreateInterfaceFn Sys_GetFactory(HMODULE* pModule);

	IMemAlloc* GetMemAlloc();

	void GetInterfaces();
};

extern CInterfaces I;