#include "Menu.h"
#include "CDraw.h"
Menu gMenu;

#define keyDown GetAsyncKeyState
#define keyPress(x) keyDown(x) & 1

#include "Aim.h"
#include "ESP.h"
#include "Misc.h"
#include "HvH.h"
#include "Trigger.h"
#include "VisualEffects.h"

CKey gKey;

#define GROUP_WIDTH 180
#define TAB_WIDTH 150
#define MENU_TOPBAR 30

/*

─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
─████████████████───██████████████────██████████████───████████──████████────████████████───██████████████─██████──██████─████████████████───
─██░░░░░░░░░░░░██───██░░░░░░░░░░██────██░░░░░░░░░░██───██░░░░██──██░░░░██────██░░░░░░░░████─██░░░░░░░░░░██─██░░██──██░░██─██░░░░░░░░░░░░██───
─██░░████████░░██───██░░██████░░██────██░░██████░░██───████░░██──██░░████────██░░████░░░░██─██░░██████████─██░░██──██░░██─██░░████████░░██───
─██░░██────██░░██───██░░██──██░░██────██░░██──██░░██─────██░░░░██░░░░██──────██░░██──██░░██─██░░██─────────██░░██──██░░██─██░░██────██░░██───
─██░░████████░░██───██░░██████░░██────██░░██████░░████───████░░░░░░████──────██░░██──██░░██─██░░██████████─██░░██──██░░██─██░░████████░░██───
─██░░░░░░░░░░░░██───██░░░░░░░░░░██────██░░░░░░░░░░░░██─────████░░████────────██░░██──██░░██─██░░░░░░░░░░██─██░░██──██░░██─██░░░░░░░░░░░░██───
─██░░██████░░████───██░░██████░░██────██░░████████░░██───────██░░██──────────██░░██──██░░██─██░░██████████─██░░██──██░░██─██░░██████░░████───
─██░░██──██░░██─────██░░██──██░░██────██░░██────██░░██───────██░░██──────────██░░██──██░░██─██░░██─────────██░░░░██░░░░██─██░░██──██░░██─────
─██░░██──██░░██████─██░░██████░░██────██░░████████░░██───────██░░██──────────██░░████░░░░██─██░░██████████─████░░░░░░████─██░░██──██░░██████─
─██░░██──██░░░░░░██─██░░░░░░░░░░██────██░░░░░░░░░░░░██───────██░░██──────────██░░░░░░░░████─██░░░░░░░░░░██───████░░████───██░░██──██░░░░░░██─
─██████──██████████─██████████████────████████████████───────██████──────────████████████───██████████████─────██████─────██████──██████████─
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

*/

