#pragma once
#include "Engine.h"
#include "getvfunc.h"

#include "Netvars.h"
#include "Offsets.h"

class CBaseEntity;
class CPlayer;

class CEntList
{
public:
	CBaseEntity* GetClientEntity(int entnum)
	{
		typedef CBaseEntity* (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 3)(this, entnum);
	}
	CBaseEntity* GetClientEntityFromHandle(int hEnt)
	{
		typedef CBaseEntity* (__thiscall* OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 4)(this, hEnt);
	}
	int GetHighestEntityIndex(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 6)(this);
	}
};

#define me gBase.Engine->GetLocalPlayer()
#define GetBaseEntity gBase.EntList->GetClientEntity
#define GetEntHandle gBase.EntList->GetClientEntityFromHandle

#define GetNV(type, offset) (type*)(this + offset)
#define OFF_RET(name, type, offset)		\
inline type* name()						\
{										\
	return GetNV(type, offset);			\
}
#define NV_RET(name, type, offset)	\
inline type name()					\
{									\
	return *GetNV(type, offset);	\
}
#define pNV_RET(name, type, offset) \
OFF_RET(p##name, type, offset)		\
NV_RET(name, type, offset)


#include "Entity enums.h"
class CBaseEntity
{
public:
	// - Gets the origin position (usually the bottom) of the entity
	Vector& GetAbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 9)(this);
	}
	Vector& GetAbsAngles()
	{
		typedef Vector& (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 10)(this);
	}
	void SetAbsAngles(Vector Angle)
	{
		typedef void(__thiscall* SetAngleFn)(CBaseEntity*, const Vector&);
		static SetAngleFn SetAngle = (SetAngleFn)gOffsets.SetAbsAngles;
		SetAngle(this, Angle);
	}
	// - Gets the center position of the entity
	Vector GetWorldCenter()
	{
		Vector vMin, vMax, output = this->GetAbsOrigin();
		this->GetRenderBounds(vMin, vMax);
		output.z += (vMin.z + vMax.z) / 2;

		return output;
	}
	DWORD* GetModel()
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef DWORD* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}
	int DrawModel(int Flags)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef int(__thiscall* OriginalFn)(PVOID, int);
		return getvfunc<OriginalFn>(pRenderable, 10)(pRenderable, Flags);
	}
	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(PVOID, matrix3x4*, int, int, float);
		return getvfunc<OriginalFn>(pRenderable, 16)(pRenderable, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
	ClientClass* GetClientClass()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	}
	bool IsDormant()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 8)(pNetworkable);
		return false;
	}
	int GetIndex()
	{
		PVOID pNetworkable = (PVOID)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pNetworkable, 9)(pNetworkable);
	}
	void GetRenderBounds(Vector& mins, Vector& maxs)
	{
		PVOID pRenderable = (PVOID)(this + 0x4);
		typedef void(__thiscall* OriginalFn)(PVOID, Vector&, Vector&);
		getvfunc<OriginalFn>(pRenderable, 20)(pRenderable, mins, maxs);
	}
	matrix3x4& GetRgflCoordinateFrame()
	{
		PVOID pRenderable = static_cast<PVOID>(this + 0x4);
		typedef matrix3x4 &(__thiscall * OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(pRenderable, 34)(pRenderable);
	}
	
	// - Gets the entity type
	inline ent_id Type()
	{
		return (ent_id)this->GetClientClass()->iClassID;
	}
	// - Returns true if this entity is a player,
	//   and sets it to the specified player pointer
	bool ToPlayer(CPlayer* &player)
	{
		if (this != nullptr && !this->IsDormant() && this->Type() == ent_id::CTFPlayer)
		{
			player = (CPlayer*)this;
			return true;
		}
		return false;
	}
	// - Gets the entity's team
	// - Example:
	//		if (player->Team() == 2) string teamName = "BLU"
	NV_RET(Team, byte, nEntity.m_iTeamNum);
	NV_RET(GetSimulationTime, float, nEntity.m_flSimulationTime)
	NV_RET(GetCollideableMins, Vector, nEntity.m_vecMins)
	NV_RET(GetCollideableMaxs, Vector, nEntity.m_vecMaxs)
	
	Vector EstimateAbsVelocity()
	{
		typedef void(__thiscall * EstimateAbsVelocityFn)(CBaseEntity *, Vector &);
		static EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)gOffsets.EstimateAbsVelocity;

		Vector v;
		vel(this, v);

		return v;
	}
};

