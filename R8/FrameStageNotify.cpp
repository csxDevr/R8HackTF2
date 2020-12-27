#include "FrameStageNotify.h"
#include "Entity.h"
#include "VMTHooks.h"

#include "Misc.h"
#include "Aim.h"
#include "HvH.h"
#include "Cache.h"
#include "NetChan.h"

bool call_original = true;
void Original_FrameStageNotify(ClientFrameStage_t curStage, bool bSet)
{
	if (bSet)
		call_original = false;
	VMTManager::GetHook(gBase.Client).GetMethod<void(__thiscall *)(PVOID, ClientFrameStage_t)>((int)e_offset::FrameStageNotify)(gBase.Client, curStage);
}

#include "VisualEffects.h"
#include "ESP.h"

void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{
	auto &hook = VMTManager::GetHook(gBase.Client);
	try
	{
		gVFX.FrameStageNotify(curStage);
		gEsp.FrameStageNotify(curStage);

		CPlayer* you;
		if (!GetBaseEntity(me)->ToPlayer(you))
		{
			Original_FrameStageNotify(curStage);
			call_original = true;
			return;
		}

		gMisc.FrameStageNotify(you, curStage);
		pNcc.fsn(you, curStage);

		if (call_original)
			Original_FrameStageNotify(curStage);
		call_original = true;
	}
	catch (...)
	{
		CATCH_WARN();
	}
}