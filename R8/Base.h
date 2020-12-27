#pragma once
#include <Windows.h>
#include <string>
using namespace std;

#include "Vector.h"
#include "Vector2.h"
#include "Offsets.h"

#define WIN32_LEAN_AND_MEAN
#pragma optimize("gsy",on)
#pragma warning( disable : 4244 ) //Possible loss of data

#define PI 3.14159265358979323846f

#define RADPI 57.295779513082f

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
class CreateInterface_t
{
public:
	CreateInterfaceFn instance;
	CreateInterface_t(const char* szDll);
	void* operator()(const char* pName, int* pReturnCode = nullptr);
};

inline wstring ToWchar(const char* text)
{
	size_t length = strlen(text) + 1;
	wstring wc(length, L'#');
	mbstowcs(&wc[0], text, length);

	return wc;
}

inline string ToChar(const wchar_t* text)
{
	size_t length = wcslen(text) + 1;
	string str(length, L'#');
	wcstombs(&str[0], text, length);

	return str;
}

inline string FloatStr(float input)
{
	string text = to_string(input);
	int last0 = text.find_last_not_of('0'), dot = text.find_first_of('.');

	return text.substr(0, (last0 != string::npos && last0 > dot) ? last0 + 1 : dot);
}

// Forward declaring common classes to reduce the need for extra headers

class CBaseEntity;
class CPlayer;
class CUserCmd;

// Forward declaring all interface classes for use in separate headers

class ICvar;
class IPanel;
class CInput;
class ISurface;
class CGlobals;
class CEntList;
class CHLClient;
class CRenderView;
class CPrediction;
class IVModelInfo;
class CModelRender;
class IEngineTrace;
class CClientState;
class EngineClient;
class CMaterialSystem;
class ClientModeShared;
class IGameEventManager2;
class IPlayerInfoManager;

class IMemAlloc;

#define CATCH_WARN() gBase.OnException(__FUNCTION__)
class CInterfaces
{
public:
	ICvar*				Cvar;
	IPanel*				Panels;
	CInput*				Input;
	ISurface*			Surface;
	CGlobals*			Globals;
	CEntList*			EntList;
	CHLClient*			Client;
	CRenderView*		RenderView;
	CPrediction*		Prediction;
	IVModelInfo*		ModelInfo;
	CModelRender*		ModelRender;
	IEngineTrace*		EngineTrace;
	CClientState*		ClientState;
	EngineClient*		Engine;
	CMaterialSystem*	MatSystem;
	ClientModeShared*	ClientMode;
	IGameEventManager2*	GameEventManager;
	IPlayerInfoManager*	PlayerInfoManager;

	IMemAlloc* g_pMemAlloc;

	bool GetInterfaces();
	void HookInterfaces();
	void Fatal(const char* szTitle, const char* szMessage, ...);
	void OnException(const char* szFunction);
};
extern CInterfaces gBase;