//#include "Editor.h"
void Menu::CreateGUI()
{
	//AddWindow(&gEditor);

	Tabs.AddTab(
		new Tab("Aimbot", {
			new Groupbox("General", {
				&gAim.enabled,
				new Space(5),
				&gAim.loud,
				&gAim.hitscan,
				&gAim.fov,
				&gAim.hitbox,
				&gAim.autoshoot
			}, GROUP_WIDTH),
			new Groupbox("Specific", {
				&gAim.scope,
				&gAim.predict
			}, GROUP_WIDTH),
			new Groupbox("Visual addons", {
				&gAim.target,
				&gAim.cache,
			}, GROUP_WIDTH),
			new Groupbox("Back-tracking", {
				&gAim.smarttrack,
				&gAim.backtrack
			}, GROUP_WIDTH)
		})
	);

	Tabs.AddTab(
		new Tab("Triggerbot", {
			new Groupbox("General", {
				&gTrigger.backstab,
				&gTrigger.airblast
			}, GROUP_WIDTH),
			new Groupbox("Aim assist", {
				&gTrigger.enabled,
				&gTrigger.hitbox
			}, GROUP_WIDTH)
		})
	);

	Tabs.AddTab(
		new Tab("ESP", {
			new Groupbox("Players", {
				&gEsp.enabled,
				&gEsp.boxes,
				&gEsp.bones,
				&gEsp.name,
				&gEsp.health,
				&gEsp.team,
			}, GROUP_WIDTH),
			new Groupbox("Details",{
				&gEsp.classes,
				&gEsp.conditions,
				&gEsp.local,
				&gEsp.cloaked
			}, GROUP_WIDTH),
			&gEsp.panel,
			new Groupbox("Box settings",{
				&gEsp.box_outline,
				&gEsp.box_thick,
				&gEsp.box_color,
				&gEsp.box_style
			}, GROUP_WIDTH),
			new Groupbox("Buildings",{
				&gEsp.sentries,
				&gEsp.others,
				&gEsp.bTeam,
				&gEsp.bType,
				&gEsp.bBoxes,
				&gEsp.bConds,
				&gEsp.bHealth
			}, GROUP_WIDTH),
			new Groupbox("Material",{
				&gEsp.glow,
				&gEsp.chams,
				&gEsp.chams_vis,
				&gEsp.outline,
				&gEsp.material,
				&gEsp.alpha
			}, GROUP_WIDTH),
		})
	);

	Tabs.AddTab(
		new Tab("HvH", {
			new Groupbox("Anti Aim", {
				&pNcc.antiaim,
				&pNcc.show,
				&pNcc.silent,
				new Space(5),
				&pNcc.pitch,
				&pNcc.realp,
				&pNcc.fakep,
				new Space(5),
				&pNcc.yaw,
				&pNcc.realy,
				&pNcc.fakey
			}, GROUP_WIDTH),
			new Groupbox("Anti Anti Aim", {
				&pNcc.resolver,
				//new Space(5),
				//&pNcc.manualp,
				//&pNcc.manualy,
			}, GROUP_WIDTH),
		})
	);

	Tabs.AddTab(
		new Tab("Misc", {
			new Groupbox("General",{
				&gMisc.bhop,
				&gMisc.nopush,
				&gMisc.crouchwalk,
			}, GROUP_WIDTH),
			new Groupbox("Visuals",{
				&gMisc.thirdperson,
				new Space(5),
				&gMisc.paths,
				&gMisc.fov,
				&gMisc.noscope,
				new Space(5),
				&gVFX.worldclr
				//&gMisc.utaunt
			}, GROUP_WIDTH)
		})
	);

	Tabs.AddTab(
		new Tab("About", {
			new DrawPanel("About section", gMenu.style->About, gMenu.scale.x - TAB_WIDTH - 32, gMenu.scale.y - MENU_TOPBAR)
		})
	);
}

