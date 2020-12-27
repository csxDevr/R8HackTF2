#pragma once
#include "Cheat.h"
#include "Controls.h"
class CVisualEffects : CBaseCheat
{
	CHEAT_INIT(CVisualEffects, ch_FSN)
public:
	ColorPicker worldclr = ColorPicker("World color", SColor(0));
	void FrameStageNotify(ClientFrameStage_t Stage);
};
extern CVisualEffects gVFX;