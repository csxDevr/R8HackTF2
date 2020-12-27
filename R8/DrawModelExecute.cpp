#include "DrawModelExecute.h"
#include "VMTHooks.h"
#include "Entity.h"
#include "ESP.h"
#include "Materials.h"
#include "CDraw.h"
#include "Cache.h"
#include "CMat.h"
#include "Aim.h"
#include "Cheat.h"

void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	VMTManager* hook = &VMTManager::GetHook(gBase.ModelRender);
	auto CallOriginal = hook->GetMethod<DrawModelExecuteFn>((int)e_offset::DrawModelExecute);
	try
	{
		CPlayer *pLocal;
		if (pInfo.pModel && GetBaseEntity(me)->ToPlayer(pLocal))
			gEsp.DME(state, pInfo, pCustomBoneToWorld, pLocal, CallOriginal);
		CBaseCheat::RunDrawModelExecute(state, pInfo, pCustomBoneToWorld);
	}
	catch (...)
	{
		CATCH_WARN();
		return CallOriginal(gBase.ModelRender, state, pInfo, pCustomBoneToWorld);
	}
	CallOriginal(gBase.ModelRender, state, pInfo, pCustomBoneToWorld);
	gMat.ResetMaterial();
}