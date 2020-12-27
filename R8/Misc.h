#pragma once
#include "Cheat.h"
#include "FrameStageNotify.h"
#include "Controls.h"
#include <deque>

class ITexture;
class CUserCmd;
class Misc : public CBaseCheat
{
	CHEAT_INIT(Misc, ch_Draw | ch_Command | ch_FSN)// | ch_DrawPlayers)
public:
	// General

	Checkbox bhop = Checkbox("Bunnyhop");
	Checkbox nopush = Checkbox("No push");
	Checkbox crouchwalk = Checkbox("Crouch walk");
	//KeyBind teleport = KeyBind("Teleport", 'R');
	//Checkbox debugcmd = Checkbox("Debug UserCmd");

	Listbox lagmode = Listbox("Server lag mode", { "Off", "Basic - Weaker", "Unreliable - Stronger" });
	Listbox lagcmd = Listbox("Server lagger cmd", { "use", "voicemenu 0 0", "spec_next", "lastinv" });
	Slider lagexploit = Slider("Server lagger", 0, 0, 512, 4);

	// Visuals

	KeyBind thirdperson = KeyBind("Third person key", (byte)'T');
	Listbox paths = Listbox("Prediction path", { "Off", "Normal", "Strafe" });
	Checkbox cam = Checkbox("Cam");
	Slider fov = Slider("Custom FOV", 0, 0, 179, 1);
	Listbox noscope = Listbox("Remove scope", { "Disabled", "No scope", "No zoom", "Both" });
	Listbox utaunt = Listbox("Unusual taunt", { "None", "Sparkle", "Fire", "Lanterns", "Electricity", "Pixels", "Disco", "Lights", "Fireworks" });

	Vector predPosAt(float flTime, CPlayer* target);

	void Draw(CPlayer* you);
	void Command(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket);
	void FrameStageNotify(CPlayer* You, ClientFrameStage_t Stage);

	ITexture* mirrorTexture;
private:
	deque<CUserCmd*> commands;
};
extern Misc gMisc;