class CBaseObject : public CBaseEntity
{
public:
	NV_RET(Health, int, nObject.m_iHealth)
	NV_RET(MaxHealth, int, nObject.m_iMaxHealth)
	NV_RET(Sapped, bool, nObject.m_bHasSapper)
	NV_RET(PlasmaDisabled, bool, nObject.m_bPlasmaDisable)
	NV_RET(Disabled, bool, nObject.m_bDisabled)
};

class CAttributeList;
class CKnife;
class CCombatWeapon : public CBaseEntity
{
public:
	NV_RET(NextPrimaryAttack, float, nWeapon.m_flNextPrimaryAttack)
	NV_RET(LastFireTime, float, nWeapon.m_flLastFireTime)
	NV_RET(Clip1, int, nWeapon.m_iClip1)
	pNV_RET(ItemID, int, nWeapon.m_iItemDefinitionIndex)

	// - Returns true if this entity is a knife,
	//   and sets it to the specified knife pointer
	bool ToKnife(CKnife* &Knife)
	{
		if (this != nullptr && !this->IsDormant() && this->Type() == ent_id::CTFKnife)
		{
			Knife = (CKnife*)this;
			return true;
		}
		return false;
	}
};

typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class CPlayer : public CBaseEntity
{
public:
	// - Gets the player's flags
	// - Example:
	//		if (player->Flags() & FL_ONGROUND) //...
	NV_RET(Flags, int, nPlayer.m_fFlags)
	// - Gets the player's conditions
	// - Example:
	//		if (player->Conditions() & tf_cond::zoomed) then...
	NV_RET(Conditions, int, nPlayer.m_nPlayerCond)
	// - Gets the player's class
	NV_RET(Class, tf_classes, nPlayer.m_iClass)
	// - Gets the player's state of living
	NV_RET(Lifestate, source_lifestates, nPlayer.m_lifeState)
	// - Gets the player's health
	NV_RET(Health, int, nPlayer.m_iHealth)
	// - Get or set the player's glow status
	pNV_RET(GlowEnabled, bool, nPlayer.m_bGlowEnabled)
	// - Gets the player's viewangles
	pNV_RET(Viewangles, Vector, nPlayer.m_angEyeAngles)
	// - Pointer to a handle array that contains your currently equipped weapons
	OFF_RET(MyWeapons, size_t, nPlayer.m_hMyWeapons)
	// - Gets the player's attribute list
	OFF_RET(AttributeList, CAttributeList, nPlayer.m_AttributeList)

	// - Returns false if this entity is null or not a player
	bool Valid()
	{
		return this && !this->IsDormant() && this->Type() == ent_id::CTFPlayer;
	}
	// - Gets the player's max health
	int MaxHealth()
	{
		typedef int(__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 107)(this);
	}

	// - Get the player's class name
	const char* StrClass()
	{
		switch (Class())
		{
		case tf_classes::TF2_Scout: return "Scout";
		case tf_classes::TF2_Sniper: return "Sniper";
		case tf_classes::TF2_Soldier: return "Soldier";
		case tf_classes::TF2_Demoman: return "Demoman";
		case tf_classes::TF2_Medic: return "Medic";
		case tf_classes::TF2_Heavy: return "Heavy";
		case tf_classes::TF2_Pyro: return "Pyro";
		case tf_classes::TF2_Spy: return "Spy";
		case tf_classes::TF2_Engineer: return "Engineer";
		}
		return nullptr;
	}
	
	// - Gets the player's info
	// - Example:
	//		string name = player->Info().name
	player_info_s Info()
	{
		player_info_s pInfo;
		gBase.Engine->GetPlayerInfo(this->GetIndex(), &pInfo);
		return pInfo;
	}

	// - Gets the player's first person camera position
	inline Vector EyePos()
	{
		Vector* localOrigin = (Vector*)(this + nEntity.m_vecOrigin);
		return *(Vector*)(this + nPlayer.m_vecViewOffset) + *localOrigin;
	}
	// - Gets a pointer to the player's current weapon
	inline CCombatWeapon* ActiveWeapon()
	{
		return (CCombatWeapon*)GetEntHandle(*GetNV(size_t, nPlayer.m_hActiveWeapon));
	}
	// - Gets the center position of i hitbox
	// - i range: Min = 0, Max = 17
	Vector HitboxPos(int i)
	{
		if (i < 0 || i > 17) i = 0; // Quick sanity check

		DWORD *model = this->GetModel();
		if (!model)
			return Vector();

		studiohdr_t *hdr = gBase.ModelInfo->GetStudiomodel(model);
		if (!hdr)
			return Vector();

		matrix3x4 matrix[128];
		if (!this->SetupBones(matrix, 128, 0x100, gBase.Globals->curtime))
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
		VectorTransform(vCenter, matrix[box->bone], vHitbox);

		return vHitbox;
	}

	// 290	C_BaseCombatCharacter::UpdateGlowEffect(void)	!! Incorrect !!
	void UpdateGlowEffect()
	{
		typedef void(__thiscall* OriginalFn)(CPlayer*);
		return getvfunc<OriginalFn>(this, 226)(this);
	}
	// 291	C_BaseCombatCharacter::DestroyGlowEffect(void)	!! Incorrect !!
	void DestroyGlowEffect()
	{
		typedef void(__thiscall* OriginalFn)(CPlayer*);
		return getvfunc<OriginalFn>(this, 227)(this);
	}
	// 252	C_TFPlayer::UpdateClientSideAnimation(void)
	void UpdateClientSideAnimations(int unknown = 0)
	{
		typedef void(__thiscall* OriginalFn)(CPlayer*, int);
		return getvfunc<OriginalFn>(this, 252)(this, unknown);
	}
	// 55	C_TFPlayer::DrawModel(int)
	int DrawPlayerModel(int flags)
	{
		typedef int(__thiscall* OriginalFn)(CPlayer*, int unk, double unk1, double unk2, int);
		return getvfunc<OriginalFn>(this, 55)(this, 0, 0, 0, flags);
	}
};

