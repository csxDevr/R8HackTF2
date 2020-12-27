#include "ESP.h"
#include "Entity.h"
#include "Panels.h"
#include "CDraw.h"

#include "Aim.h"
#include "Cache.h"
#include "Misc.h"

Esp gEsp;

void Esp::FrameStageNotify(ClientFrameStage_t Stage)
{
	if (Stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		return;

	static bool bWasGlowing = gEsp.enabled.value && glow.value;
	if (bWasGlowing && !glow.value)
	{
		for (int i = 1; i <= gBase.Engine->GetMaxClients(); i++)
		{
			CPlayer* player;
			if (!GetBaseEntity(i)->ToPlayer(player))
				continue;

			*player->pGlowEnabled() = false;
			player->UpdateGlowEffect();
		}
	}
	bWasGlowing = glow.value;
}

void Esp::DrawPlayer(CPlayer* pLocal, CPlayer* ent, int index)
{
	if (!enabled.value) 
		if (gAim.cache.value < 3)
			return; // No aimbot backtrack dots in ESP

	CPlayer* player;
	bool bLocalEsp = local.value && gMisc.thirdperson.KeyDown();
	if ((index == me && !bLocalEsp) || !ent->ToPlayer(player) || player->Lifestate() != LIFE_ALIVE)
		return;

	if ((index != me || !bLocalEsp) &&
		player->Team() == pLocal->Team())
		if (!team.value || (team.value == 2 && player->Class() != TF2_Medic))
			return;

	bool bCloaked = player->Class() == tf_classes::TF2_Spy && player->Conditions() & tf_cond::TFCond_Cloaked;
	if (cloaked.value == 2 && bCloaked)
		return;

	Vector origin = player->GetAbsOrigin(), pos;
	if (!gDraw.WorldToScreen(player->GetWorldCenter(), pos))
		return;

	// Use this so we can have Aim highlight its target
	SColor color = gDraw.TeamColor(player->Team());
	if (gAim.target.value && index == gAim.targetIndex)
		color = SColor(60, 200, 20);
	else if (cloaked.value == 1 && bCloaked)
		color = SColor(180);
	else if (!box_color.bDef)
		color = box_color.color;

	if (gAim.cache.value == 3 || (gAim.cache.value == 4 && index == gAim.targetIndex)) // Ticks only mode
	{
		PlayerCache* cache;
		if (cache = gCache.FindPlayer(player))
		{
			// Iterating lists is easier with an auto loop
			// 'i' is just here to prevent going over our backtrack amount
			size_t i = 0;
			for (auto& bone : cache->bones)
			{
				i++;
				if (i > gAim.backtrack.value)
					break;

				Vector pos, vecBone = cache->GetHitbox(bone, 0);
				if (!gDraw.WorldToScreen(vecBone, pos))
					continue;

				gDraw.DrawString(pos.x, pos.y, color, ".", gFonts.esp);
			}
		}
	}
	if (!enabled.value)
		return;

	DrawPlayerBox(pLocal, player, origin, color);

	if (bones.value)
	{
		static int torso[6] = { 1, 2, 3, 4, 5, 0 }, limbs[4 * 4] = { 14, 13, 12, 1, 17, 16, 15, 1, 8, 7, 6, 5, 11, 10, 9, 5 };

		for (int i = 0; i < 5; i++) // Loop through the torso bones first
		{
			Vector first, second;

			if (gDraw.WorldToScreen(player->HitboxPos(torso[i]), first))
				if (gDraw.WorldToScreen(player->HitboxPos(torso[i + 1]), second))
					gDraw.DrawLine(first.x, first.y, second.x, second.y, color);
		}
		for (int i = 0; i < 4; i++) // First loop get each set of limbs in the array
		{
			Vector first, second;
			for (int a = 0; a < 3; a++) // Second loop to draw point a to b for every limb
			{
				if (gDraw.WorldToScreen(player->HitboxPos(limbs[i * 4 + a]), first))
					if (gDraw.WorldToScreen(player->HitboxPos(limbs[i * 4 + a + 1]), second))
						gDraw.DrawLine(first.x, first.y, second.x, second.y, color);
			}
		}
	}

	if (gEsp.glow.value)
	{
		if (player->GlowEnabled())
			return; // If glow is already enabled

		*player->pGlowEnabled() = true;
		player->UpdateGlowEffect();
	}
}

void Esp::DrawEntity(CPlayer* pLocal, CBaseEntity* ent, int index)
{
	if (!sentries.value && !others.value)
		return;

	ent_id iType = ent->Type();
	if (iType != ent_id::CObjectSentrygun && iType != ent_id::CObjectDispenser && iType != ent_id::CObjectTeleporter)
		return;

	if (!sentries.value && iType == ent_id::CObjectSentrygun)
		return;
	else if (!others.value && iType != ent_id::CObjectSentrygun)
		return;

	CBaseObject* object = (CBaseObject*)ent;
	if (!bTeam.value && ent->Team() == pLocal->Team())
		return;

	// Use this so we can have Aim highlight its target
	SColor color = gDraw.TeamColor(ent->Team());
	if (gAim.target.value && index == gAim.targetIndex)
		color = SColor(60, 200, 20);
	else if (!box_color.bDef)
		color = box_color.color;

	Vector2 xy, wh;
	if (!GetBounds(pLocal, ent, xy, wh))
		return;
	float x = xy.x, y = xy.y, w = wh.x, h = wh.y;
	
	if (bBoxes.value)
	{
		if (box_style.value == 2)
			DrawBox3D(ent, color);
		else
			DrawBox(x, y, w, h, color);
	}

	static int textW = 0, textH = 0;
	if (!textH)
		gBase.Surface->GetTextSize(gFonts.esp, L"pA", textW, textH);

	if (bType.value)
	{
		const wchar_t* typeStr = L"";
		switch (iType)
		{
		case ent_id::CObjectSentrygun: typeStr = L"Sentry"; break;
		case ent_id::CObjectDispenser: typeStr = L"Dispenser"; break;
		case ent_id::CObjectTeleporter: typeStr = L"Teleporter"; break;
		}
		int textW, textH;
		gBase.Surface->GetTextSize(gFonts.esp, typeStr, textW, textH);
		gDraw.DrawString(x + (w / 2) - (textW / 2), y - textH, color, typeStr, gFonts.esp);
	}

	int iText = y - textH;
	if (bHealth.value)
	{
		float percent = (float)object->Health() / object->MaxHealth();
		if (percent > 1.f) percent = 1.f;
		SColor hp = hsv2rgb(percent * 0.3f, 0.9f, 0.8f);
		if (bHealth.value == 1)
			gDraw.DrawString(x + w + 3, iText += textH, hp, to_string(object->Health()), gFonts.esp);
		else if (bHealth.value == 2)
		{
			int height = float(h - 2) * percent;
			gDraw.DrawRect(x - 5, y, 4, h, SColor(50));
			gDraw.DrawRect(x - 4, y + h - height - 1, 2, height, hp);
		}
	}

	if (bConds.value && object->Disabled())
	{
		const char* szCond;
		if (object->PlasmaDisabled())
			szCond = "Disabled (Plasma)";
		else if (object->Sapped())
			szCond = "Sapped";
		else
			szCond = "Disabled";

		gDraw.DrawString(x + w + 3, iText += textH, color, (string)"*" + szCond + "*", gFonts.esp);
	}
}

#include "Materials.h"
#include "CMat.h"
#include "HvH.h"
#include "R8_inetchannel.h"
void Esp::DME(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld, CPlayer* pLocal, DrawModelExecuteFn Original)
{
	if (!chams.value && (gAim.cache.value == 0 || gAim.cache.value > 2))
		return;

	CPlayer *player;
	if (!GetBaseEntity(pInfo.entity_index)->ToPlayer(player) || player->Lifestate() != LIFE_ALIVE)
		return;

	bool bIsTarget = gAim.target.value && pInfo.entity_index == gAim.targetIndex;
	bool bReset = true;

	if (team.value == 1 || player->Team() != pLocal->Team() || (team.value == 2 && player->Class() == TF2_Medic))
	{
		if (chams.value)
		{
			if (outline.value)
			{
				gMat.ForceMaterial(bIsTarget ? SColor(60, 200, 20) : SColor(255), gMat.wireframe);
				Original(gBase.ModelRender, state, pInfo, pCustomBoneToWorld);
			}

			Matptr desired = gMat.glow; // Flat
			if (material.value == 1) // Shaded
				desired = gMat.shaded;
			else if (material.value == 2) // Shiny
				desired = gMat.shiny;

			SColor color = box_color.bDef ? gDraw.LightTeamColor(player->Team()) : box_color.color;
			color[3] = 255 - alpha.value;

			//if (desired == gMat.glow)
				//desired->SetMaterialVarFlag(MATERIAL_VAR_FLAT, true);
			desired->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, chams_vis.value != 2);

			gMat.ForceMaterial((bIsTarget && !outline.value) ? SColor(60, 200, 20) : color, desired);
			if (chams_vis.value == 0 || chams_vis.value == 2)
				bReset = false;
			else if (chams_vis.value == 1)
				Original(gBase.ModelRender, state, pInfo, pCustomBoneToWorld);
		}
	}

	if (player->Team() != pLocal->Team())
	{
		// Show backtrack chams on all players, or just the target
		if (gAim.cache.value == 2 || (gAim.cache.value == 1 && bIsTarget))
		{
			PlayerCache* cache = gCache.FindIndex(pInfo.entity_index);
			int tick =
				gCache.GetTick() - gAim.backtrack.value + TIME_TO_TICKS(gBase.Engine->GetNetChannelInfo()->GetLatency(MAX_FLOWS));
			HitboxData* bone;
			if (cache && (bone = cache->FindTick(tick)))
			{
				gMat.shaded->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				gMat.ForceMaterial(gDraw.TeamColor(player->Team()), gMat.shaded);
				Original(gBase.ModelRender, state, pInfo, bone->matrix);
			}
		}
	}

	if (bReset)
		gMat.ResetMaterial();
}