void Menu::Draw()
{
	if (key == VK_INSERT || key == VK_F11)
		enabled = !enabled;

	if (!enabled)
		return;

	
//                                  █████████████████████████████████████████████████████████████████                                  //
//                                  █░░░░░░░░░░░░███░░░░░░░░░░░░░░█░░░░░░██░░░░░░█░░░░░░░░░░░░░░░░███                                  //
//      R8 by csxDevr               █░░▄▀▄▀▄▀▄▀░░░░█░░▄▀▄▀▄▀▄▀▄▀░░█░░▄▀░░██░░▄▀░░█░░▄▀▄▀▄▀▄▀▄▀▄▀░░███                                  //
//        build 12.5.1              █░░▄▀░░░░▄▀▄▀░░█░░▄▀░░░░░░░░░░█░░▄▀░░██░░▄▀░░█░░▄▀░░░░░░░░▄▀░░███                                  //
//         vac undetected           █░░▄▀░░██░░▄▀░░█░░▄▀░░█████████░░▄▀░░██░░▄▀░░█░░▄▀░░████░░▄▀░░███                                  //
//                                  █░░▄▀░░██░░▄▀░░█░░▄▀░░░░░░░░░░█░░▄▀░░██░░▄▀░░█░░▄▀░░░░░░░░▄▀░░███     http://getze.xyz/csx         // 
//                                  █░░▄▀░░██░░▄▀░░█░░▄▀▄▀▄▀▄▀▄▀░░█░░▄▀░░██░░▄▀░░█░░▄▀▄▀▄▀▄▀▄▀▄▀░░███     CSX Community supported      //
//                                  █░░▄▀░░██░░▄▀░░█░░▄▀░░░░░░░░░░█░░▄▀░░██░░▄▀░░█░░▄▀░░░░░░▄▀░░░░███     http://csxdevrnull.usite.pro //
//                                  █░░▄▀░░██░░▄▀░░█░░▄▀░░█████████░░▄▀▄▀░░▄▀▄▀░░█░░▄▀░░██░░▄▀░░█████                                  //
//                                  █░░▄▀░░░░▄▀▄▀░░█░░▄▀░░░░░░░░░░█░░░░▄▀▄▀▄▀░░░░█░░▄▀░░██░░▄▀░░░░░░█                                  //
//                                  █░░▄▀▄▀▄▀▄▀░░░░█░░▄▀▄▀▄▀▄▀▄▀░░███░░░░▄▀░░░░███░░▄▀░░██░░▄▀▄▀▄▀░░█                                  //
//                                  █░░░░░░░░░░░░███░░░░░░░░░░░░░░█████░░░░░░█████░░░░░░██░░░░░░░░░░█                                  //
//                                  █████████████████████████████████████████████████████████████████                                  //

	style->WaterMark("R8 TF2", 20, 20);
	gDraw.DrawString(50, 110, SColor(150, 120), "12.5.1 - " __DATE__, gFonts.verdana_bold);
	gDraw.DrawString(50, 210, SColor(150, 120), "http://getze.xyz/csx | CSX Community, INC.");

	static bool dragging = false;

	if (mb == e_mb::lclick && mouseOver(pos.x, pos.y, scale.x, MENU_TOPBAR))
		dragging = true;
	else if (mb != e_mb::ldown)
		dragging = false;

	if (dragging && focus == 0)
	{
		pos.x += mouse.x - pmouse.x;
		pos.y += mouse.y - pmouse.y;
	}

	for (auto& window : Windows)
	{
		if (!dragging && !focus)
			window->HandleInput();
		window->Draw();
	}

	POINT _pos = pos, _scale = scale;
	_scale.y += MENU_TOPBAR;

#pragma region Main window
	// Menu outline
	gDraw.OutlineRect(pos.x - 1, pos.y - 1, _scale.x + 2, _scale.y + 2, SColor(0));
	int topbar = style->TopBar(_pos.x, _pos.y, _scale.x, "R8 HACK                              coded by csxDevr");

	// Re-adjusting pos and scale for easy coding
	_pos.y += topbar, _scale.y -= topbar;

	// Tab region
	gDraw.DrawRect(_pos.x, _pos.y, TAB_WIDTH, _scale.y, SColor(22, 23, 24));
	gDraw.DrawLine(_pos.x + TAB_WIDTH - 1, _pos.y, _pos.x + TAB_WIDTH - 1, _pos.y + _scale.y, SColor(0));

	Tabs.SetPos(_pos.x, _pos.y + topbar);
	Tabs.SetWidth(TAB_WIDTH);
	Tabs.HandleInput();
	Tabs.Draw(false);

	// Control region
	gDraw.DrawRect(_pos.x + TAB_WIDTH, _pos.y, _scale.x - TAB_WIDTH, _scale.y, SColor(36, 36, 42));
	// Dividing line
	gDraw.DrawLine(_pos.x, _pos.y, _pos.x + _scale.x, _pos.y, SColor(0));
	// Re-adjusting pos and scale again
	_pos.x += TAB_WIDTH + 3, _scale.x = scale.x - (_pos.x - pos.x);

#pragma endregion
	
	if (Tabs.active)
	{
		int cx = _pos.x + 13, cy = _pos.y + 12;
		int maxWidth = 0;
		auto& controls = Tabs.active->GetChildren();
		for (size_t i = 0; i < controls.size(); i++)
		{
			if (!controls[i]->IsVisible())
				continue;

			if (cy + controls[i]->GetHeight() > scale.y + _pos.y - 12)
				cy = _pos.y + 12, cx += 13 + maxWidth + 10, maxWidth = 0;

			if (controls[i]->GetWidth() > maxWidth)
				maxWidth = controls[i]->GetWidth();
			controls[i]->SetPos(cx, cy);

			bool over = mouseOver(cx, cy, controls[i]->GetWidth(), controls[i]->GetHeight());
			bool getInput = !(controls[i]->flags & noinput) && over && !IsDialogOpen();
			if (getInput)
				controls[i]->HandleInput();

			cy += controls[i]->Draw(getInput) + SPACING;
		}
	}

	size_t last = dialogs.size() - 1;
	if (dialogs.size() > 1)
	{
		e_mb new_mb = mb;
		e_mw new_mw = mw;
		POINT new_mouse = mouse, new_pmouse = pmouse;

		// Enforce focus so that only the last dialog gets to use these variables
		mb = e_mb::null, mw = e_mw::null, mouse = pmouse = { 0, 0 };

		for (size_t i = 0; i < last; i++)
		{
			if (dialogs[i] == nullptr)
				continue;

			dialogs[i]->Draw(dialogs[i]->data, i + 1);
		}
		mb = new_mb, mw = new_mw, mouse = new_mouse, pmouse = new_pmouse;
		dialogs[last]->Draw(dialogs[last]->data, last + 1);
	}
	else if (!last)
		dialogs[last]->Draw(dialogs[last]->data, last + 1);

	if (key == VK_ESCAPE && dialogs.size())
		dialogs.pop_back();
}

