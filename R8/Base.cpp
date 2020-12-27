#include "Base.h"
#include "Cheat.h"
#include "VMTHooks.h"
#include "CSignature.h"
#include "IGameEvents.h"
#include "memalloc.h"

CInterfaces gBase;

bool CInterfaces::GetInterfaces()
{
	CreateInterface_t ClientFactory			("client.dll");
	CreateInterface_t EngineFactory			("engine.dll");
	CreateInterface_t VGUIFactory			("vguimatsurface.dll");
	CreateInterface_t ServerFactory			("server.dll");
	CreateInterface_t CVarFactory			("vstdlib.dll");
	CreateInterface_t MaterialSystemFactory	("MaterialSystem.dll");
	CreateInterface_t VGUI2Factory			("vgui2.dll");

	PlayerInfoManager = (IPlayerInfoManager*)ServerFactory		("PlayerInfoManager002");
	GameEventManager =	(IGameEventManager2*)EngineFactory		("GAMEEVENTSMANAGER002");
	ModelRender =		(CModelRender*)EngineFactory			("VEngineModel016");
	EngineTrace =		(IEngineTrace*)EngineFactory			("EngineTraceClient003");
	RenderView =		(CRenderView*)EngineFactory				("VEngineRenderView014");
	Prediction =		(CPrediction*)ClientFactory				("VClientPrediction001");
	MatSystem =			(CMaterialSystem*)MaterialSystemFactory	("VMaterialSystem081");
	ModelInfo =			(IVModelInfo*)EngineFactory				("VModelInfoClient006");
	Surface =			(ISurface*)VGUIFactory					("VGUI_Surface030");
	EntList =			(CEntList*)ClientFactory				("VClientEntityList003");
	Engine =			(EngineClient*)EngineFactory			("VEngineClient013");
	Panels =			(IPanel*)VGUI2Factory					("VGUI_Panel009");
	Client =			(CHLClient*)ClientFactory				("VClient017");
	Cvar =				(ICvar*)CVarFactory						("VEngineCvar004");

	/*HINSTANCE tier0;
	if (!(tier0 = GetModuleHandle("tier0.dll")) || !(g_pMemAlloc = (IMemAlloc*)GetProcAddress(tier0, "g_pMemAlloc")))
		gBase.Fatal("Fatal error", "Failed to import \"g_pMemAlloc\" from \"tier0.dll\"");*/

	VMTBaseManager chl(Client);
	DWORD input = gSignatures.dwFindPattern(
		(DWORD)chl.GetMethod<DWORD>((int)e_offset::CreateMove),
		(DWORD)chl.GetMethod<DWORD>((int)e_offset::CreateMove) + 0x100,
		"8B 0D");

	DWORD globalsScan = gSignatures.GetClientSignature("A3 ? ? ? ? 8D 45 08 6A 01 50 E8");
	DWORD clientMode = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
	DWORD clientState = gSignatures.GetEngineSignature("6A 01 B9 ? ? ? ? E8 ? ? ? ? 50 B9");

	if (!globalsScan || !clientMode || !clientState || !input)
	{
		CHECKPTR(globalsScan); // This will do the crash message for us
		return false; // You'll need this if you plan to do crash message elsewhere
	}

	gBase.Input = **(CInput***)(input + 2);
	gBase.Globals = **(CGlobals***)(globalsScan + 1);
	gBase.ClientMode = **(ClientModeShared***)(clientMode + 2);
	gBase.ClientState = *(CClientState**)(clientState + 3);

	return true;
}

#include "Menu.h"
#include "Panels.h"
#include "Client.h"
#include "FrameStageNotify.h"
#include "DrawModelExecute.h"

