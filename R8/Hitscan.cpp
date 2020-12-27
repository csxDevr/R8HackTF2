#include "Hitscan.h"
#include "Entity.h"

Hitscan gHit;

int Hitscan::ProjSpeed(CBaseEntity* weapon)
{
	if (weapon == NULL)
		return 0;
	ent_id name = (ent_id)weapon->GetClientClass()->iClassID;
	if (name == ent_id::CTFRocketLauncher_DirectHit)
		return 1980;
	else if (name == ent_id::CTFRocketLauncher_AirStrike ||
		name == ent_id::CTFParticleCannon ||
		name == ent_id::CTFRocketLauncher)
		return 1100;
	else if (name == ent_id::CTFRaygun)
		return 840;
	return false; // Return this so we know if the weapon isn't supported
}

bool Hitscan::CanHit(CBaseEntity* you, void* target, Vector vStart, Vector vEnd, trace_t *result)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilterCustom Filter(you);

	Ray.Init(*(Vector*)&vStart, *(Vector*)&vEnd);
	gBase.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result)
		*result = Trace;

	return Trace.m_pEnt == target;
}

bool Hitscan::AssumeVis(CBaseEntity* you, Vector vStart, Vector vEnd, trace_t *result)
{
	trace_t Trace;
	Ray_t Ray;
	CTraceFilterCustom Filter(you, nullptr, true);

	Ray.Init(*(Vector*)&vStart, *(Vector*)&vEnd);
	gBase.EngineTrace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace);

	if (result)
		*result = Trace;

	return !Trace.DidHit();
}

bool CTraceFilterCustom::ShouldHitEntity(CBaseEntity* pEntityHandle, int contentsMask)
{
	CBaseEntity* pEntity = (CBaseEntity *)pEntityHandle;
	ent_id type = pEntity->Type();

	if (predVis && type == ent_id::CTFPlayer)
		return false;

	switch (type)
	{
	case ent_id::CFuncAreaPortalWindow:
	case ent_id::CFuncRespawnRoomVisualizer:
	case ent_id::CSniperDot:
	case ent_id::CTFMedigunShield:
		return false;
	}

	bool shouldHit = true;
	if (you)
		shouldHit = pEntity != you;
	if (shouldHit && pSkip)
		shouldHit = pEntity != pSkip;
	return shouldHit;
}