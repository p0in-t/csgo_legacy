#pragma once
#include "Includes.h"
#include "Color.h"

typedef unsigned long HFont;

enum VGuiPanel_t
{
	PANEL_ROOT = 0,
	PANEL_GAMEUIDLL,  // the console, game menu
	PANEL_CLIENTDLL,
	PANEL_TOOLS,
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS,
	PANEL_GAMEUIBACKGROUND, // the console background, shows under all other stuff in 3d engine view
	PANEL_TRANSITIONEFFECT,
	PANEL_STEAMOVERLAY,
};

// In-game panels are cropped to the current engine viewport size
enum PaintMode_t
{
	PAINT_UIPANELS = (1 << 0),
	PAINT_INGAMEPANELS = (1 << 1),
};

abstract_class ISurface //implementation in CMatSystemSurface , fucking cancer
{
public:
	// call to Shutdown surface; surface can no longer be used after this is called
	virtual void Connect() = 0;
	virtual void Disconnect() = 0;
	virtual void QueryInterface(int a1, int a2, const char* a3) = 0;
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void GetDependencies() = 0;
	virtual void Unknown1() = 0;
	virtual void Unknown2() = 0;
	virtual void Unknown3() = 0;
	virtual void RunFrame() = 0;
	virtual void GetEmbeddedPanel() = 0;
	virtual void SetEmbeddedPanel() = 0;

	// drawing context
	virtual void PushMakeCurrent(void* panel, bool useInsets) = 0;
	virtual void PopMakeCurrent(void* panel) = 0;

	// rendering functions
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetColor(Color col) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawFilledRectArray(void* pRects, int numRects) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int* px, int* py, int numPoints) = 0;
	virtual void DrawSetApparentDepth(float depth) = 0;
	virtual void DrawClearApparentDepth() = 0;
	virtual void DrawSetTextFont(HFont font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextColor(Color col) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int& x, int& y) = 0;
	virtual void DrawPrintText(const wchar_t* text, int textLen, void* drawType = 0) = 0;
	virtual void DrawUnicodeChar(wchar_t wch, void* drawType = 0) = 0;
	virtual void DrawFlushText() = 0;		// flushes any buffered text (for rendering optimizations)
	virtual void*CreateHTMLWindow(void* events, void* context) = 0;
	virtual void PaintHTMLWindow(void* htmlwin) = 0;
	virtual void DeleteHTMLWindow(void* htmlwin) = 0;

	enum ETextureFormat
	{
		eTextureFormat_RGBA,
		eTextureFormat_BGRA,
		eTextureFormat_BGRA_Opaque, // bgra format but alpha is always 255, CEF does this, we can use this fact for better perf on win32 gdi
	};

	virtual void CreateNewTextureID(void* htmlwin) = 0;
	virtual bool DrawGetTextureFile(int id, char* filename, int maxlen) = 0;
	virtual void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char* rgba, int wide, int tall) = 0;
	virtual void DrawSetTexture(int id) = 0;
	virtual bool DeleteTextureByID(int id) = 0;
	virtual void DrawGetTextureSize(int id, int& wide, int& tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool IsTextureIDValid(int id) = 0;
	virtual int  Unknown4() = 0;
	virtual void GetScreenSize(int& wide, int& tall) = 0;
	virtual void Unknown5() = 0;
	virtual void BringToFront(void* panel) = 0;
	virtual void SetForegroundWindow(void* panel) = 0;
	virtual void Unknown6() = 0;
	virtual void SetMinimized(void* panel, bool state) = 0;
	virtual bool IsMinimized(void* panel) = 0;
	virtual void Unknown7() = 0;
	virtual void SetTitle(void* panel, const wchar_t* title) = 0;
	virtual void Unknown8() = 0;

	// windows stuff
	virtual void CreatePopup(void* panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
	virtual void Unknown9() = 0;
	virtual void Unknown10() = 0;
	virtual void SetCursor(void* cursor) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void Unknown11() = 0;
	virtual bool IsWithin(int x, int y) = 0;
	virtual void Unknown12() = 0;
	virtual bool SupportsFeature(void* feature) = 0;

	// restricts what gets drawn to one panel and it's children
	// currently only works in the game
	virtual void RestrictPaintToSinglePanel(void* panel, bool bForceAllowNonModalSurface = false) = 0;

	// these two functions obselete, use IInput::SetAppModalSurface() instead
	virtual void Unknown13() = 0;
	virtual void Unknown14() = 0;
	virtual void UnlockCursor() = 0;
	virtual void LockCursor() = 0;
	virtual void Unknown15() = 0;
	virtual void GetTopmostPopup() = 0;
	virtual void Unknown16() = 0;
	// fonts
	// creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
	virtual HFont CreateFont() = 0;

	virtual bool SetFontGlyphSet(HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0) = 0;

	// adds a custom font file (only supports true type font files (.ttf) for now)
	virtual bool AddCustomFontFile(const char* fontFileName) = 0;

	// returns the details about the font
	virtual int GetFontTall(HFont font) = 0;
	virtual int GetFontAscent(HFont font, wchar_t wch) = 0;
	virtual bool IsFontAdditive(HFont font) = 0;
	virtual void GetCharABCwide(HFont font, int ch, int& a, int& b, int& c) = 0;
	virtual int GetCharacterWidth(HFont font, int ch) = 0;
	virtual void GetTextSize(HFont font, const wchar_t* text, int& wide, int& tall) = 0;

	// notify icons?!?
	virtual void* Unknown17() = 0;
	virtual void Unknown18() = 0;

	// plays a sound
	virtual void PlaySound(const char* fileName) = 0;

	//!! these functions should not be accessed directly, but only through other vgui items
	//!! need to move these to seperate interface
	virtual int GetPopupCount() = 0;
	virtual void* GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(void* childPanel) = 0;
	virtual void Unknown19() = 0;
	virtual void AddPanel(void* panel) = 0;
	virtual void ReleasePanel(void* panel) = 0;
	virtual void MovePopupToFront(void* panel) = 0;
	virtual void MovePopupToBack(void* panel) = 0;
	virtual void SolveTraverse(void* panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(std::uint32_t panel) = 0;
	virtual void EnableMouseCapture(void* panel, bool state) = 0;

	// returns the size of the workspace
	virtual void GetWorkspaceBounds(int& x, int& y, int& wide, int& tall) = 0;

	// gets the absolute coordinates of the screen (in windows space)
	virtual void GetAbsoluteWindowBounds(int& x, int& y, int& wide, int& tall) = 0;

	// gets the base resolution used in proportional mode
	virtual void Unknown20() = 0;
	virtual void CalculateMouseVisible() = 0;
	virtual bool NeedKBInput() = 0;
	virtual void Unknown21() = 0;
	virtual void SurfaceGetCursorPos(int& x, int& y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;

	// SRC only functions!!!
	virtual void DrawTexturedLine(const void*& a, const void*& b) = 0;
	virtual void Unknown22() = 0;
	virtual void Unknown23() = 0;
	virtual void Unknown24() = 0;
	virtual void Unknown25() = 0;
	virtual void Unknown26() = 0;
	virtual void Unknown27() = 0;
	virtual void Unknown28() = 0;
	virtual void Unknown29() = 0;
	virtual void Unknown30() = 0;
	virtual void Unknown31() = 0;
	virtual void Unknown32() = 0;
	virtual void Unknown33() = 0;
	virtual void Unknown34() = 0;

	// video mode changing
	virtual void OnScreenSizeChanged(int nOldWidth, int nOldHeight) = 0;
	virtual void Unknown35() = 0;
	virtual void Unknown36() = 0;

	// create IVguiMatInfo object ( IMaterial wrapper in VguiMatSurface, NULL in CWin32Surface )
	virtual void PaintTraverseEx(void* panel, bool paintPopups = false) = 0;
	virtual void Unknown37() = 0;

	// From the Xbox
	virtual void SetPanelForInput(void* vpanel) = 0;
	virtual void Unknown38() = 0;
	virtual void Unknown39() = 0;
	virtual void Unknown40() = 0;
	virtual void Unknown41() = 0;
	virtual void Unknown42() = 0;

	// adds a bitmap font file
	virtual bool AddBitmapFontFile(const char* fontFileName) = 0;
	virtual void Unknown44() = 0;
	virtual void Unknown45() = 0;
	virtual void Unknown46() = 0;
	virtual void Unknown47() = 0;
	virtual void Unknown48() = 0;
	virtual void Unknown49() = 0;
	virtual void Unknown50() = 0;
	virtual void Unknown51() = 0;
	virtual void Unknown52() = 0;
	virtual void Unknown53() = 0;
	virtual void Unknown54() = 0;
	virtual void Unknown55() = 0;
	virtual void Unknown56() = 0;
	virtual void Unknown57() = 0;
	virtual void Unknown58() = 0;
	virtual void Unknown59() = 0;
	virtual void Unknown60() = 0;
	virtual void Unknown61() = 0;
	virtual void Unknown62() = 0;
	virtual void Unknown63() = 0;
	virtual void Unknown64() = 0;
	virtual void Unknown65() = 0;
	virtual void Unknown66() = 0;
	virtual void Unknown67() = 0;
	virtual void Unknown68() = 0;
	virtual void Unknown69() = 0;
	virtual void Unknown70() = 0;
	virtual void Unknown71() = 0;
	virtual void Unknown72() = 0;
	virtual void Unknown73() = 0;
	virtual void Unknown74() = 0;
	virtual void Unknown75() = 0;
	virtual void Unknown76() = 0;
	virtual void Unknown77() = 0;
	virtual void Unknown78() = 0;
	virtual void Unknown79() = 0;
	virtual void Unknown80() = 0;
	virtual void Unknown81() = 0;
	virtual void Unknown82() = 0;
	virtual void Unknown83() = 0;
	virtual void Unknown84() = 0;
	virtual void Unknown85() = 0;
	virtual void Unknown86() = 0;
	virtual void Unknown87() = 0;
	virtual void Unknown88() = 0;
	virtual void Unknown89() = 0;
	virtual void Unknown90() = 0;
	virtual void Unknown91() = 0;
	virtual void Unknown92() = 0;
	virtual void Unknown93() = 0;
};