void Esp::DrawPlayerBox(CPlayer* You, CPlayer* player, Vector origin, SColor color)
{
	Vector2 xy, wh;
	if (!GetBounds(You, player, xy, wh, true))
		return;

	float x = xy.x, y = xy.y, w = wh.x, h = wh.y;
	if (boxes.value)
	{
		if (box_style.value == 2)
			DrawBox3D(player, color);
		else
			DrawBox(x, y, w, h, color);
	}

	static int textW = 0, textH = 0;
	if (!textH)
		gBase.Surface->GetTextSize(gFonts.esp, L"pA", textW, textH);
	int iText = y - textH;

	if (classes.value)
		gDraw.DrawString(x + w + 3, iText += textH, color, player->StrClass(), gFonts.esp);

	if (health.value)
	{
		float percent = (float)player->Health() / player->MaxHealth();
		if (percent > 1) percent = 1;
		SColor hp = hsv2rgb(percent * 0.3f, 0.9f, 0.8f);
		if (health.value == 1)
			gDraw.DrawString(x + w + 3, iText += textH, hp, to_string(player->Health()), gFonts.esp);
		else if (health.value == 2)
		{
			int height = float(h - 2) * percent;
			gDraw.DrawRect(x - 5, y, 4, h, SColor(50));
			gDraw.DrawRect(x - 4, y + h - height - 1, 2, height, hp);
		}
		else if (health.value == 3 || health.value == 4)
		{
			Vector lastInner, lastOuter, uLastInner, uLastOuter;
			float degrees = percent * 360;
			for (float i = 0.f; i <= degrees; i += 10.f)
			{
				Vector vInner = origin + Vector(45, 0, 0), vOuter = origin + Vector(48, 0, 0);
				Vector inner, outer, uInner, uOuter;

				Vector2 newXy = rotate_vec2(*(Vector2*)&vInner, *(Vector2*)&origin, DEG2RAD(i));
				vInner.x = newXy.x, vInner.y = newXy.y;
				newXy = rotate_vec2(*(Vector2*)&vOuter, *(Vector2*)&origin, DEG2RAD(i));
				vOuter.x = newXy.x, vOuter.y = newXy.y;

				if (!gDraw.WorldToScreen(vInner, inner) || !gDraw.WorldToScreen(vOuter, outer))
					break;
				if (!gDraw.WorldToScreen(vInner + Vector(0, 0, 3), uInner) || !gDraw.WorldToScreen(vOuter + Vector(0, 0, 3), uOuter))
					break;

				if (i)
				{
					Texture iTex = player->Team() == RED_TEAM ? gTex.team_red : gTex.team_blue;

					static Vertex_t verts[4];
					verts[3] = Vertex_t::FromVec3(uLastInner);
					verts[2] = Vertex_t::FromVec3(uLastOuter);
					verts[1] = Vertex_t::FromVec3(uOuter);
					verts[0] = Vertex_t::FromVec3(uInner);
					gDraw.DrawPolygon(4, verts, iTex);
					verts[0] = Vertex_t::FromVec3(uLastInner);
					verts[1] = Vertex_t::FromVec3(uLastOuter);
					verts[2] = Vertex_t::FromVec3(uOuter);
					verts[3] = Vertex_t::FromVec3(uInner);
					gDraw.DrawPolygon(4, verts, iTex);

					verts[0] = Vertex_t::FromVec3(uLastOuter);
					verts[1] = Vertex_t::FromVec3(uOuter);
					verts[2] = Vertex_t::FromVec3(outer);
					verts[3] = Vertex_t::FromVec3(lastOuter);
					gDraw.DrawPolygon(4, verts, iTex);

					if (health.value == 4)
						gDraw.DrawLine(uLastOuter.x, uLastOuter.y, uOuter.x, uOuter.y, SColor(0));
				}

				lastInner = inner, lastOuter = outer, uLastInner = uInner, uLastOuter = uOuter;
			}
		}
	}

	if (name.value)
	{
		player_info_s info = player->Info();
		int txtw, txth;
		gBase.Surface->GetTextSize(gFonts.esp, ToWchar(info.name).c_str(), txtw, txth);
		gDraw.DrawString(x + (w / 2) - (txtw / 2), y - txth, color, info.name, gFonts.esp);
	}

	if (conditions.value)
	{
		int iCond = player->Conditions();
		
		list<const char*> szConds;
		if (iCond & tf_cond::TFCond_Ubercharged)
		{
			if (iCond & tf_cond::TFCond_Kritzkrieged || iCond & tf_cond::TFCond_MegaHeal)
			{
				if (iCond & tf_cond::TFCond_Kritzkrieged)
					szConds.push_back("Kritz");
				if (iCond & tf_cond::TFCond_MegaHeal)
					szConds.push_back("Quick-Fix");
			}
			else
				szConds.push_back("Ubered");
		}

		if (iCond & tf_cond::TFCond_MiniCrits)
			szConds.push_back("Mini-crits");
		if (iCond & tf_cond::TFCond_Cloaked)
			szConds.push_back("Cloaked");
		if (iCond & tf_cond::TFCond_Jarated)
			szConds.push_back("Jarated");
		if (iCond & tf_cond::TFCond_Milked)
			szConds.push_back("Milked");
		if (iCond & tf_cond::TFCond_Bleeding)
			szConds.push_back("Bleeding");
		if (iCond & tf_cond::TFCond_OnFire)
			szConds.push_back("Burning");
		if (iCond & tf_cond::TFCond_Zoomed)
			szConds.push_back("Zoomed");
		else if (iCond & tf_cond::TFCond_Slowed)
			szConds.push_back("Slowed");
		if (iCond & tf_cond::TFCond_Stunned)
			szConds.push_back("Stunned");
		if (iCond & tf_cond::TFCond_MarkedForDeath)
			szConds.push_back("Marked for death");
		if (iCond & tf_cond::TFCond_Disguising)
			szConds.push_back("Disguising");
		if (iCond & tf_cond::TFCond_DefenseBuffed)
			szConds.push_back("Buffed");
		if (iCond & tf_cond::TFCond_Charging)
			szConds.push_back("Charging");

		for (auto cond : szConds)
			gDraw.DrawString(x + w + 3, iText += textH, color, (string)"*" + cond + "*", gFonts.esp);
	}
}

