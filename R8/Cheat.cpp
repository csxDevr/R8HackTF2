#include "Cheat.h"

#define RUN_CHEATS(CheatType, Function)			\
	for (auto cheat : GetCheats())				\
		if (cheat->iFlags & CheatType)			\
			cheat->Function

void CBaseCheat::RunDraw(CPlayer* You)
{
	RUN_CHEATS(ch_Draw, Draw(You));
}
void CBaseCheat::RunDrawEntity(CPlayer* You, CBaseEntity* Entity, int Index)
{
	RUN_CHEATS(ch_DrawEntities, DrawEntity(You, Entity, Index));
}
void CBaseCheat::RunDrawPlayer(CPlayer* You, CPlayer* Player, int Index)
{
	RUN_CHEATS(ch_DrawPlayers, DrawPlayer(You, Player, Index));
}
void CBaseCheat::RunCommand(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket)
{
	RUN_CHEATS(ch_Command, Command(cmd, You, iLocal, bSendPacket));
}
void CBaseCheat::RunCHL(CUserCmd* cmd, CPlayer* You, int iLocal)
{
	RUN_CHEATS(ch_CHL, CHL(cmd, You, iLocal));
}
void CBaseCheat::RunFrameStageNotify(CPlayer* You, ClientFrameStage_t Stage)
{
	RUN_CHEATS(ch_FSN, FrameStageNotify(You, Stage));
}

void CBaseCheat::RunDrawModelExecute(const DrawModelState_t & state, const ModelRenderInfo_t & pInfo, matrix3x4 * pCustomBoneToWorld)
{
	RUN_CHEATS(ch_DME, DrawModelExecute(state, pInfo, pCustomBoneToWorld));
}
