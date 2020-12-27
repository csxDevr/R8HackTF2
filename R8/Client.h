#pragma once

extern int lastChlTick;

class CUserCmd;
bool __fastcall Hooked_CreateMove(void* ClientMode, int edx, float input_sample_frametime, CUserCmd* pCommand);
//void __fastcall Hooked_CHLCreateMove(void* ecx, void* edx, int sequence_number, float input_sample_time, bool active);