void Esp::DrawBox(int x, int y, int w, int h, SColor color)
{
	if (box_style.value == 0)
	{
		for (int i = 0; i < box_thick.value; i++)
			gDraw.OutlineRect(x + i, y + i, w - i * 2, h - i * 2, color);

		if (box_outline.value)
		{
			gDraw.OutlineRect(x - 1, y - 1, w + 2, h + 2, SColor(0));
			if (box_outline.value == 2)
				gDraw.OutlineRect(x + box_thick.value, y + box_thick.value, w - box_thick.value * 2, h - box_thick.value * 2, SColor(0));
		}
	}
	else if (box_style.value == 1)
	{
		// At this point, a for loop would take longer to code
		int size = (w < h ? w : h) / 3;
		if (box_outline.value)
		{
			ExpandOutline(x, y, size, box_thick.value, SColor(0), 1);
			ExpandOutline(x, y + box_thick.value, box_thick.value, size - box_thick.value, SColor(0), 1);

			ExpandOutline(x + w - size, y, size, box_thick.value, SColor(0), 1);
			ExpandOutline(x + w - box_thick.value, y + box_thick.value, box_thick.value, size - box_thick.value, SColor(0), 1);

			ExpandOutline(x, y + h - box_thick.value, size, box_thick.value, SColor(0), 1);
			ExpandOutline(x, y + h - size, box_thick.value, size - box_thick.value, SColor(0), 1);

			ExpandOutline(x + w - size, y + h - box_thick.value, size, box_thick.value, SColor(0), 1);
			ExpandOutline(x + w - box_thick.value, y + h - size, box_thick.value, size - box_thick.value, SColor(0), 1);
		}

		gDraw.DrawRect(x, y, size, box_thick.value, color);
		gDraw.DrawRect(x, y + box_thick.value, box_thick.value, size - box_thick.value, color);

		gDraw.DrawRect(x + w - size, y, size, box_thick.value, color);
		gDraw.DrawRect(x + w - box_thick.value, y + box_thick.value, box_thick.value, size - box_thick.value, color);

		gDraw.DrawRect(x, y + h - box_thick.value, size, box_thick.value, color);
		gDraw.DrawRect(x, y + h - size, box_thick.value, size - box_thick.value, color);

		gDraw.DrawRect(x + w - size, y + h - box_thick.value, size, box_thick.value, color);
		gDraw.DrawRect(x + w - box_thick.value, y + h - size, box_thick.value, size - box_thick.value, color);
	}
}

