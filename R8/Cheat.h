#pragma once
#include "Base.h"

// - Initializes a CBaseCheat class
// - This allows the class to run automatically
#define CHEAT_INIT(Name, Flags)	\
public:							\
	Name()						\
	{							\
		SetFlags(Flags);		\
	}

class CBaseEntity;
class CBaseCheat;
class CUserCmd;
class CPlayer;

enum e_cheat
{
	// - Completely useless. Only used as a blank class's default value
	ch_None = 0,
	// - Draw on the screen
	ch_Draw = (1 << 0),
	// - Draw individual players
	ch_DrawPlayers = (1 << 1),
	// - Draw individual entities (Not players)
	ch_DrawEntities = (1 << 2),
	// - Control the player from CreateMove
	ch_Command = (1 << 3),
	// - Control the player from CHL Createmove
	ch_CHL = (1 << 4),
	// - Modify game information in FrameStageNotify
	ch_FSN = (1 << 5),
	// - Draw models or modify existing materials and models
	ch_DME = (1 << 6)
};

#include <list>
enum ClientFrameStage_t;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class CBaseCheat
{
	e_cheat iFlags = ch_None;
	static list<CBaseCheat*>& GetCheats() { static list<CBaseCheat*> cheats; return cheats; }
protected:
	inline void SetFlags(int Flags) { iFlags = (e_cheat)Flags; }
public:
	CBaseCheat()
	{
		GetCheats().push_back(this);
	}
	~CBaseCheat()
	{
		GetCheats().remove(this);
	}

	// - Get the flags that determine a cheat's functionality
	inline e_cheat GetFlags() { return iFlags; }
	// - Get a list of all running cheats
	static inline const list<CBaseCheat*>& Cheats() { return GetCheats(); }

	virtual void Draw(CPlayer* You) {}
	virtual void DrawEntity(CPlayer* You, CBaseEntity* Entity, int Index) {}
	virtual void DrawPlayer(CPlayer* You, CPlayer* Player, int Index) {}
	virtual void Command(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket) {}
	virtual void CHL(CUserCmd* cmd, CPlayer* You, int iLocal) {}
	virtual void FrameStageNotify(CPlayer* You, ClientFrameStage_t Stage) {}
	virtual void DrawModelExecute(const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL) {}

	static void RunDraw(CPlayer* You);
	static void RunDrawEntity(CPlayer* You, CBaseEntity* Entity, int Index);
	static void RunDrawPlayer(CPlayer* You, CPlayer* Player, int Index);
	static void RunCommand(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket);
	static void RunCHL(CUserCmd* cmd, CPlayer* You, int iLocal);
	static void RunFrameStageNotify(CPlayer* You, ClientFrameStage_t Stage);
	static void RunDrawModelExecute(const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld = NULL);
};