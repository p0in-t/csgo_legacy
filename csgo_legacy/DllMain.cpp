#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>
#include "Interfaces.h"
#include "Hooks.h"

VOID WINAPI Exit(HMODULE hMain) 
{
	FreeLibraryAndExitThread(hMain, 0);
}

VOID SetupFonts()
{
	g_Vars.fontCourierNew = I.g_pSurface->CreateFont();
	I.g_pSurface->SetFontGlyphSet(g_Vars.fontCourierNew, "Courier New", 30, 500, 0, 0, 0);

	g_Vars.espFont = I.g_pSurface->CreateFont();
	I.g_pSurface->SetFontGlyphSet(g_Vars.espFont, "Courier New", 16, 1000, 0, 0, FONTFLAG_ANTIALIAS);
}

INT WINAPI main() 
{
	g_Vars.hClient = GetModuleHandle("client.dll");
	g_Vars.hEngine = GetModuleHandle("engine.dll");

	if (g_Vars.hClient == NULL)
	{
		Exit(g_Vars.hMain);
	}

	if (g_Vars.hEngine == NULL)
	{
		Exit(g_Vars.hMain); 
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	g_Vars.logs.open("logs.txt");

	g_Vars.logs << "\n\nInjected at " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "\n\n";

	g_Pattern.Initialize();

	I.GetInterfaces();

	g_Netvar.Initialize();

	H.SetupHooks();

	SetupFonts();

	while (!I.g_pInputSystem->IsButtonDown(KEY_END)) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	Exit(g_Vars.hMain);

	return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpvReserved) 
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);

		g_Vars.hMain = hModule;

		HANDLE handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, hModule, NULL, NULL);

		if (handle)
			CloseHandle(handle);
	} break;
	case DLL_PROCESS_DETACH:
	{
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);

		H.UnhookAll();

		g_Vars.logs << "\n\nUninjected at " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "\n";

		g_Vars.logs.close();
	} break;
	}

	return TRUE;
}