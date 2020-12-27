#include "Misc.h"
#include "VMTHooks.h"
#include "Entity.h"
#include "CDraw.h"
#include "R8_inetchannel.h"
#include "Aim.h"
#include "R8_ICvar.h"

Misc gMisc;

#include "ESP.h"
Vector Misc::predPosAt(float flTime, CPlayer* target)
{
	if (!flTime)
		return target->GetAbsOrigin();

	static ConVar* sv_gravity = gBase.Cvar->FindVar("sv_gravity");

	Vector startPos = target->GetAbsOrigin(), velocity = target->EstimateAbsVelocity();
	float zdrop;
	if (target->Flags() & FL_ONGROUND)
		zdrop = velocity.z * flTime;
	else
		zdrop = 0.5 * -sv_gravity->GetInt() * pow(flTime, 2) + velocity.z * flTime;

	Vector result(
		startPos.x + (velocity.x * flTime),
		startPos.y + (velocity.y * flTime),
		startPos.z + zdrop);

	if (paths.value == 2)
	{
		float angleY = 0;
		PlayerCache* cache = gCache.FindPlayer(target);
		if (cache && cache->Full()) // Determine player strafe by averaging cache data
		{
			// A = Last, B = Mid, C = Current (startPos)
			int last = (MAX_CACHE - 1) / 3, mid = (MAX_CACHE - 1) / 6;
			Vector vLast, vMid;
			if (HitboxData* data = cache->FindTick(gBase.Globals->tickcount - last))
			{
				vLast = data->vCenter;
				if (data = cache->FindTick(gBase.Globals->tickcount - mid))
					vMid = data->vCenter;
			}
			// Approximate angle
			Vector forward, strafe;
			VectorAngles(vMid - vLast, forward);
			VectorAngles(startPos - vMid, strafe);
			// Divide our angle to measure by distance
			angleY = (strafe.y - forward.y) / ((vMid - vLast).Length2D() + (startPos - vMid).Length2D());
		}
		gEsp.rotate_vec2(*(Vector2*)&result, *(Vector2*)&startPos, DEG2RAD(angleY * (result - startPos).Length2D()));
	}

	return result;
}

inline Vector2 vectorAngle(float radians, float length)
{
	return Vector2(sin(radians) * length, cos(radians) * length);
}

inline float toAngle(const Vector2& vec)
{
	return atan2(vec.x, vec.y);
}

#include "Hitscan.h"
#include "Ray tracing.h"
#include <sstream>
void Misc::Draw(CPlayer* You)
{
	if (!paths.value)
		return;

	if (!You || You->Lifestate() != LIFE_ALIVE) // Not in-game
		return;

	for (int c = 0; c < 1 + (gAim.targetIndex > 0 && gAim.target.value); c++)
	{
		Vector velocity = You->EstimateAbsVelocity() / 10, predPos[101];
		int flags = You->Flags();

		int max = (flags & FL_ONGROUND) ? 21 : 101;
		for (int i = 0; i < max; i++)
			predPos[i] = predPosAt(i * 0.05, You);

		SColor lineColor;
		for (int i = 0; i < max - 1; i++)
		{
			trace_t result; // Check to see if we're about to hit a surface
			if (!gHit.AssumeVis(You, predPos[i], predPos[i + 1], &result))
			{
				// Draw a circle to show our landing position/angle
				if (!(flags & FL_ONGROUND))
				{
					const Vector relSquare[] = { Vector(30, 30, 0), Vector(30, -30, 0), Vector(-30, -30, 0), Vector(-30, 30, 0) };
					Vector scrPos[4];
					bool visible = true;
					for (size_t j = 0; j < sizeof(relSquare) / sizeof(Vector); j++)
					{
						if (!gDraw.WorldToScreen(relSquare[j] + result.endpos, scrPos[j]))
						{
							visible = false;
							break;
						}
					}
					if (visible)
					{
						Vector start, end;
						if (gDraw.WorldToScreen(result.endpos, start) && gDraw.WorldToScreen(result.endpos + result.plane.normal * 30, end))
							gDraw.DrawLine(start.x, start.y, end.x, end.y, SColor(255));
						for (size_t j = 0; j < sizeof(scrPos) / sizeof(Vector); j++)
						{
							int last = j - 1;
							if (j == 0)
								last = 3;
							gDraw.DrawLine(scrPos[j].x, scrPos[j].y, scrPos[last].x, scrPos[last].y, SColor(255));
						}
					}
				}
				break;
			}

			if (i < 21) // Make a gradiant from green to red
				lineColor = hsv2rgb((100.f - (i * 5)) / 255, 1, 1, 255);

			Vector screenPos[3];
			if (!gDraw.WorldToScreen(predPos[i], screenPos[0]))
				continue;
			if (!gDraw.WorldToScreen(predPos[i + 1], screenPos[1]))
				continue;

			gDraw.DrawLine(screenPos[0].x, screenPos[0].y, screenPos[1].x, screenPos[1].y, lineColor);

			Vector2 extraLine = Vector2(predPos[i + 1].x, predPos[i + 1].y);
			extraLine += vectorAngle(toAngle(Vector2(velocity.x, velocity.y)), 10);
			extraLine = gEsp.rotate_vec2(extraLine, Vector2(predPos[i + 1].x, predPos[i + 1].y), DEG2RAD(90));
			if (!gDraw.WorldToScreen(Vector(extraLine.x, extraLine.y, predPos[i + 1].z), screenPos[2]))
				continue;

			gDraw.DrawLine(screenPos[1].x, screenPos[1].y, screenPos[2].x, screenPos[2].y, lineColor);
		}

		if (!GetBaseEntity(gAim.targetIndex)->ToPlayer(You))
			break;
	}
}

