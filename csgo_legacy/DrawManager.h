#pragma once
#include <Windows.h>
#include <string>
#include "Color.h"
#include "Interfaces.h"

class CDrawManager
{
public:
	const wchar_t* ToWCHAR(const char* text);
	void FastDrawText(std::string text, int x, int y, HFont font, Color* clr, bool bCentered);
	void FastDrawText(std::string text, int x, int y, HFont font, int r, int g, int b, int a, bool bCentered);
	void DrawFilledRect(int x0, int y0, int x1, int y1, int r, int g, int b, int a);
	bool ClipTransform(const Vector& point, Vector* pClip);
	bool ScreenTransform(const Vector& point, Vector* pScreen);
};

extern CDrawManager* g_pDraw;