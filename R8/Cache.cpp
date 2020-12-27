#include "Cache.h"
#include "Entity.h"
#include "Client.h"

#include "R8_inetchannel.h"

CCache gCache;

Vector PlayerCache::GetHitbox(int tick, int i)
{
	if (i < 0 || i > 17) i = 0; // Quick sanity check

	const HitboxData* bone = FindTick(tick);
	if (!bone)
		return Vector();

	return GetHitbox(*bone, i);
}

Vector PlayerCache::GetHitbox(const HitboxData& Bone, int i)
{
	if (i < 0 || i > 17) i = 0; // Quick sanity check

	DWORD *model = player->GetModel();
	if (!model)
		return Vector();

	studiohdr_t *hdr = gBase.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector();

	int HitboxSetIndex = *(int *)((DWORD)hdr + 0xB0);
	if (!HitboxSetIndex)
		return Vector();

	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)hdr) + HitboxSetIndex);
	mstudiobbox_t* box = pSet->pHitbox(i);
	if (!box)
		return Vector();

	Vector vCenter = (box->bbmin + box->bbmax) * 0.5f;

	Vector vHitbox;
	VectorTransform(vCenter, Bone.matrix[box->bone], vHitbox);

	return vHitbox;
}

HitboxData* PlayerCache::FindTick(int tick)
{
	for (auto& bone : bones)
		if (bone.tick == tick)
			return &bone;
	return nullptr;
}

void PlayerCache::Update(CPlayer* Player)
{
	if (Player != nullptr)
		player = Player;
	else if (player == nullptr)
		return;
	
	if (player->Lifestate() != LIFE_ALIVE)
		bones.clear();

	index = player->GetIndex();
	guid = player->Info().userID;

	bones.push_front(HitboxData());
	if (bones.size() > MAX_CACHE)
		bones.pop_back();

	HitboxData* bone = &bones.front();
	player->SetupBones(bone[0].matrix, 128, 0x100, gBase.Globals->curtime);
	bone[0].vCenter = player->GetWorldCenter(), bone[0].tick = gCache.GetTick();
}

PlayerCache::PlayerCache(CPlayer* Player)
{
	if (Player == nullptr)
		return;

	Update(Player);
}

bool PlayerCache::Full()
{
	return bones.size() >= MAX_CACHE;
}

PlayerCache* CCache::FindIndex(int Index)
{
	for (auto& player : players)
		if (player.player && player.index == Index)
			return &player;
	return nullptr;
}

PlayerCache * CCache::FindUID(int GUID)
{
	for (auto& player : players)
		if (player.player && player.guid == GUID)
			return &player;
	return nullptr;
}

PlayerCache* CCache::FindPlayer(CPlayer* Player)
{
	for (auto& player : players)
		if (player.player && player.player == Player)
			return &player;
	return nullptr;
}

PlayerCache* CCache::FindSlot()
{
	for (auto& player : players)
		if (player.player == nullptr)
			return &player;
	return nullptr;
}

int CCache::GetTick()
{
	return lastChlTick;
}

void CCache::Update(CPlayer* You, CUserCmd* cmd)
{
	tick = cmd->tick_count;

	for (auto& player : players)
	{
		CPlayer* pPlayer;
		if (!GetBaseEntity(player.index)->ToPlayer(pPlayer))
			player.player = nullptr;
	}

	for (int i = 1; i <= gBase.Engine->GetMaxClients(); i++)
	{
		//if (i == me)
			//continue;
		CPlayer* pPlayer;
		if (!GetBaseEntity(i)->ToPlayer(pPlayer))
			continue;
		if (pPlayer->Team() == You->Team() && i != me)
			continue;

		bool found = false;
		for (auto& player : players)
		{
			if (pPlayer->Info().userID == player.guid)
			{
				player.Update(pPlayer);
				found = true;
				break;
			}
		}
		if (found)
			continue;

		PlayerCache cache(pPlayer), *pCache = FindSlot();
		if (pCache)
			*pCache = cache;
		else
			players.push_back(cache);
	}
}

#include "Aim.h"
int CCache::ApproxLostTicks()
{
	INetChannel* netchan = gBase.Engine->GetNetChannelInfo();
	float lost_time;

	lost_time = netchan->GetLatency(FLOW_INCOMING) + netchan->GetLatency(FLOW_OUTGOING);

	return TIME_TO_TICKS(ceil(lost_time * 10));
}