//#include "R8_ITexture.h"
//static IMaterialSystem* iMat = (IMaterialSystem*)gBase.MatSystem;
#include "R8_cnetmessage.h"
void Misc::Command(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket)
{
	//static int waittime = 66, iTicksLeft = 0;
	//static bool bEndMove = false;
	//if (teleport.KeyDown()/* && !iTicksLeft*/)
		//iTicksLeft = true;

	//if (iTicksLeft && cmd->command_number)
	//{
		/*commands.push_front(cmd);
		if (commands.size() > waittime)
			commands.pop_back();*/

		//*bSendPacket = false, iTicksLeft = false;
	//}
	/*else if (!iTicksLeft)
	{
		if (commands.size() == waittime)
		{
			for (int i = 1; i < commands.size(); i++)
				commands[i]->forwardmove = i >= 44 ? -450.f : 450.f;
		}
		commands.clear();
	}*/

	bool bShooting = gAim.CanShoot() && cmd->buttons & (IN_ATTACK | IN_ATTACK2);
	bool bCrouching = You->Flags() & FL_DUCKING && You->Flags() & FL_ONGROUND;
	if (crouchwalk.value && bCrouching && !bShooting)
	{
		Vector vecMove(-cmd->forwardmove, -cmd->sidemove, 0.0f);
		float flLength = vecMove.Length();
		if (flLength > 0.0f)
		{
			Vector angMove;
			VectorAngles(vecMove, angMove);
			cmd->forwardmove = -flLength;
			cmd->sidemove = 0.0f; // Move only backwards, no sidemove
			cmd->viewangles.y -= angMove.y;
			cmd->viewangles.z = 90.0f; // OMFG SUPER 1337 SPEEDHAQ METHODS 8)
		}
	}

	if (bhop.value)
	{
		if (cmd->buttons & IN_JUMP)
			if (!(You->Flags() & FL_ONGROUND))
				cmd->buttons &= ~IN_JUMP;
	}

	*(bool*)(You + nPlayer.m_nForceTauntCam) = thirdperson.KeyDown();

	static ConVar* avoidPush = gBase.Cvar->FindVar("tf_avoidteammates_pushaway");
	if (avoidPush)
		avoidPush->SetValue(!nopush.value);

	if (gMisc.lagmode.value && gMisc.lagexploit.value)
	{
		const char* szCommand = lagcmd.list[lagcmd.value].c_str();
		if (gMisc.lagmode.value == 1) // Basic
			for (int i = 0; i < gMisc.lagexploit.value; i++)
				gBase.Engine->ClientCmd_Unrestricted(szCommand);
		else if (gMisc.lagmode.value == 2) // Unreliable
		{
			NET_StringCmd cmd(szCommand);
			INetChannel* net = gBase.Engine->GetNetChannelInfo();
			cmd.SetNetChannel(net);
			cmd.SetReliable(false);
			for (int i = 0; i < gMisc.lagexploit.value; i++)
				net->SendNetMsg(cmd, false);
			net->Transmit();
		}
	}
}

class CAttribute
{
public:
	void *pad;
	uint16_t m_iAttributeDefinitionIndex;
	float m_flValue;
	unsigned int uintPad;

	inline CAttribute(uint16_t iAttributeDefinitionIndex, float flValue)
	{
		m_iAttributeDefinitionIndex = iAttributeDefinitionIndex;
		m_flValue = flValue;
	}
};

enum attrib_ids
{
	//Apply these for australium item
	AT_is_australium_item = 2027,
	AT_loot_rarity = 2022,
	AT_item_style_override = 542,

	//This one and item style override 0 to golden pan
	AT_ancient_powers = 150,

	AT_uses_stattrack_module = 719,

	AT_is_festive = 2053,

	//Only ks sheen is visible
	AT_sheen = 2014,

	AT_unusual_effect = 134,
	AT_particle_effect = 370,

	AT_weapon_allow_inspect = 731,
};

enum unusual_effects
{
	WE_hot = 701,
	WE_isotope = 702,
	WE_cool = 703,
	WE_energyorb = 704,

