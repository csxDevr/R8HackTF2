#pragma once
#include "Controls.h"
#include "FrameStageNotify.h"

class ProNCC
{
public:
	KeyBind resolver = KeyBind("Resolver", VK_XBUTTON1);

	Checkbox manual = Checkbox("Manual resolve");
	KeyBind manualp = KeyBind("Manual pitch switch", 'F');
	KeyBind manualy = KeyBind("Manual yaw switch", 'F');

	Checkbox antiaim = Checkbox("Anti aim");
	Checkbox show = Checkbox("Show fake");
	Checkbox silent = Checkbox("Silent reals");

	Checkbox pitch = Checkbox("Pitch");
	Listbox realp = Listbox("Real pitch", { "Up", "Half up", "Center", "Down" });
	Listbox fakep = Listbox("Fake pitch", { "Up", "Down" });

	Checkbox yaw = Checkbox("Yaw");
	Listbox realy = Listbox("Real yaw", { "None", "Edge", "Left", "Right", "Back", "Spin" });
	Listbox fakey = Listbox("Fake yaw", { "None", "Edge", "Left", "Right", "Back", "Spin" });

	bool lastFake = false;

	void command(CPlayer* you, CUserCmd* cmd, bool* bSendPacket = nullptr);
	void fsn(CPlayer* you, ClientFrameStage_t curStage);

	// - Returns true if we're using timed fake angles on this tick
	inline bool FakeAngle() { return bFakeAngle; }
	bool ResolveShots();
private:
	bool bFakeAngle = false;

	struct
	{
		int index = 0; // - Entity index
	} gamerShot;
};
extern ProNCC pNcc;