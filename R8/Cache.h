#pragma once
#include "Engine.h"
#include <list>

#define MAX_CACHE TIME_TO_TICKS(1)

class CPlayer;

struct HitboxData
{
	Vector vCenter;
	matrix3x4 matrix[128];
	int tick;
};

class CCache;
class PlayerCache
{
public:
	CPlayer* player = nullptr;
	int index;
	int guid;
	bool bHit = false;
	byte resolve = 0;
	byte attempted = 0;

	list<HitboxData> bones;
	Vector GetHitbox(int tick, int i);
	Vector GetHitbox(const HitboxData& Bone, int i);
	
	bool Full();
	HitboxData* FindTick(int tick);
	void Update(CPlayer* Player = nullptr);

	PlayerCache(CPlayer* Player);
};

class CCache
{
	int tick = 0;
public:
	int GetTick();
	int ApproxLostTicks();

	list<PlayerCache> players;
	PlayerCache* FindSlot();

	PlayerCache* FindIndex(int Index);
	PlayerCache* FindUID(int GUID);
	PlayerCache* FindPlayer(CPlayer* Player);

	void Update(CPlayer* You, CUserCmd* cmd);
};

extern CCache gCache;