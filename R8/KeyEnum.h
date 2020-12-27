#pragma once
#include <string>
using namespace std;

class CKey
{
public:
	inline bool AlphaChar(char c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
	}

	inline bool AcceptedKey(byte c)
	{
		return
			(c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
			c == VK_ESCAPE || c == VK_TAB || c == VK_CONTROL || c == VK_SHIFT || c == VK_SPACE ||
			c == VK_LBUTTON || c == VK_RBUTTON || (c >= VK_MBUTTON && c <= VK_XBUTTON2)|| c == VK_MENU;
	}

	string KeyToString(byte c)
	{
		if (!AcceptedKey(c))
			return to_string(c);
		else if (AlphaChar(c))
			return string(1, c);

		switch (c)
		{
		case VK_ESCAPE:
			return "Escape";
		case VK_TAB:
			return "Tab";
		case VK_CONTROL:
			return "Control";
		case VK_SHIFT:
			return "Shift";
		case VK_SPACE:
			return "Spacebar";
		case VK_LBUTTON:
			return "Left mouse";
		case VK_RBUTTON:
			return "Right mouse";
		case VK_MBUTTON:
		case VK_XBUTTON1:
		case VK_XBUTTON2:
			return "Mouse " + string(1, '0' + c - 1);
		case VK_MENU:
			return "Alt";
		}

		return NULL;
	}
};
extern CKey gKey;