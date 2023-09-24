#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <functional>
#include "Interfaces.h"
#include "NetvarManager.h"
#include "DrawManager.h"

#define PAINTTRAVERSE_ID			41
#define PANEL_VTABLE_SIZE			66
#define CLIENTMODE_ID				24
#define CLIENTMODE_VTABLE_SIZE		56

using PaintTraverseFn = void(__thiscall*)(void*, uint32_t, bool, bool);
using CreateMoveFn	  = bool(__thiscall*)(void*, float, CUserCmd* cmd);

class CHooks
{
public:
	void SetupHooks();
	void UnhookAll();

	template <typename T>
	void Hook(void* pInterface, void* Fn, T& Og, int id, int size);

	struct VirtualData_t
	{
		std::unordered_map<DWORD*, std::function<void()>> tableMap;

		PaintTraverseFn OGPaintTraverse		= NULL;
		CreateMoveFn	OGCreateMove		= NULL;
	}m_VirtualData;
};

extern CHooks H;