void Esp::DrawBox3D(CBaseEntity* Entity, SColor Color)
{
	Vector origin = Entity->GetAbsOrigin();
	Vector maxs = Entity->GetCollideableMaxs() + origin, mins = Entity->GetCollideableMins() + origin;
	float boxWidth = (maxs.x - mins.x) / 2.f;
	Vector boxCoords[4] =
	{
		Vector(origin.x - boxWidth, origin.y - boxWidth, mins.z),
		Vector(origin.x + boxWidth, origin.y - boxWidth, mins.z),
		Vector(origin.x + boxWidth, origin.y + boxWidth, mins.z),
		Vector(origin.x - boxWidth, origin.y + boxWidth, mins.z),
	}, scrBox[8];
	Vector height(0, 0, maxs.z - mins.z);

	for (int i = 0; i < 4; i++) // Loop through all the points to get the screen positions
	{
		if (!gDraw.WorldToScreen(boxCoords[i], scrBox[i]) && i > 0)
			scrBox[i] = scrBox[i - 1];
		if (!gDraw.WorldToScreen(boxCoords[i] + height, scrBox[i + 4]) && i > 0)
			scrBox[i + 4] = scrBox[i + 3];
	}

	for (int i = 0; i < 4; i++) // Another loop to connect them with lines
	{
		int next = i == 3 ? -3 : 1;
		gDraw.DrawLine(scrBox[i].x, scrBox[i].y, scrBox[i + next].x, scrBox[i + next].y, Color);
		gDraw.DrawLine(scrBox[i].x, scrBox[i].y, scrBox[i + 4].x, scrBox[i + 4].y, Color);
		gDraw.DrawLine(scrBox[i + 4].x, scrBox[i + 4].y, scrBox[i + 4 + next].x, scrBox[i + 4 + next].y, Color);
	}
}

