#pragma once
#include "Cheat.h"
#include "Controls.h"

class Trigger : public CBaseCheat
{
public:
	// General

	Checkbox backstab = Checkbox("Auto backstab");
	Listbox airblast = Listbox("Auto airblast", { "Off", "Legit", "Rage - Loud", "Rage - Silent" });

	// Auto shooting

	KeyBind enabled = KeyBind("Enabled", VK_LSHIFT);
	Listbox hitbox = Listbox("Hitbox", { "Head", "Torso", "Feet", "All" });

	void Command(CUserCmd* cmd, CPlayer* You, int iLocal, bool* bSendPacket);

	CHEAT_INIT(Trigger, ch_Command)
private:
	bool ShouldTarget(CPlayer* Player);
};
extern Trigger gTrigger;