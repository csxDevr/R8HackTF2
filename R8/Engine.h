#pragma once
#include "Base.h"
#include "getvfunc.h"
#include "CSignature.h"

using namespace std;

typedef float matrix3x4[3][4];

#define	FL_ONGROUND (1<<0)
#define FL_DUCKING (1<<1)
#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define BLU_TEAM 3
#define RED_TEAM 2

#define TICK_INTERVAL			(gBase.Globals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1, // (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

class RecvTable;
class ClientClass
{
private:
	BYTE _chPadding[8];
public:
	char* chName;
	RecvTable* Table;
	ClientClass* pNextClass;
	int iClassID;
};

class CHLClient
{
public:
	ClientClass * GetAllClasses(void)
	{
		typedef ClientClass* (__thiscall* OriginalFn)(void*); //Anything inside a VTable is a __thiscall unless it completly disregards the thisptr. You can also call them as __stdcalls, but you won't have access to the __thisptr.
		return getvfunc<OriginalFn>(this, 8)(this); //Return the pointer to the head CClientClass.
	}
};

#include "studio.h"
struct model_t;
class IMaterial;
class IVModelInfo
{
public:
	const char* GetModelName(DWORD* model)
	{
		typedef const char* (__thiscall* OriginalFn)(void*, DWORD*);
		return getvfunc<OriginalFn>(this, 3)(this, model);
	}
	studiohdr_t* GetStudiomodel(const DWORD *mod)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodelFn)(void*, const DWORD*);
		return getvfunc< GetStudiomodelFn >(this, 28)(this, mod);
	}
	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return getvfunc< GetModelIndexFn >(this, 2)(this, name);
	}
	void GetModelMaterials(const model_t *model, int count, IMaterial **ppMaterials)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, const model_t *, int, IMaterial **);
		return getvfunc<OriginalFn>(this, 16)(this, model, count, ppMaterials);
	}
};

class CUserCmd
{
public:
	virtual ~CUserCmd() {}; //Destructor 0
	int command_number; //4
	int tick_count; //8
	Vector viewangles; //C
	float forwardmove; //18
	float sidemove; //1C
	float upmove; //20
	int	buttons; //24
	byte impulse; //28
	int weaponselect; //2C
	int weaponsubtype; //30
	int random_seed; //34
	short mousedx; //38
	short mousedy; //3A
	bool hasbeenpredicted; //3C;
	int unk; // 40
};

class CNetChan;
class INetChannel;
typedef struct player_info_s player_info_t;

typedef struct con_nprint_s
{
	int		index;			// Row #
	float	time_to_live;	// # of seconds before it disappears. -1 means to display for 1 frame then go away.
	float	color[3];		// RGB colors ( 0.0 -> 1.0 scale )
	bool	fixed_width_font;
} con_nprint_t;

class EngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* OriginalFn)(void*, int&, int&);
		return getvfunc<OriginalFn>(this, 5)(this, width, height);
	}
	bool GetPlayerInfo(int ent_num, player_info_t *pinfo)
	{
		typedef bool(__thiscall* OriginalFn)(void*, int, player_info_t *);
		return getvfunc<OriginalFn>(this, 8)(this, ent_num, pinfo);
	}
	bool Con_IsVisible(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 11)(this);
	}
	int GetLocalPlayer(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 12)(this);
	}
	float Time(void)
	{
		typedef float(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 14)(this);
	}
	void GetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(void*, Vector& va);
		return getvfunc<OriginalFn>(this, 19)(this, va);
	}
	Vector GetViewAngles()
	{
		Vector out;
		GetViewAngles(out);
		return out;
	}
	void SetViewAngles(Vector& va)
	{
		typedef void(__thiscall* OriginalFn)(void*, Vector& va);
		return getvfunc<OriginalFn>(this, 20)(this, va);
	}
	int GetMaxClients(void)
	{
		typedef int(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 21)(this);
	}
	bool IsInGame(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 26)(this);
	}
	bool IsConnected(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 27)(this);
	}
	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 28)(this);
	}
	const matrix3x4& WorldToScreenMatrix(void)
	{
		typedef const matrix3x4& (__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 36)(this);
	}
	bool IsTakingScreenshot(void)
	{
		typedef bool(__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 85)(this);
	}
	INetChannel* GetNetChannelInfo(void)
	{
		typedef INetChannel* (__thiscall* OriginalFn)(void*);
		return getvfunc<OriginalFn>(this, 72)(this);
	}
	const char* GetProductVersionString()
	{
		typedef const char* (__thiscall* OriginalFn)(PVOID);
		return getvfunc<OriginalFn>(this, 99)(this);
	}
	void ClientCmd_Unrestricted(const char* chCommandString)
	{
		typedef void(__thiscall* OriginalFn)(void*, const char *);
		return getvfunc<OriginalFn>(this, 106)(this, chCommandString);
	}
	void CL_SendMove()
	{
		static DWORD dwSendMove = gSignatures.GetEngineSignature("55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D 4D CC");
		((void(__thiscall*)())dwSendMove)();
	}
};

enum playercontrols
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),	// Used by client.dll for when scoreboard is held down
	IN_SPEED = (1 << 17),	// Player is holding the speed key
	IN_WALK = (1 << 18),	// Player holding walk key
	IN_ZOOM = (1 << 19),	// Zoom key for HUD zoom
	IN_WEAPON1 = (1 << 20),	// weapon defines these bits
	IN_WEAPON2 = (1 << 21),	// weapon defines these bits
	IN_BULLRUSH = (1 << 22),
};

class ClientModeShared
{
public:
	bool IsChatPanelOutOfFocus(void)
	{
		typedef void*(__thiscall* OriginalFn)(void*);
		void* CHudChat = getvfunc<OriginalFn>(this, 19)(this);
		if (CHudChat)
		{
			return *(PFLOAT)((DWORD)CHudChat + 0xFC) == 0;
		}
		return false;
	}
};

class CInput
{
public:
	// get the current cmd
	CUserCmd * GetUserCmd(int seq)
	{
		typedef CUserCmd *(__thiscall * OriginalFn)(void*, int);
		return getvfunc<OriginalFn>(this, 8)(this, seq);
	}
};

class CGlobals
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

class IPlayerInfoManager
{
public:
	CGlobals * GetGlobalVars()
	{
		typedef CGlobals*(__thiscall* GetGlobalVarsFn)(void*);
		return getvfunc<GetGlobalVarsFn>(this, 1)(this);
	}
};

class CPrediction
{
public:
	void SetLocalViewAngles(Vector& ang)
	{
		typedef void(__thiscall* OriginalFn)(CPrediction*, Vector&);
		return getvfunc<OriginalFn>(this, 13)(this, ang);
	}
};

#include "CUtlVector.h"
typedef unsigned long* EHANDLE;
struct GlowObjectDefinition_t
{
	EHANDLE m_hEntity;
	Vector m_vGlowColor;
	float m_flGlowAlpha;

	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	int m_nSplitScreenSlot;

	// Linked list of free slots
	int m_nNextFreeSlot;

	// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE = -2;
};

class CGlowManager
{
public:
	CUtlVector<GlowObjectDefinition_t> glowObjects;
};
extern CGlowManager* pGlowManager;