#include "Entity.h"
#include "Ray tracing.h"
#include "IGameEvents.h"

#include "HvH.h"
#include "Aim.h"
#include "Hitscan.h"

ProNCC pNcc;

class CGamerHurt : public IGameEventListener2
{
public:
	CGamerHurt() { }//gBase.GameEventManager->AddListener(this, "player_hurt", false); }
	~CGamerHurt() { }//gBase.GameEventManager->RemoveListener(this); }
protected:
	void FireGameEvent(IGameEvent *Event);
};

inline float CalcEdge(CPlayer* pLocal)
{
	trace_t result;
	Vector eyePos = pLocal->EyePos();
	float leastDist = -1, edgeYaw = -1;
	for (int i = 0; i < 360; i += 2)
	{
		Vector angle(0, i, 0), direction;
		AngleVectors(angle, &direction);

		if (gHit.AssumeVis(pLocal, eyePos, direction * 90 + eyePos, &result))
			continue;

		Vector normal = result.plane.normal;
		if (abs(normal.y - direction.y) <= 2 && normal.x < 60 && normal.x > -60)
		{
			if (leastDist == -1 || Vector(result.endpos - eyePos).Length() < leastDist)
			{
				edgeYaw = i + 90, leastDist = Vector(result.endpos - eyePos).Length();

				Vector centerEndpos = result.endpos, rightNrm;
				AngleVectors(Vector(0, i - 90, 0), &rightNrm);

				Vector start = rightNrm * 60 + centerEndpos;
				gHit.AssumeVis(pLocal, start, start + direction * 80, &result);

				if (result.DidHitWorld())
					edgeYaw = i - 90;
			}
		}
	}

	if (leastDist != -1)
		return edgeYaw;
	else
		return -1;
}

#include "R8_inetchannel.h"
void ProNCC::command(CPlayer* you, CUserCmd* cmd, bool* bSendPacket)
{
	/*if (resolver.value)
	{
		static CGamerHurt gamerHurt; // Listen for the gamer pain
		if (cmd->buttons & IN_ATTACK && gAim.CanShoot(you) && gAim.targetIndex)
		{
			float flPitch = ((CPlayer*)(GetBaseEntity(gAim.targetIndex)))->pViewangles()->x;
			PlayerCache* cache;
			if (flPitch > 89 || flPitch < -89 || ((cache = gCache.FindIndex(gAim.targetIndex)) && cache->resolve))
			{
				gamerShot.index = gAim.targetIndex;
				gamerShot.wait = gBase.Globals->curtime + gBase.Engine->GetNetChannelInfo()->GetLatency(MAX_FLOWS) * 1.5;
			}
		}
	}*/

	if (!cmd->command_number)
		return;

	bFakeAngle = false;
	if (!antiaim.value)
		return;

	// Don't mess up our aim when we shoot something
	if ((gAim.targetIndex && gAim.loud.value) || ((cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2)))
		if (gAim.CanShoot())
			return;

	bool bSwitch = cmd->tick_count % 2;
	if (silent.value && !bSwitch)
		*bSendPacket = false;

	Vector vNew = gBase.Engine->GetViewAngles();

	// Pitch AA
	if (pitch.value)
	{
		if (silent.value)
		{
			vNew.x = bSwitch ? -89.f + float(realp.value) * 45.f : (fakep.value ? -88.9f : 88.9f); // Trip up auto resolvers
			if (!bSwitch) // We're currently using timing-based fake angles
				bFakeAngle = true;
		}
		else
			vNew.x = -89.f + float(realp.value) * 45.f + (fakep.value ? -360.f : 360.f);
	}

	// Yaw AA
	if (yaw.value && (realy.value || fakey.value))
	{
		static float flSpin = 0.f;
		while (flSpin > 360.f)
			flSpin -= 360.f;
		flSpin += 3.f;

		int yawMode = 0;
		if (!fakey.value)
			yawMode = realy.value;
		else
		{
			yawMode = bSwitch ? realy.value : fakey.value;
			if (!bSwitch) // We're currently using timing-based fake angles
				bFakeAngle = true;
		}

		switch (yawMode)
		{
		case 1: // Edge
			float edge;
			if ((edge = CalcEdge(you)) != -1)
				vNew.y = edge;
			break;
		case 2: // Left
			vNew.y += 90.f; break;
		case 3: // Right
			vNew.y += -90.f; break;
		case 4: // Back
			vNew.y += 180.f; break;
		case 5: // Spin
			vNew.y += flSpin; break;
		}
	}
	cmd->viewangles = vNew;
}

void ProNCC::fsn(CPlayer* you, ClientFrameStage_t curStage)
{
	/*if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (auto& pPlayer : gCache.players)
		{
			if (pPlayer.resolve)
				pPlayer.player->pViewangles()->x = -89.f + (45.f * float(pPlayer.resolve - 1));
		}
	}
	else if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
	{

		if (ResolveShots() && gamerShot.index)
		{
			if (PlayerCache* cache = gCache.FindIndex(gamerShot.index))
			{
				if (!cache->bHit)
				{
					if (!cache->attempted || cache->attempted >= 0b111111)
					{
						cache->resolve = rand() % 6 + 1;
						cache->attempted = 1 << (cache->resolve - 1);
					}
					else
					{
						while (cache->attempted)
						{
							byte attempt = rand() % 6;
							if (!(cache->attempted & (1 << attempt)))
							{
								cache->resolve = attempt + 1, cache->attempted |= (1 << attempt);
								break;
							}
						}
					}
				}
				cache->bHit = false;
			}
		}
		gamerShot.index = 0;
	}*/

	if (!pNcc.resolver.KeyDown() || curStage != FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		return;

	for (int i = 1; i <= gBase.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;
		CPlayer* player;
		if (!GetBaseEntity(i)->ToPlayer(player))
			continue;

		// Ultrabasic auto resolver. You should probably add options to force viewangles
		// Especially useful to force y viewangles for eztaps on fake-siders
		Vector* angles = player->pViewangles();
		if (angles->x > 89)
			angles->x = -80;
		else if (angles->x < -89)
			angles->x = 89;
	}
}

bool ProNCC::ResolveShots()
{
	if (!pNcc.resolver.KeyDown())
		return false;

	CPlayer* you;
	if (!GetBaseEntity(me)->ToPlayer(you) || you->Class() != TF2_Sniper)
		return false;

	return you->ActiveWeapon() == GetEntHandle(you->MyWeapons()[0]);
}

void CGamerHurt::FireGameEvent(IGameEvent* Event)
{
	if (!pNcc.ResolveShots())
		return;

	if (PlayerCache* cache = gCache.FindUID(Event->GetInt("userid")))
		cache->bHit = true;
}