#include "VisualEffects.h"
#include "Entity.h"
#include "Materials.h"
#include "CMat.h"

CVisualEffects gVFX;

unordered_map<MaterialHandle_t, SColor> worldmat_new;
unordered_map<MaterialHandle_t, SColor> worldmat_old;

void CVisualEffects::FrameStageNotify(ClientFrameStage_t Stage)
{
	if (!gBase.Engine->IsInGame() && worldmat_new.size())
	{
		for (auto& hMat : worldmat_old) // Reset all old materials
		{
			IMaterial* mat = gBase.MatSystem->GetMaterial(hMat.first);
			SColor& color = hMat.second;
			float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
			mat->ColorModulate(blend[0], blend[1], blend[2]);
			mat->AlphaModulate(blend[3]);
		}

		worldmat_new.clear();	// Clear cache of materials
		worldmat_old.clear();	//
	}

	if (Stage != ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
		return;

	for (MaterialHandle_t i = gBase.MatSystem->FirstMaterial(); i != gBase.MatSystem->InvalidMaterial(); i = gBase.MatSystem->NextMaterial(i))
	{
		IMaterial* mat = gBase.MatSystem->GetMaterial(i);

		if (!mat || strcmp(mat->GetTextureGroupName(), TEXTURE_GROUP_WORLD))
			continue;

		if (worldmat_new.find(i) == worldmat_new.end())
		{
			float r, g, b, a = mat->GetAlphaModulation();
			mat->GetColorModulation(&r, &g, &b);

			SColor old_color(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
			worldmat_new.emplace(i, old_color);
			worldmat_old.emplace(i, old_color);
		}

		SColor color = worldclr.bDef ? worldmat_old.at(i) : worldclr.color;
		if (worldmat_new.at(i) != color)
		{
			float blend[4] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f, (float)color[3] / 255.f };
			mat->ColorModulate(blend[0], blend[1], blend[2]);
			mat->AlphaModulate(blend[3]);

			worldmat_new.at(i) = color;
		}
	}
}