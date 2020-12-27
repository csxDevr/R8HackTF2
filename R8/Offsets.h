#pragma once
#include <Windows.h>

#define CHECKPTR(Pointer) (gOffsets.SafetyCheck(Pointer))
class COffsets
{
public:
	void GetOffsets();

	DWORD EstimateAbsVelocity;
	DWORD SetAbsAngles;
	DWORD ForceFullUpdate;
	DWORD LoadFromBuffer;
	DWORD KeyValues;
	DWORD g_pGlowManager;
	DWORD dwWriteUserCmd;

	void SafetyCheck(void* Pointer)
	{
		if (Pointer)
			return;

		MessageBox(NULL, "R8 failed to get an important pointer!\nThis is mostly caused by an outdated cheat.\nThe game will now close...", "Fatal error", MB_OK);
		exit(0);
	}
	inline void SafetyCheck(DWORD Pointer)
	{
		return SafetyCheck((void*)Pointer);
	}
};
extern COffsets gOffsets;