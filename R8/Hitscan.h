#pragma once
#include "Base.h"
#include "Ray tracing.h"

class CTraceFilterCustom : public ITraceFilter
{
public:
	bool ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask);
	TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
	CBaseEntity* you;
	bool predVis = false;

	CTraceFilterCustom(CBaseEntity* You, CBaseEntity* Ignore = nullptr, bool PredictionMode = false)
	{
		you = You, pSkip = Ignore, predVis = PredictionMode;
	}
};

class Hitscan
{
public:
	// - Returns true if we hit the target entity from vStart to vEnd
	bool CanHit(CBaseEntity* you, void* target, Vector vStart, Vector vEnd, trace_t *result = nullptr);

	// - Returns true if nothing will stop our bullet from vStart to vEnd
	bool AssumeVis(CBaseEntity* you, Vector vStart, Vector vEnd, trace_t *result = nullptr);

	// - Returns the projectile speed in hammer units
	// - Will return false for unsupported weapons
	int ProjSpeed(CBaseEntity* weapon);
};
extern Hitscan gHit;