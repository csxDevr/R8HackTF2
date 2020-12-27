#include "Styles.h"
#include "CDraw.h"

int DefaultStyle::ControlPanel(int x, int y, int w, int h, const char* name)
{
	gDraw.OutlineRect(x, y, w, h, SColor(50, 50, 55));
	
	int txtMargin = 0;

	int textW = 0, textH = 15;
	if (name)
		gBase.Surface->GetTextSize(gFonts.calibri_light_small, ToWchar(name).c_str(), textW, textH);
	gDraw.DrawRect(x + 1, y + 1, w - 2, textH + txtMargin, SColor(25));
	if (name)
		gDraw.DrawString(x + ((w / 2) - (textW / 2)), y, SColor(150), name, gFonts.calibri_light_small);

	gDraw.DrawLine(x, y + textH + txtMargin, x + w, y + textH + txtMargin, SColor(50, 50, 55));
	x += 1, y += textH + txtMargin + 1, w -= 2, h -= textH + txtMargin + 2;

	gDraw.DrawRect(x, y, w, h, SColor(30, 30, 33));

	return textH + txtMargin + 1;
}

int DefaultStyle::DialogButton(int x, int y, int w, const char* text, bool mouseOver)
{
	gDraw.DrawRect(x, y, w, 15, SColor(42, 42, 48));
	gDraw.OutlineRect(x, y, w, 15, SColor(60));

	gDraw.DrawString(x + 3, y, mouseOver ? SColor(90, 150, 225) : SColor(125, 125, 130), text, gFonts.verdana_bold);

	return 15;
}

void DefaultStyle::Dialog(int x, int y, int w, int h)
{
	gDraw.OutlineRect(x, y, w, h, SColor(58, 58, 70));
	gDraw.DrawRect(x + 1, y + 1, w - 2, h - 2, SColor(36, 36, 40));
}

#define TOPBAR 25
int DefaultStyle::TopBar(int x, int y, int w, const char* title)
{
	// Dark topbar
	gDraw.DrawRect(x, y, w, TOPBAR, SColor(32, 34, 37));

	if (title)
		gDraw.DrawString(x + 10, y, SColor(120), title, gFonts.segoe);

	return TOPBAR;
}

#include "ESP.h"
void WindowStyle::WaterMark(const char* name, int x, int y, bool background)
{
	static float hue = 0;

	static SColor red(237, 28, 36);
	static SColor light(200);

	int tw = 0, th = 0;
	gBase.Surface->GetTextSize(gFonts.cambria, ToWchar(name).c_str(), tw, th);

	if (background)
	{
		static Vertex_t verts[4];
		verts[0] = Vertex_t(Vector2(x + 10, y - 10));
		verts[1] = Vertex_t(Vector2(x + tw + 10, y - 10));
		verts[2] = Vertex_t(Vector2(x + tw + 10, y + th + 20));
		verts[3] = Vertex_t(Vector2(x + 10, y + th + 20));

		for (int i = 0; i < 4; i++)
		{
			Vector2 newVec = verts[i].m_Position;
			newVec = gEsp.rotate_vec2(newVec, Vector2(x + tw / 2, y + th / 2), DEG2RAD(5));

			verts[i].m_Position = newVec;
		}
		gDraw.DrawPolygon(4, verts, gTex.dark_gray);
	}

	gDraw.DrawString(x + 14, y + 4, light, name, gFonts.cambria);

	// Replaced colors.red with color
	gDraw.DrawRect(x, y, tw, 5, red);
	gDraw.DrawRect(x, y + 5, 5, th / 2, red);

	gDraw.DrawRect(x + 31, y + 14 + th, tw, 5, red);
	gDraw.DrawRect(x + 31 + tw - 5, y + 14 + th / 2, 5, th / 2, red);
}