class CKnife : public CCombatWeapon
{
public:

	NV_RET(ReadyToBackstab, bool, nWeapon.m_bReadyToBackstab)

	bool CalcBackstab(CPlayer* pSpy, CPlayer *pVictim, Vector vCenter)
	{
		Vector2 wsc_spy_to_victim = (vCenter - pSpy->GetWorldCenter()).AsVector2D();
		wsc_spy_to_victim.NormalizeInPlace();

		Vector temp1; AngleVectors(*pSpy->pViewangles(), &temp1);
		Vector2 eye_spy = temp1.AsVector2D();
		eye_spy.NormalizeInPlace();

		Vector temp2; AngleVectors(*pVictim->pViewangles(), &temp2);
		Vector2 eye_victim = temp2.AsVector2D();
		eye_victim.NormalizeInPlace();

		if (wsc_spy_to_victim.Dot(eye_victim) <= 0.0f) return false;
		if (wsc_spy_to_victim.Dot(eye_spy) <= 0.5f) return false;
		if (eye_spy.Dot(eye_victim) <= -0.3f) return false;

		return true;
	}
};

class CTEFireBullets
{
	CTEFireBullets() {};
	~CTEFireBullets() {};
public:
	void** vmt; // 0x0
private:
	char pad01[0xC];
public:
	int m_iPlayer; // 0x10
	Vector m_vecOrigin; // 0x14
	float m_vecAngles[2]; // 0x24
	int m_iWeaponID; // 0x2C
	int m_iMode; // 0x30
	int m_iSeed; // 0x34
	float m_flSpread; // 0x38
	bool m_bCritical; // 0x3C
};