bool Esp::GetBounds(CPlayer* You, CBaseEntity * Entity, Vector2 &xy, Vector2 &wh, bool Thin)
{
	Vector origin = Entity->GetAbsOrigin(), angLocal;
	Vector mins = Entity->GetCollideableMins() + origin, maxs = Entity->GetCollideableMaxs() + origin;
	VectorAngles(You->EyePos() - origin, angLocal);
	ClampAngle(angLocal);
	
	if (Thin)
		angLocal.y += 90;

	const static int iBounds = 6;
	float centerZ = mins.z + ((maxs.z - mins.z) / 2), boxWidth = (maxs.x - mins.x) / 2;
	Vector bounds[iBounds] =
	{
		origin,
		Vector(origin.x + boxWidth, origin.y, centerZ),
		Vector(origin.x - boxWidth, origin.y, centerZ),
		Vector(origin.x, origin.y + boxWidth, centerZ),
		Vector(origin.x, origin.y - boxWidth, centerZ),
		Vector(origin.x, origin.y, maxs.z)
	};

	Vector2 vec2origin(origin.x, origin.y);
	for (int i = 0; i < iBounds; i++)
	{
		Vector2 point = rotate_vec2(Vector2(bounds[i].x, bounds[i].y), vec2origin, DEG2RAD(angLocal.y));
		bounds[i] = Vector(point.x, point.y, bounds[i].z);
	}

	return ScreenBounds(bounds, iBounds, xy, wh);
}