void CInterfaces::HookInterfaces()
{
	VMTBaseManager* panelHook = new VMTBaseManager(gBase.Panels);
	panelHook->HookMethod(&Hooked_PaintTraverse, e_offset::PaintTraverse);
	panelHook->Rehook();

	VMTBaseManager* clientModeHook = new VMTBaseManager(gBase.ClientMode);
	clientModeHook->HookMethod(&Hooked_CreateMove, e_offset::CreateMove);
	clientModeHook->Rehook();

	VMTBaseManager* chlHook = new VMTBaseManager(gBase.Client);
	//chlHook->HookMethod(&Hooked_CHLCreateMove, e_offset::CreateMove);
	//chlHook->HookMethod(&Hooked_WriteUserCmdDeltaToBuffer, e_offset::WriteUserCmdToBuffer);
	chlHook->HookMethod(&Hooked_FrameStageNotify, e_offset::FrameStageNotify);
	chlHook->Rehook();

	VMTBaseManager* modelHook = new VMTBaseManager(gBase.ModelRender);
	modelHook->HookMethod(&Hooked_DrawModelExecute, e_offset::DrawModelExecute);
	modelHook->Rehook();

	HWND thisWindow;
	while (!(thisWindow = FindWindow("Valve001", NULL)))
		Sleep(500);
	gMenu.windowProc = (WNDPROC)SetWindowLongPtr(thisWindow, GWLP_WNDPROC, (LONG_PTR)&Hooked_WndProc);
}

#include "Engine.h"
CGlowManager* pGlowManager;
COffsets gOffsets;
void COffsets::GetOffsets()
{
	// Add signatures here
	CHECKPTR(EstimateAbsVelocity = gSignatures.GetClientSignature(
		"E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07"));
	CHECKPTR(SetAbsAngles = gSignatures.GetClientSignature("55 8B EC 83 EC 60 56 57 8B F1"));
	CHECKPTR(ForceFullUpdate = gSignatures.GetEngineSignature("56 8B F1 83 BE ? ? ? ? ? 74 1D"));
	CHECKPTR(KeyValues = gSignatures.GetEngineSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6"));
	CHECKPTR(LoadFromBuffer = gSignatures.GetEngineSignature("55 8B EC 83 EC 38 53 8B 5D 0C"));
	CHECKPTR(g_pGlowManager = gSignatures.GetClientSignature("B9 ? ? ? ? E8 ? ? ? ? B0 01 5D"));
	CHECKPTR(dwWriteUserCmd = gSignatures.GetClientSignature("55 8B EC 83 EC 08 53 8B 5D 0C 56 8B 75 10"));

	// Offset your offsets here :D

	KeyValues -= 0x42;
	pGlowManager = *(CGlowManager**)(g_pGlowManager += 0x1);
	EstimateAbsVelocity = ((*(PDWORD)(EstimateAbsVelocity + 1)) + EstimateAbsVelocity + 5);
}

CreateInterface_t::CreateInterface_t(const char* szModule)
{
	instance = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(szModule), "CreateInterface");
	if (!instance)
	{
		MessageBox(NULL, "Failed to get a CreateInterface handle!\nThe program will now exit.", "Fatal error", MB_OK);
		exit(0);
	}
}
void* CreateInterface_t::operator()(const char* pName, int* pReturnCode)
{
	void* result = instance(pName, pReturnCode);
	if (!result)
	{
		MessageBox(NULL, string((string)"Failed to get '" + pName + "'\nThe program will now exit").c_str(), "Fatal error", MB_OK);
		exit(0);
	}

	return result;
}

void CInterfaces::Fatal(const char* szTitle, const char* szMessage, ...)
{
	va_list va_alist;
	char szBuffer[256] = { 0 };

	va_start(va_alist, szMessage);
	vsprintf_s(szBuffer, szMessage, va_alist);
	va_end(va_alist);

	MessageBox(NULL, szBuffer, szTitle, MB_OK);
	exit(0);
}

void CInterfaces::OnException(const char* szFunction)
{
	va_list va_alist;
	char szBuffer[256] = { 0 };
	sprintf_s(szBuffer, "Failed %s", szFunction);

	MessageBox(NULL, szBuffer, "Warning", MB_OK);
}
