#include "Aim.h"
#include "Entity.h"
#include "Hitscan.h"

Aim gAim;

void Aim::command(CPlayer* you, CUserCmd* cmd, bool* bSendPacket)
{
	targetIndex = 0;
	if (!enabled.KeyDown())
		return;

	if (scope.value && you->Class() == TF2_Sniper && !(you->Conditions() & TFCond_Zoomed))
		if (cmd->weaponselect == 0)
			return; // Return if our sniper's primary weapon isn't scoped in

	Vector angles;
	if (!(targetIndex = FindTarget(you, angles)))
		return;

	bool bCanShoot = CanShoot();

	// Only aim when we're shooting with silent aim
	if (loud.value || cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2 || autoshoot.KeyDown())
	{
		if (!loud.value)
		{
			if (!bCanShoot)
				return;

			if (gHit.ProjSpeed(you->ActiveWeapon()))
				bChoke = true;
		}

		cmd->viewangles = angles; // Tadaa!

		if (autoshoot.KeyDown() && bCanShoot)
		{
			float tickbase = *(int*)(you + nPlayer.m_nTickBase);
			float flChargedTime = (tickbase * TICK_INTERVAL) - *(float*)(you + nPlayer.m_flFOVTime);
			if (you->Conditions() & tf_cond::TFCond_Zoomed && flChargedTime < 0.2)
				return;

			cmd->buttons |= IN_ATTACK;
		}
	}
}

int Aim::FindTarget(CPlayer* you, Vector& Angles)
{
	int Index = 0;

	Vector vecClient = gBase.Engine->GetViewAngles(), vecFinal = vecClient;
	float lowestDist = 181.0f;
	for (int i = 1; i <= gBase.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;
		CPlayer* player;
		if (!GetBaseEntity(i)->ToPlayer(player) ||
			player->Lifestate() != LIFE_ALIVE ||
			player->Team() == you->Team())
			continue;

		if (player->Conditions() & tf_cond::TFCond_Ubercharged ||
			player->Conditions() & tf_cond::TFCond_Bonked)
			continue;

		Vector target;
		if (smarttrack.value == 1) // Always backtrack
		{
			if (!ScanForHit(you, player, e_aim::backtrack, target))
				continue;
		}
		else
		{
			// If we can't hitscan, and auto-backtrack is disabled or fails, then continue
			if (!ScanForHit(you, player, hitscan.value ? e_aim::hitscan : e_aim::priority, target) &&
				(smarttrack.value != 2 || !ScanForHit(you, player, e_aim::backtrack, target)))
				continue;
		}

		Vector angles = AnglesTo(target, you); // Converting that line direction to angles
		Vector angleDiff = angles - vecClient; // Get the distance of the target from our crosshair

		// The max degrees between two points on a circle is 180 if you want the shortest path
		if (angleDiff.y > 180)
			angleDiff.y -= 360;
		else if (angleDiff.y < -180)
			angleDiff.y += 360;

		// Check if it's within our FOV range
		float distance = angleDiff.Length2D();
		if (distance <= fov.value)
		{
			// Look for lower FOV distance than last target
			if (distance < lowestDist)
				Index = i, lowestDist = distance, vecFinal = angles;
		}
	}

	if (Index)
		Angles = vecFinal;
	return Index;
}

#include "Misc.h"
#include "R8_inetchannel.h"
bool Aim::ScanForHit(CPlayer* you, CPlayer* Target, e_aim Option, Vector& Pos)
{
	if (!Target)
		return false;

	auto& priority = BestHitboxes();
	bool found = false;
	Vector start = you->EyePos(), end;

	int speed = gHit.ProjSpeed(you->ActiveWeapon());
	if (Option == e_aim::priority || Option == e_aim::hitscan)
	{
		if (speed)
		{
			float flTime = (start - Target->GetAbsOrigin()).Length() / speed;
			for (int i = 0; i < predict.value + 1; i++)
			{
				end = gMisc.predPosAt(flTime, Target);
				flTime = (start - end).Length() / speed;
			}

			if (gHit.AssumeVis(you, start, end))
				found = true;
		}
		else
		{
			for (auto& hitbox : priority)
			{	// Look for the best hit
				end = Target->HitboxPos(hitbox);
				if (gHit.CanHit(you, Target, start, end))
				{
					found = true;
					break;
				}
			}
		}
	}
	if (!found && !speed && Option == e_aim::hitscan)
	{
		for (int i = 0; i < 18; i++)
		{	// Look for any possible hit

			// Make sure we didn't already check this hitbox
			bool isRepeat = false;
			for (auto& hitbox : priority)
			{
				if (i == hitbox)
				{
					isRepeat = true;
					break;
				}
			}
			if (isRepeat)
				continue;

			end = Target->HitboxPos(i);
			if (gHit.CanHit(you, Target, start, end))
			{
				found = true;
				break;
			}
		}
	}
	if (!found && !speed && Option == e_aim::backtrack)
	{
		PlayerCache* cache = gCache.FindPlayer(Target);
		if (cache)
		{
			int iBacktrack = gCache.GetTick() - backtrack.value;
			if (float(backtrack.value) * TICK_INTERVAL > 0.2f)
				iBacktrack += TIME_TO_TICKS(gBase.Engine->GetNetChannelInfo()->GetLatency(MAX_FLOWS));
			if (cache->FindTick(iBacktrack))
			{
				end = cache->GetHitbox(iBacktrack, priority[0]);
				if (gHit.AssumeVis(you, start, end))
					found = true, doBacktrack = iBacktrack;
			}
		}
	}

	if (found)
		Pos = end;
	return found;
}

const vector<int>& Aim::BestHitboxes()
{
	static const vector<int>
		head({ 0 }),
		body({ 2, 3, 4, 1, 5 }),
		feet({ 14, 17 });
	if (hitbox.value == 0)
		return head;
	else if (hitbox.value == 1)
		return body;
	else if (hitbox.value == 2)
		return feet;
	else if (hitbox.value == 3)
	{
		CPlayer* you;
		if (GetBaseEntity(me)->ToPlayer(you))
		{
			switch (you->Class())
			{
			case TF2_Sniper:
				return head;
			case TF2_Soldier:
				return feet;
			}
			return body;
		}
	}
	return head;
}

// * Credits to F1ssi0n
void Aim::SilentFix(CUserCmd *Cmd, Vector vOldAng)
{
	Vector vecSilent(Cmd->forwardmove, Cmd->sidemove, Cmd->upmove), angMove;
	float flSpeed = vecSilent.Length2D();
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD(Cmd->viewangles.y - vOldAng.y + angMove.y);
	Cmd->forwardmove = cos(flYaw) * flSpeed, Cmd->sidemove = sin(flYaw) * flSpeed;
}

Vector Aim::AnglesTo(const Vector& Pos, CPlayer* You)
{
	if (!You)
		return Vector();

	Vector angles; // Converting that line direction to angles
	VectorAngles(Pos - You->EyePos(), angles);
	ClampAngle(angles);

	return angles;
}

bool Aim::CalcCanShoot(CPlayer* You)
{
	CCombatWeapon* wep = You->ActiveWeapon();
	if (!wep)
		return false;

	static float flNext = 0, flLast = 0;
	static CCombatWeapon* pLastWep;

	if (flLast != wep->LastFireTime() || wep != pLastWep)
		flLast = wep->LastFireTime(), flNext = wep->NextPrimaryAttack();

	if (!wep->Clip1())
		return false;

	pLastWep = wep;

	return bCanShoot = flNext <= float(*(int*)(You + nPlayer.m_nTickBase)) * TICK_INTERVAL;
}