	HE_sparkle = 4,
	HE_buringflames = 13,
	HE_lanterns = 37,
	HE_terrorwat = 57,
	HE_memoryleak = 65,
	UT_72 = 3004,
	UT_holygrail = 3003,
	UT_fireworks_blue = 3002,
	UT_fireworks_red = 3001,
};

#include "CUtlVector.h"
class CAttributeList
{
public:
	void *pad;
	CUtlVector<CAttribute, CUtlMemory<CAttribute>> m_Attributes;

	inline bool IsEmpty()
	{
		return m_Attributes.Size() == 0;
	}

	inline void AddAttribute(int iIndex, float flValue)
	{
		for (int i = 0; i < m_Attributes.Count(); i++)
		{
			CAttribute& at = m_Attributes[i];
			if (at.m_iAttributeDefinitionIndex == iIndex)
			{
				at.m_flValue = flValue;
				return;
			}
		}

		//15 = MAX_ATTRIBUTES
		if (m_Attributes.Count() > 14)
			return;

		CAttribute attr(iIndex, flValue);
		m_Attributes.AddToTail(attr);
	}

	inline void RemoveAttribute(int iIndex)
	{
		for (int i = 0; i < m_Attributes.Count(); i++)
		{
			if (m_Attributes[i].m_iAttributeDefinitionIndex == iIndex)
				return m_Attributes.FastRemove(i);
		}
	}
};

#include "Offsets.h"
#include "HvH.h"
void Misc::FrameStageNotify(CPlayer* You, ClientFrameStage_t Stage)
{
	if (Stage == FRAME_RENDER_START)
	{
		if (thirdperson.KeyDown())
		{
			// Basic clamping to show what you look like to others
			if (pNcc.antiaim.value && pNcc.show.value)
			{
				Vector vecAngles = gAim.lastAngles;
				if (vecAngles.x > 89.f)
					vecAngles.x = 89.f;
				else if (vecAngles.x < -89.f)
					vecAngles.x = -89.f;

				*(Vector*)(You + nPlayer.deadflag + 0x4) = vecAngles;
			}
			else
				*(Vector*)(You + nPlayer.deadflag + 0x4) = gAim.lastAngles;
		}

		int &fovPtr = *(int*)(You + nPlayer.m_iFOV), defaultFov = *(int*)(You + nPlayer.m_iDefaultFOV);
		if (noscope.value == 2 || noscope.value == 3) // If remove zoom or remove scope + zoom are set
		{
			fovPtr = fov.value ? fov.value : defaultFov;
			*(float*)(You + nPlayer.m_flFOVRate) = 0;
		}
		else if (fov.value && !(You->Conditions() & tf_cond::TFCond_Zoomed))
			fovPtr = fov.value;
	}
	else if (Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		//Original_FrameStageNotify(Stage); // We need to call the original before doing our skin changer

		/*size_t* hMyWeapons = you->MyWeapons();
		for (int i = 0; hMyWeapons[i]; i++)
		{
			CCombatWeapon* pWeapon = (CCombatWeapon*)gBase.EntList->GetClientEntityFromHandle(hMyWeapons[i]);
			if (!pWeapon)
				continue;

			int* id = pWeapon->pItemID();
			switch (*id)
			{

			}
		}*/


		/*CAttributeList* pAttribs = You->AttributeList();
		if (pAttribs && pAttribs->IsEmpty())
		{
			switch (gMisc.utaunt.value)
			{
			case 0: break;
			case 1:
				pAttribs->AddAttribute(AT_particle_effect, HE_sparkle); break;
			case 2:
				pAttribs->AddAttribute(AT_particle_effect, HE_buringflames); break;
			case 3:
				pAttribs->AddAttribute(AT_particle_effect, HE_lanterns); break;
			case 4:
				pAttribs->AddAttribute(AT_particle_effect, HE_terrorwat); break;
			case 5:
				pAttribs->AddAttribute(AT_particle_effect, HE_memoryleak); break;
			case 6:
				pAttribs->AddAttribute(AT_particle_effect, UT_72); break;
			case 7:
				pAttribs->AddAttribute(AT_particle_effect, UT_holygrail); break;
			case 8:
				byte team = You->Team();
				if (team == BLU_TEAM)
					pAttribs->AddAttribute(AT_particle_effect, UT_fireworks_blue);
				else if (team == RED_TEAM)
					pAttribs->AddAttribute(AT_particle_effect, UT_fireworks_red);
			}
		}

		static int prevTaunt = gMisc.utaunt.value;
		if (gMisc.utaunt.value != prevTaunt)
		{
			typedef void(__thiscall *ForceFullUpdateFn)(CClientState *);
			static ForceFullUpdateFn ForceFullUpdate = (ForceFullUpdateFn)gOffsets.ForceFullUpdate;
			ForceFullUpdate(gBase.ClientState);

			prevTaunt = gMisc.utaunt.value;
		}*/
	}
}