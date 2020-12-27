#pragma once

enum ClientFrameStage_t;

void __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void Original_FrameStageNotify(ClientFrameStage_t curStage, bool bSet = true);