#include "VMTHooks.h"
#include "Panels.h"
#include "CDraw.h"
#include "Netvars.h"
#include "Entity.h"
#include "Controls.h"

#include "ESP.h"
#include "Menu.h"
#include "Misc.h"

#include <string>

#include "Client.h"
#include "Cache.h"

#include "Engine.h"	
#include "R8_inetchannel.h"

CScreenSize gScreenSize;
#include "Aim.h"
#include "Ray tracing.h"
//===================================================================================
void __fastcall Hooked_PaintTraverse(void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	try
	{
		const char* szName = gBase.Panels->GetName(vguiPanel);
		static unsigned int FocusOverlay, HudScope;

		if (!FocusOverlay)
		{
			if( szName[0] == 'F' && szName[5] == 'O' && szName[12] == 'P' )
			{
				FocusOverlay = vguiPanel;
				Intro();
			}
		}
		else
			gBase.Panels->SetTopmostPopup(FocusOverlay, true);
		if (!HudScope && szName[8] == NULL && szName[0] == 'H' && szName[3] == 'S' && szName[7] == 'e')
			HudScope = vguiPanel;

		if (vguiPanel == HudScope && (gMisc.noscope.value == 1 || gMisc.noscope.value == 3))
			return;

		VMTManager& hook = VMTManager::GetHook(pPanels); //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall*)(void*, unsigned int, bool, bool)>((int)e_offset::PaintTraverse)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.
		
		if (FocusOverlay != vguiPanel || gBase.Engine->IsDrawingLoadingImage())
			return;

		CScreenSize newSize;
		gBase.Engine->GetScreenSize(newSize.width, newSize.height);
		if (newSize.width != gScreenSize.width || newSize.height != gScreenSize.height)
			gDraw.Reload(); // Reload stuff like fonts and screen size to prevent resolution change/fullscreen issues

		gMenu.GetInput();

		CPlayer* you = nullptr;
		if (gBase.Engine->IsInGame() && GetBaseEntity(me)->ToPlayer(you))
		{
			// Draw all players
			for (int i = 1; i <= gBase.Engine->GetMaxClients(); i++)
			{
				CPlayer* player;
				if (!GetBaseEntity(i)->ToPlayer(player))
					continue;

				gEsp.DrawPlayer(you, player, i);
			}
			// Draw all other entities
			for (int i = gBase.Engine->GetMaxClients() + 1; i <= gBase.EntList->GetHighestEntityIndex(); i++)
			{
				CBaseEntity* ent = GetBaseEntity(i);
				if (!ent || ent->IsDormant())
					continue;

				gEsp.DrawEntity(you, ent, i);
			}
		}

		CBaseCheat::RunDraw(you);

		gAim.backtrack.max = gAim.smarttrack.value > 1 ? TIME_TO_TICKS(0.2f) : MAX_CACHE - 1;

		gMenu.Draw();
		gMenu.EndInput();
		gBase.Panels->SetMouseInputEnabled(vguiPanel, gMenu.enabled);
	}
	catch(...)
	{
		CATCH_WARN();
	}
}
//===================================================================================
/*#include "MemoryModule.h"

inline void* GetResource(DWORD lpDefine, LPCSTR lpType, size_t* size = nullptr)
{
	HRSRC hResInst = FindResourceA(gMenu.hInst, MAKEINTRESOURCEA(lpDefine), lpType);
	HGLOBAL hGlobal = LoadResource(gMenu.hInst, hResInst);

	if (size != nullptr)
		*size = SizeofResource(gMenu.hInst, hResInst);

	return LockResource(hGlobal);
}*/

#include "CMat.h"
//#include "R8_ITexture.h"
void Intro(void)
{
	gDraw.Init();
	InitTextures();

	try
	{
		nEntity.init();
		nPlayer.init();
		nWeapon.init();
		nObject.init();
	}
	catch (...)
	{
		MessageBox(NULL, "Failed to initialize netvars!", "Fatal error", MB_OK);
	}

	try
	{
		gMat.Initialize();
		/*IMaterialSystem* matSys = (IMaterialSystem*)gBase.MatSystem;
		matSys->forceBeginRenderTargetAllocation();
		gMisc.mirrorTexture = matSys->createFullFrameRenderTarget("mirrorcam_rt");
		matSys->forceEndRenderTargetAllocation();*/
	}
	catch (...)
	{
		MessageBox(NULL, "Failed to initialize materials!", "Fatal error", MB_OK);
	}

	gMenu.CreateGUI();
}