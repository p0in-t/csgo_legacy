#include "DrawManager.h"

const wchar_t* CDrawManager::ToWCHAR(const char* text)
{
	int wcharsz = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
	wchar_t* wc = new wchar_t[wcharsz];
	MultiByteToWideChar(CP_UTF8, 0, text, -1, wc, wcharsz);

	return wc;
}

void CDrawManager::FastDrawText(std::string text, int x, int y, HFont font, Color* clr, bool bCentered)
{
	if (clr == NULL)
		clr = new Color(255, 255, 255, 255);

	int wcharsz = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
	wchar_t* wc = new wchar_t[wcharsz];
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wc, wcharsz);
	int w = 0, h = 0;

	if (bCentered)
	{
		I.g_pSurface->GetTextSize(g_Vars.espFont, wc, w, h);
	}

	I.g_pSurface->DrawSetTextPos(x - w / 2, y - h / 2);
	I.g_pSurface->DrawSetTextColor(clr->r(), clr->g(), clr->b(), clr->a());
	I.g_pSurface->DrawSetTextFont(font);
	I.g_pSurface->DrawPrintText(wc, wcslen(wc));

	delete clr;
	delete[] wc;
}

void CDrawManager::FastDrawText(std::string text, int x, int y, HFont font, int r, int g, int b, int a, bool bCentered)
{
	int wcharsz = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
	wchar_t* wc = new wchar_t[wcharsz];
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wc, wcharsz);
	int w = 0, h = 0;

	if (bCentered)
	{
		I.g_pSurface->GetTextSize(g_Vars.espFont, wc, w, h);
	}

	I.g_pSurface->DrawSetTextPos(x - w / 2, y - h / 2);
	I.g_pSurface->DrawSetTextColor(r, g, b, a);
	I.g_pSurface->DrawSetTextFont(font);
	I.g_pSurface->DrawPrintText(wc, wcslen(wc));

	delete[] wc;
}

void CDrawManager::DrawFilledRect(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	I.g_pSurface->DrawSetColor(r, g, b, a);
	I.g_pSurface->DrawFilledRect(x0, y0, x1, y1);
}

bool CDrawManager::ClipTransform(const Vector& point, Vector* pClip)
{
	// UNDONE: Clean this up some, handle off-screen vertices
	float w;
	const VMatrix& worldToScreen = I.g_pEngineClient->WorldToScreenMatrix();

	pClip->x = worldToScreen[0][0] * point[0] + worldToScreen[0][1] * point[1] + worldToScreen[0][2] * point[2] + worldToScreen[0][3];
	pClip->y = worldToScreen[1][0] * point[0] + worldToScreen[1][1] * point[1] + worldToScreen[1][2] * point[2] + worldToScreen[1][3];
	//	z		 = worldToScreen[2][0] * point[0] + worldToScreen[2][1] * point[1] + worldToScreen[2][2] * point[2] + worldToScreen[2][3];
	w = worldToScreen[3][0] * point[0] + worldToScreen[3][1] * point[1] + worldToScreen[3][2] * point[2] + worldToScreen[3][3];

	// Just so we have something valid here
	pClip->z = 0.0f;

	bool behind;
	if (w < 0.001f)
	{
		behind = true;
		pClip->x *= 100000;
		pClip->y *= 100000;
	}
	else
	{
		behind = false;
		float invw = 1.0f / w;
		pClip->x *= invw;
		pClip->y *= invw;
	}

	return behind;
}

bool CDrawManager::ScreenTransform(const Vector& point, Vector* pScreen)
{
	bool retval = ClipTransform(point, pScreen);

	int x, y, w, h;

	I.g_pSurface->GetAbsoluteWindowBounds(x, y, w, h);

	pScreen->x = 0.5f * (pScreen->x + 1.0f) * w + x;
	pScreen->y = 0.5f * (pScreen->y + 1.0f) * h + y;

	return retval;
}

CDrawManager* g_pDraw;