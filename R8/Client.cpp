#include "Client.h"
#include "Entity.h"
#include "VMTHooks.h"

#include "Misc.h"
#include "Aim.h"
#include "Trigger.h"
#include "HvH.h"
#include "Cache.h"
#include "NetChan.h"
#include "R8_inetchannel.h"
#include "NetChan.h"

#include "Cheat.h"

int lastChlTick = 0;

#include <fstream>
//============================================================================================
bool __fastcall Hooked_CreateMove(void* ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode);
	bool bReturn = hook.GetMethod<bool(__thiscall*)(void*, float, CUserCmd*)>((int)e_offset::CreateMove)(ClientMode, input_sample_frametime, pCommand);

	static VMTBaseManager* netHook = new VMTBaseManager();
	INetChannel* newNet = gBase.Engine->GetNetChannelInfo();
	if (newNet && getvfunc<DWORD>(newNet, (size_t)e_offset::SendDatagram) != (DWORD)Hooked_SendDatagram)
	{
		netHook->Init(newNet);
		netHook->HookMethod(Hooked_SendDatagram, e_offset::SendDatagram);
		netHook->Rehook();
		ResetIncomingSequences();
	}

	if (pCommand->command_number)
	{
		lastChlTick = pCommand->tick_count;
		UpdateIncomingSequences();
	}

	CPlayer* you;
	if (!GetBaseEntity(me)->ToPlayer(you) || you->Lifestate() != LIFE_ALIVE)
		return bReturn;

	try
	{
		void *createMoveEBP = NULL;
		__asm mov createMoveEBP, ebp;
		bool* bSendPacket = (bool *)(***(byte****)createMoveEBP) - 0x1;

		if (pCommand->command_number)
		{
			if (gAim.backtrack.value || gAim.cache.value || gMisc.paths.value)
			{
				gCache.Update(you, pCommand);
				gAim.CalcCanShoot(you);
			}
		}

		Vector vOld = pCommand->viewangles; // Save old angles beforehand for movement fix
		CBaseCheat::RunCommand(pCommand, you, you->GetIndex(), bSendPacket);
		gAim.command(you, pCommand, bSendPacket);
		pNcc.command(you, pCommand, bSendPacket);

		bool bAttack = (pCommand->buttons & IN_ATTACK) || (pCommand->buttons & IN_ATTACK2);
		bool isAttacking = (bAttack && gAim.CanShoot()) || (gAim.targetIndex && gAim.loud.value);
		bool silentAim = (gAim.enabled.KeyDown() && !gAim.loud.value) || gTrigger.airblast.value == 3; // Stuff that needs to set angles while shooting
		bool otherAims = pNcc.antiaim.value || gMisc.crouchwalk.value; // Other things that won't disturb aim while shooting can go here

		// If we have silent aim off, but we're using anti-aims and attacking, show our angles
		// Otherwise, hide them
		if ((!silentAim && otherAims && isAttacking) || (!silentAim && !otherAims))
			gBase.Engine->SetViewAngles(pCommand->viewangles);
		
		if (pCommand->command_number)
		{
			if (gAim.Choke())
				*bSendPacket = false;
			if (gAim.doBacktrack && pCommand->buttons & IN_ATTACK)
				pCommand->tick_count = gAim.doBacktrack, gAim.doBacktrack = false;
		}

		if (silentAim || otherAims)
			gAim.SilentFix(pCommand, vOld);

		bool antiAim = pNcc.antiaim.value &&
			(pNcc.pitch.value ||
			(pNcc.yaw.value && (pNcc.realy.value || pNcc.fakey.value)));;

		// If we are using AA, update every tick for time-based angles
		if (!antiAim || (pCommand->command_number && (!pNcc.silent.value || pNcc.show.value == pNcc.FakeAngle())))
			gAim.lastAngles = pCommand->viewangles;
	}
	catch(...)
	{
		CATCH_WARN();
	}
	return false;
}