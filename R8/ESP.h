#pragma once
#include "Controls.h"
#include "Base.h"
#include "DrawModelExecute.h"

enum ClientFrameStage_t;

class Esp
{
public:
	static void PreviewBox(int x, int y, int w, int h);
	DrawPanel panel = DrawPanel("ESP Preview", PreviewBox, 180, 190);

	Checkbox enabled = Checkbox("Enabled");
	Checkbox boxes = Checkbox("Boxes", true);
	Checkbox bones = Checkbox("Bones");
	Checkbox name = Checkbox("Name", true);
	Listbox health = Listbox("Health", { "Off", "Text - 2D", "Bar - 2D", "Plain Circle - 3D", "Outline Circle - 3D" }, 2);
	Listbox team = Listbox("Show team", { "Disabled", "Enabled", "Medics only" });

	Slider box_outline = Slider("Outlines", 1, 0, 2, 1);
	ColorPicker box_color = ColorPicker("Color", SColor(0));
	Slider box_thick = Slider("Thickness", 2, 1, 10, 1);
	Listbox box_style = Listbox("Style", { "2D - Plain", "2D - Cornered", "3D - Plain" });

	Checkbox classes = Checkbox("Class", true);
	Checkbox conditions = Checkbox("Show conditions", true);
	Checkbox local = Checkbox("Local ESP");
	Listbox cloaked = Listbox("Cloaked spies", { "None", "Highlight", "Don't show" }, 1);

	Checkbox glow = Checkbox("Glow");
	Checkbox chams = Checkbox("Chams");
	Listbox chams_vis = Listbox("Chams visibility", { "Always", "Behind walls", "Visible" }, 1);
	Checkbox outline = Checkbox("Outline");
	Listbox material = Listbox("Material", {"Flat", "Shaded", "Shiny"});
	Slider alpha = Slider("Transparency", 0, 0, 255, 1);

	// Buildings

	Checkbox sentries = Checkbox("Show Sentries", true);
	Checkbox others = Checkbox("Other buildings");
	Checkbox bTeam = Checkbox("Team buldings");
	Checkbox bType = Checkbox("Type", true);
	Checkbox bBoxes = Checkbox("Boxes", true);
	Checkbox bConds = Checkbox("Show conditions", true);
	Listbox bHealth = Listbox("Health", { "Off", "Text - 2D", "Bar - 2D" }, 2);

	inline Vector2 rotate_vec2(Vector2& point, const Vector2& origin, float radians);

	// - Override materials for chams, etc.
	void DME(const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld, CPlayer* pLocal, DrawModelExecuteFn Original);
	// - Mainly used to update glow effects
	void FrameStageNotify(ClientFrameStage_t Stage);
	// - Draws all players according to ESP settings
	void DrawPlayer(CPlayer* pLocal, CPlayer* ent, int index);
	// - Draws all entities according to ESP settings
	void DrawEntity(CPlayer* pLocal, CBaseEntity* ent, int index);
private:
	// - Thicc lines
	inline void ExpandOutline(int x, int y, int w, int h, SColor color, int Thickness);
	// - Get bounds of world coordinates for a 2D box
	bool ScreenBounds(Vector* worldpos, int amount, Vector2 &xy, Vector2 &wh);
	// - Draw a player box according to ESP settings
	void DrawPlayerBox(CPlayer* You, CPlayer* player, Vector Origin, SColor color);
	// - Drwa a generic box according to ESP settings
	void DrawBox(int x, int y, int w, int h, SColor color);
	// - Draw a 3D box around an entity
	void DrawBox3D(CBaseEntity* Entity, SColor Color);
	// - Get the screen bounds of a given entity
	bool GetBounds(CPlayer* You, CBaseEntity* Entity, Vector2 &xy, Vector2 &wh, bool Thin = false);
};
extern Esp gEsp;