bool Menu::mouseOver(int x, int y, int w, int h)
{
	return mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h;
}

bool Menu::mouseOver(CBounds region)
{
	return mouse.x >= region.x && mouse.x <= region.x + region.w && mouse.y >= region.y && mouse.y <= region.y + region.h;
}

void Menu::GetInput()
{
	int mx = 0, my = 0;
	gBase.Surface->GetCursorPosition(mx, my);

	pmouse = mouse;
	mouse = { mx, my };
	
	if (keyDown(VK_LBUTTON))
	{
		if (mb == e_mb::lclick || mb == e_mb::ldown)
			mb = e_mb::ldown;
		else
			mb = e_mb::lclick;
	}
	else if (keyDown(VK_RBUTTON))
	{
		if (mb == e_mb::rclick || mb == e_mb::rdown)
			mb = e_mb::rdown;
		else
			mb = e_mb::rclick;
	}
	else
		mb = e_mb::null;
}

void Menu::EndInput()
{
	// Reseting Window message variables so they won't stick
	mw = e_mw::null;
	key = NULL;
}

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		if ((int)wParam < 0)
			gMenu.mw = e_mw::up;
		else
			gMenu.mw = e_mw::down;
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // Allows input to work with alt key
		if (wParam > 255)
			break;
		gMenu.keys[wParam] = true, gMenu.last_key = wParam = gMenu.key = wParam;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// Avoid Mouse 3/4/5 here
		if (wParam > 255 && wParam < VK_MBUTTON && wParam > VK_XBUTTON2)
			break;
		gMenu.keys[wParam] = false;
		if (gMenu.last_key == wParam)
			gMenu.last_key = NULL;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		gMenu.keys[VK_MBUTTON] = uMsg == WM_MBUTTONDOWN;
		if (uMsg == WM_MBUTTONDOWN)
			gMenu.last_key = gMenu.key = VK_MBUTTON;
		else if (gMenu.last_key == VK_MBUTTON)
			gMenu.last_key = NULL;
	break;
	/*case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
		int param = GET_KEYSTATE_WPARAM(wParam);
		if (!(param & MK_XBUTTON1) && !(param & MK_XBUTTON2))
			break;

		byte vk;
		if (param & MK_XBUTTON1)
		{
			vk = VK_XBUTTON1;
			gMenu.keys[vk] = uMsg == WM_XBUTTONDOWN;
			if (uMsg == WM_XBUTTONDOWN)
				gMenu.last_key = gMenu.key = vk;
			else if (gMenu.last_key == vk)
				gMenu.last_key = NULL;
		}
		if (param & MK_XBUTTON2)
		{
			vk = VK_XBUTTON2;
			gMenu.keys[vk] = uMsg == WM_XBUTTONDOWN;
			if (uMsg == WM_XBUTTONDOWN)
				gMenu.last_key = gMenu.key = vk;
			else if (gMenu.last_key == vk)
				gMenu.last_key = NULL;
		}*/
	}

	if (gMenu.enabled && gMenu.keys[VK_ESCAPE])
		return 0; // Don't let escape key affect the game while using our UI

	return CallWindowProc(gMenu.windowProc, hWnd, uMsg, wParam, lParam);
}

void Menu::OpenDialog(Dialog& dlg)
{
	dialogs.push_back(&dlg);
	focus = dialogs.size();
}
void Menu::CloseDialog(size_t Index)
{
	if (Index == 0)
		return;

	Index--;
	if (Index >= dialogs.size())
		return;

	dialogs.erase(dialogs.begin() + Index);
	focus = dialogs.size();
}