#include "Menu.h"
//#include "lodepng.h"
//#include "resource.h"
byte* load_image(unsigned short id, int width, int height, const std::string &type = "PNG")
{
/*	// csgo->module is the module you get from creating a thread in DLLMain
	// static DWORD __stdcall thread( const LPVOID parameter )
	// csgo->module = HMODULE( parameter )
	auto resource = FindResource(gMenu.hInst, MAKEINTRESOURCE(id), type.data());
	auto loaded_resource = LoadResource(gMenu.hInst, resource);
	auto resource_ptr = LockResource(loaded_resource);
	auto resource_size = SizeofResource(gMenu.hInst, resource);

	vector<uint8_t> image;

	auto w = uint32_t(width);
	auto h = uint32_t(height);

	if (const auto error = lodepng::decode(image, w, h, (unsigned char*)resource_ptr, resource_size))
		exit(0);

	const auto data = new std::uint8_t[image.size()];
	copy(image.begin(), image.end(), data);

	return data;*/
	return nullptr;
}

void WindowStyle::About(int x, int y, int w, int h)
{
	/*static TextureHolder Discord(load_image(PNG_DC, 100, 100), 100, 100);
	static TextureHolder YouTube(load_image(PNG_YT, 127, 127), 127, 127);
	Discord.Draw(x + 5, y + 5);
	YouTube.Draw(x + 5 + 100 + 50, y + 5);*/

	int CenterX = x + (w / 2);

	int dcHeight, dcWidth, iHeight, iWidth;
	gBase.Surface->GetTextSize(gFonts.arial, L"http://getze.xyz/csx | http://csxdevrnull.usite.pro | CSX Community ©2011-2021", iWidth, iHeight);
	gDraw.DrawString(CenterX - (iWidth / 2), y + 5, SColor(160), L"http://getze.xyz/csx | http://csxdevrnull.usite.pro | CSX Community ©2011-2021");

	/*gBase.Surface->GetTextSize(gFonts.verdana, L"Have a question, want to chat, need to contact to me?", iWidth, iHeight);
	gBase.Surface->GetTextSize(gFonts.calibri_light, L"Join us : https://t.me/csxcommunity", dcWidth, dcHeight);

	gDraw.DrawString(CenterX - (iWidth / 2), y + 100, SColor(200), L"Have a question, want to chat, need to contact to me?", gFonts.verdana);
	gDraw.DrawString(CenterX - (dcWidth / 2), y + 110 + iHeight, SColor(255), L"Join us : https://t.me/csxcommunity", gFonts.calibri_light);*/

	gBase.Surface->GetTextSize(gFonts.verdana, L"Join us on telegram : https://t.me/csxcommunity", iWidth, iHeight);
	gBase.Surface->GetTextSize(gFonts.calibri_light, L"Join us on telegram : https://t.me/csxcommunity", dcWidth, dcHeight);

	gDraw.DrawString(CenterX - (iWidth / 2), y + /*200*/ 150, SColor(200), L"R8 reborn by csxDevr", gFonts.verdana);
	gDraw.DrawString(CenterX - (dcWidth / 2), y + /*210*/ 160 + iHeight, SColor(255), L"R8 reborn by csxDevr", gFonts.calibri_light);
}




//                                      ░█████╗░░██████╗██╗░░██╗██████╗░███████╗██╗░░░██╗██████╗░███╗░░██╗██╗░░░██╗██╗░░░░░██╗░░░░░                                      //
//                                      ██╔══██╗██╔════╝╚██╗██╔╝██╔══██╗██╔════╝██║░░░██║██╔══██╗████╗░██║██║░░░██║██║░░░░░██║░░░░░                                      //
//                                      ██║░░╚═╝╚█████╗░░╚███╔╝░██║░░██║█████╗░░╚██╗░██╔╝██████╔╝██╔██╗██║██║░░░██║██║░░░░░██║░░░░░                                      //
//                                      ██║░░██╗░╚═══██╗░██╔██╗░██║░░██║██╔══╝░░░╚████╔╝░██╔══██╗██║╚████║██║░░░██║██║░░░░░██║░░░░░                                      //
//                                      ╚█████╔╝██████╔╝██╔╝╚██╗██████╔╝███████╗░░╚██╔╝░░██║░░██║██║░╚███║╚██████╔╝███████╗███████╗                                      //
//                                      ░╚════╝░╚═════╝░╚═╝░░╚═╝╚═════╝░╚══════╝░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚══╝░╚═════╝░╚══════╝╚══════╝                                      //