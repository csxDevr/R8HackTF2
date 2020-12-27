#include "CMat.h"
#include "R8 KeyValues.h"
#include "Materials.h"

CMat gMat;

void CMat::Initialize()
{
	CHECKPTR(glow = gBase.MatSystem->FindMaterial("dev/glow_color", "Model textures"));
	CHECKPTR(shaded = CreateMaterial(false, false));
	CHECKPTR(shiny = CreateMaterial(false, true, false, true));
	CHECKPTR(wireframe = CreateMaterial(true, true, true));
}

Matptr CMat::CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe, bool Shiny)
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"%s\"\
		\n\t\"$normalmapalphaenvmapmask\" \"%i\"\
		\n\t\"$envmapcontrast\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};

	char* baseType = (Flat ? "UnlitGeneric" : "VertexLitGeneric");
	char material[512];
	sprintf_s(material, sizeof(material), tmp, baseType, Shiny ? "env_cubemap" : "", Shiny ? 1 : 0, Shiny ? 1 : 0, (IgnoreZ) ? 1 : 0, (Wireframe) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "#devrnull_mat_%i.vmt", created);
	created++;

	KeyValues* keyValues = (KeyValues*)malloc(sizeof(KeyValues));
	KeyValues::Initialize(keyValues, baseType);
	KeyValues::LoadFromBuffer(keyValues, name, material);

	IMaterial *createdMaterial = gBase.MatSystem->CreateMaterial(name, keyValues);
	if (!createdMaterial)
		return nullptr;

	createdMaterial->IncrementReferenceCount();

	return createdMaterial;
}

void CMat::ForceMaterial(SColor color, Matptr material, bool useColor, bool forceMaterial)
{
	if (useColor)
	{
		float blend[3] = { (float)color[0] / 255.f, (float)color[1] / 255.f, (float)color[2] / 255.f };
		float alpha = (float)color[3] / 255.f;

		gBase.RenderView->SetBlend(alpha);
		gBase.RenderView->SetColorModulation(blend);
	}

	if (forceMaterial)
		gBase.ModelRender->ForcedMaterialOverride(material);
}

void CMat::ResetMaterial()
{
	static const float flDefault[3] = { 1, 1, 1 };
	gBase.RenderView->SetBlend(1);
	gBase.RenderView->SetColorModulation(flDefault);
	gBase.ModelRender->ForcedMaterialOverride(NULL);
}