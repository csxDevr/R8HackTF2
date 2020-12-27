#pragma once
#include "Base.h"
struct DrawModelState_t;
struct ModelRenderInfo_t;
typedef float matrix3x4[3][4];

typedef void(__thiscall *DrawModelExecuteFn)(CModelRender*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);

void __stdcall Hooked_DrawModelExecute(const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);