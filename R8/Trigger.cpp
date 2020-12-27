#include "Trigger.h"
#include "Entity.h"
#include "Hitscan.h"
#include "R8_inetchannel.h"
#include "Aim.h"

Trigger gTrigger;

void Trigger::Command(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket)
{
	if (airblast.value && You->Class() == TF2_Pyro)
	{
		for (int index = gBase.Engine->GetMaxClients(); index <= gBase.EntList->GetHighestEntityIndex(); index++)
		{
			CBaseEntity* current = GetBaseEntity(index);
			if (!current || current->IsDormant())
				continue;

			int id = current->GetClientClass()->iClassID;
			if (id == (int)ent_id::CTFProjectile_Rocket ||
				id == (int)ent_id::CTFGrenadePipebombProjectile ||
				id == (int)ent_id::CTFCleaver ||
				id == (int)ent_id::CTFProjectile_Jar ||
				id == (int)ent_id::CTFProjectile_Flare ||
				id == (int)ent_id::CTFProjectile_Arrow ||
				id == (int)ent_id::CTFProjectile_SentryRocket ||
				id == (int)ent_id::CTFProjectile_JarMilk ||
				id == (int)ent_id::CTFProjectile_HealingBolt ||
				id == (int)ent_id::CTFProjectile_EnergyBall)
			{
				if (current->Team() == You->Team())
					continue;

				float latency = gBase.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
				Vector vel = current->EstimateAbsVelocity(),
					pos = current->GetWorldCenter() + vel * latency,
					myPos = You->EyePos();

				Vector angles = gAim.AnglesTo(pos, You);
				Vector clientAngles; gBase.Engine->GetViewAngles(clientAngles);
				Vector aDiff = angles - clientAngles;

				if (aDiff.y > 180)
					aDiff.y -= 360;
				else if (aDiff.y < -180)
					aDiff.y += 360;

				if (airblast.value < 2 && aDiff.Length2D() > 60)
					continue; // Make sure the projectile isn't out of FOV

				if (!gHit.AssumeVis(You, myPos, pos))
					continue;

				float dist = sqrt(myPos.DistToSqr(pos));
				if (dist <= 250)
				{
					cmd->buttons |= IN_ATTACK2;
					if (airblast.value >= 2) // Rage airblast
						cmd->viewangles = angles;
				}
			}
		}
	}

	if (backstab.value)
	{
		CKnife* knife;
		if (You->ActiveWeapon()->ToKnife(knife))
		{
			if (gAim.smarttrack.value)
			{
				int iBacktrack = gCache.GetTick() - gAim.backtrack.value;
				iBacktrack += TIME_TO_TICKS(gBase.Engine->GetNetChannelInfo()->GetLatency(MAX_FLOWS));
				for (auto& pPlayer : gCache.players)
				{
					const HitboxData* hitbox;
					if (!pPlayer.player || !(hitbox = pPlayer.FindTick(iBacktrack)))
						continue;

					if (knife->CalcBackstab(You, pPlayer.player, hitbox->vCenter))
					{
						cmd->buttons |= IN_ATTACK;
						break;
					}
				}
			}
			else if (knife->ReadyToBackstab()) // CalcBackstab isn't working properly right now
				cmd->buttons |= IN_ATTACK;
		}
	}

	if (!enabled.KeyDown()) // Auto-shooting
		return;

	Vector viewAngles, forward, eyePos = You->EyePos();
	gBase.Engine->GetViewAngles(viewAngles);
	AngleVectors(viewAngles, &forward);
	forward = forward * 8192.0f + eyePos;

	Ray_t ray;
	trace_t trace;
	ray.Init(eyePos, forward);

	CTraceFilterCustom filter(You);
	gBase.EngineTrace->TraceRay(ray,
		CONTENTS_HITBOX		|
		CONTENTS_SOLID		|
		CONTENTS_WINDOW		|
		CONTENTS_MOVEABLE	|
		CONTENTS_DEBRIS		|
		CONTENTS_MONSTER,
		&filter, &trace);

	// Make sure we should target the player
	CPlayer* player;
	if (!trace.m_pEnt->ToPlayer(player)		||
		player->Lifestate() != LIFE_ALIVE	||
		You->Team() == player->Team()		||
		!ShouldTarget(player))
		return;

	if (trace.hitgroup < 1)
		return;

	switch (hitbox.value)
	{
	case 0:
		if (trace.hitbox)
			return;
	case 1:
		if (trace.hitbox <= 5)
			return;
	case 2:
		if (trace.hitbox != 17 && trace.hitbox != 14)
			return;
	}
	cmd->buttons |= IN_ATTACK;
}

bool Trigger::ShouldTarget(CPlayer* Player)
{
	int iConds = Player->Conditions();
	return !(iConds & TFCond_Ubercharged || iConds & TFCond_Bonked);
}