void Esp::PreviewBox(int x, int y, int w, int h)
{
	static Listbox* mode = new Listbox("Type", { "Player", "Building" });
	if (!gEsp.panel.GetChildren().size())
		gEsp.panel.SetChildren({ mode });

	mode->SetWidth(w - 40);
	mode->SetPos(x + 20, y + 2);
	mode->RunControl();

	const wchar_t *wcName = L"Hold on!", *wcConds[2] = { L"Cloaked", L"Jarated" };

	static int txtw, txth;
	bool boxes = gEsp.boxes.value, conds = gEsp.conditions.value, name = gEsp.name.value;
	int health = gEsp.health.value, W = 60, H = 90;

	if (mode->value)
	{
		wcName = L"Sentry", wcConds[0] = L"Sapped", wcConds[1] = L"Idle";
		boxes = gEsp.bBoxes.value, conds = gEsp.bConds.value, health = gEsp.bHealth.value, name = gEsp.bType.value;
		H -= 20;
	}

	SColor color(0, 150, 255);
	gBase.Surface->GetTextSize(gFonts.esp, wcName, txtw, txth);

	int drawW = W + 50, drawH = H + txth + 5;
	x += (w / 2) - (drawW / 2), y += (h / 1.75) - (drawH / 2);

	int Y = y + txth;

	if (boxes)
	{
		if (gEsp.box_style.value == 2) // 3D - Plain
		{
			gDraw.OutlineRect(x, Y, W, H, color);
			gDraw.OutlineRect(x + 10, Y + 10, W - 20, H - 20, color);

			gDraw.DrawLine(x, Y, x + 10, Y + 10, color);
			gDraw.DrawLine(x + W - 10, Y + 10, x + W, Y, color);
			gDraw.DrawLine(x, Y + H, x + 10, Y + H - 10, color);
			gDraw.DrawLine(x + W - 10, Y + H - 10, x + W, Y + H, color);
		}
		else
			gEsp.DrawBox(x, Y, W, H, color);
	}

	int stack = Y - txth;
	if (health)
	{
		static SColor hp = hsv2rgb(0.3f, 0.9f, 0.8f);
		if (health == 1) // Text
			gDraw.DrawString(x + W + 3, stack += txth, hp, "150", gFonts.esp);
		else // Bar
			gDraw.DrawRect(x - 4, Y, 2, H, hp);
	}

	if (conds)
	{
		gDraw.DrawString(x + W + 3, stack += txth, color, wstring(L"*") + wcConds[0] + L"*", gFonts.esp);
		gDraw.DrawString(x + W + 3, stack += txth, color, wstring(L"*") + wcConds[1] + L"*", gFonts.esp);
	}

	if (name)
		gDraw.DrawString(x + (W / 2) - (txtw / 2), y, color, wcName, gFonts.esp);
}

bool Esp::ScreenBounds(Vector *worldpos, int amount, Vector2 &xy, Vector2 &wh)
{
	float x = gScreenSize.width, y = gScreenSize.height, w = 0, h = 0;

	// Loop through all the points to find the corners of our bounds
	for (int i = 0; i < amount; i++)
	{
		Vector screen;
		if (gDraw.WorldToScreen(worldpos[i], screen))
		{
			if (!i)
				x = screen.x, y = screen.y;

			if (screen.x < x) // Get the left-most point
				x = screen.x;
			else if (screen.x > w) // Get the right-most point
				w = screen.x;

			if (screen.y < y) // Get the top-most point
				y = screen.y;
			else if (screen.y > h) // Get the bottom-most point
				h = screen.y;
		}
	}
	w -= x, h -= y; // Change the width and height from a point to a relative width and height

	xy = Vector2(x, y), wh = Vector2(w, h);

	return x + w > 1;
}

inline void Esp::ExpandOutline(int x, int y, int w, int h, SColor color, int Thickness)
{
	gDraw.OutlineRect(x - Thickness, y - Thickness, w + Thickness * 2, h + Thickness * 2, color);
}

#include <math.h>
inline Vector2 Esp::rotate_vec2(Vector2& point, const Vector2& origin, float radians)
{
	float s = sin(radians);
	float c = cos(radians);

	// translate point back to origin:
	point.x -= origin.x;
	point.y -= origin.y;

	// rotate point
	float xnew = point.x * c - point.y * s;
	float ynew = point.x * s + point.y * c;

	// translate point back:
	point.x = xnew + origin.x;
	point.y = ynew + origin.y;
	return point;
}