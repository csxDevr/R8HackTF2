#pragma once
#include "Base.h"
#include "Controls.h"
#include "Cache.h"

class CGameTrace;
typedef CGameTrace trace_t;

class Aim
{
public:
	// General

	KeyBind enabled = KeyBind("Enabled", VK_SHIFT);

	Checkbox loud = Checkbox("Loud aim", false);
	Checkbox hitscan = Checkbox("Hitscan");
	Slider fov = Slider("FOV limit", 90, 0, 180, 1);
	Listbox hitbox = Listbox("Priority", { "Head", "Body", "Feet", "Auto" }, 3);
	KeyBind autoshoot = KeyBind("Autoshoot", 'F');

	// Extra

	Checkbox scope = Checkbox("Scoped aim only");
	Slider predict = Slider("Prediction iterate", 2, 0, 5, 1);

	// ESP options

	Checkbox target = Checkbox("Highlight target", true);
	Listbox cache = Listbox("Show backtrack", { "Off", "Chams - Target", "Chams - Everyone", "Dots - Target", "Dots - Everyone" });

	// Back tracking

	Listbox smarttrack = Listbox("Back-tracking", {"Off", "Always", "Auto"});
	Slider backtrack = Slider("Back-track tick", 13, 1, 0 /* We will set this later */, 1);

	// This is for ESP to highlight our aim target
	int targetIndex;
	// Check if the aimbot has decided to backtrack
	int doBacktrack;
	// Find the angles we're currently using
	Vector lastAngles;

	void command(CPlayer* you, CUserCmd* cmd, bool* bSendPacket = nullptr);
	void SilentFix(CUserCmd *Cmd, Vector vOldAng);

	// - Get angles from the camera to desired world pos
	Vector AnglesTo(const Vector& Pos, CPlayer* You);
	// - Returns true when aimbot wants to choke packets
	inline bool Choke()
	{
		bool bReturn = bChoke;
		bChoke = false;
		return bChoke;
	}
	// - Returns true if the player can shoot
	inline bool CanShoot() { return bCanShoot; }
	// - Calculate for CanShoot() every tick
	bool CalcCanShoot(CPlayer* You);
	
private:
	bool bChoke = false, bCanShoot = false;

	enum class e_aim : byte
	{
		priority,
		hitscan,
		predict,
		backtrack,
	};

	// - Returns an entity index above zero if a target was found
	// - "Angles" will be set to viewangles aiming at the target
	int FindTarget(CPlayer* you, Vector& Angles);
	// - Checks for a valid shot. Method depends on your "Option" value
	// - "Pos" will be set to the position of a valid hit.
	bool ScanForHit(CPlayer* you, CPlayer* Target, e_aim Option, Vector& Pos);
	// - Returns a list of hitboxes
	// - Optimized based on your settings
	const vector<int>& BestHitboxes();
};
extern Aim gAim;