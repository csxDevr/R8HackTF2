#pragma once
#include "Base.h"
#include "getvfunc.h"

class IPanel
{
public:
	const char *GetName(unsigned int vguiPanel)
	{
		typedef const char* (__thiscall* OriginalFn)(void*, unsigned int);
		return getvfunc<OriginalFn>(this, 36)(this, vguiPanel);
	}
	void SetMouseInputEnabled(unsigned int panel, bool state)
	{
		getvfunc<void(__thiscall *)(void*, int, bool)>(this, 32)(this, panel, state);
	}
	void SetTopmostPopup(unsigned int panel, bool state)
	{
		getvfunc<void(__thiscall *)(void*, int, bool)>(this, 59)(this, panel, state);
	}
};

struct Vertex_t;
class ISurface
{
public:
	void DrawSetColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		getvfunc<OriginalFn>(this, 11)(this, r, g, b, a);
	}
	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		getvfunc<OriginalFn>(this, 12)(this, x0, y0, x1, y1);
	}
	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		getvfunc<OriginalFn>(this, 14)(this, x0, y0, x1, y1);
	}
	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(void*, unsigned long);
		getvfunc<OriginalFn>(this, 17)(this, font);
	}
	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int, int, int);
		getvfunc<OriginalFn>(this, 19)(this, r, g, b, a);
	}
	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* OriginalFn)(void*, int, int);
		getvfunc<OriginalFn>(this, 20)(this, x, y);
	}
	void DrawPrintText(const wchar_t *text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(void*, const wchar_t *, int, int);
		return getvfunc<OriginalFn>(this, 22)(this, text, textLen, 0);
	}
	unsigned long CreateFont()
	{
		typedef unsigned int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 66)(this);
	}
	void SetFontGlyphSet(unsigned long &font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(void*, unsigned long, const char*, int, int, int, int, int, int, int);
		getvfunc<OriginalFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}
	void GetTextSize(unsigned long font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* OriginalFn)(void*, unsigned long, const wchar_t *, int&, int&);
		getvfunc<OriginalFn>(this, 75)(this, font, text, wide, tall);
	}
	int CreateNewTextureID(bool procedural = true)
	{
		typedef int(__thiscall *OriginalFn)(void*, bool);
		return getvfunc<OriginalFn>(this, 37)(this, procedural);
	}
	void DrawSetTextureRGBA(int id, unsigned char const* rgba, int wide, int tall, int hardwareFilter = 0, bool forceReload = 0)
	{
		typedef void(__thiscall *OriginalFn)(void*, int, unsigned char const*, int, int, int, bool);
		return getvfunc<OriginalFn>(this, 31)(this, id, rgba, wide, tall, hardwareFilter, forceReload);
	}
	void DrawSetTexture(int id)
	{
		typedef void(__thiscall *OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 32)(this, id);
	}
	bool IsTextureIDValid(int id)
	{
		typedef bool(__thiscall *OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 35)(this, id);
	}
	void DrawTexturedRect(int x, int y, int width, int height)
	{
		typedef void(__thiscall *OriginalFn)(void*, int, int, int, int);
		return getvfunc<OriginalFn>(this, 34)(this, x, y, width, height);
	}
	void DrawTexturedPolygon(int n, Vertex_t *vertices, bool bClipVertices = true)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, int, Vertex_t*, bool);
		return getvfunc<OriginalFn>(this, 102)(this, n, vertices, bClipVertices);
	}
	void DrawLine(int x, int y, int x1, int y1)
	{
		typedef void(__thiscall *OriginalFn)(void*, int, int, int, int);
		return getvfunc<OriginalFn>(this, 15)(this, x, y, x1, y1);
	}
	void GetCursorPosition(int &x, int &y)
	{
		return getvfunc<void(__thiscall *)(void*, int &, int &)>(this, 96)(this, x, y);
	}
};