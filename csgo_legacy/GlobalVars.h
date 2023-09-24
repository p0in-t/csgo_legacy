#pragma once
#include <Windows.h>
#include <fstream>
#include "CUserCmd.h"

typedef unsigned long HFont;

struct Vars
{
	bool bRelease = false;

	HMODULE hMain;
	HMODULE hClient;
	HMODULE hEngine;

	CUserCmd* pCmd = new CUserCmd;
	HFont fontCourierNew;
	HFont espFont;
	std::ofstream logs;
};

extern Vars g_Vars;