//#include "Editor.h"
//#include "CDraw.h"
//
//GUIEditor gEditor("UI Editor", 960, 540);
//
//#define PANEL_A 80
//
//void GUIEditor::Draw()
//{
//	bool bInput = gMenu.mouseOver(x, y, w, h);
//
//	// Title bar
//
//	gDraw.DrawRect(x, y, w, 20, SColor(40));
//	gDraw.DrawString(x + 5, y, SColor(100), name, gFonts.calibri_light_small);
//	int iY = y + 20;
//
//	// Window region
//
//	gDraw.DrawRect(x + PANEL_A, iY, w - PANEL_A, h, SColor(25)); // Background
//	gDraw.OutlineRect(x, y, w, h + 20, SColor(0)); // Outline
//
//	gDraw.DrawRect(x, iY, PANEL_A, h, SColor(32)); // Primary panel
//	gDraw.OutlineRect(x, iY, PANEL_A, h, SColor(0)); // Panel border
//	gDraw.OutlineRect(x + 1, iY + 1, PANEL_A - 2, h - 2, SColor(30, 40, 60)); // High light
//	gDraw.OutlineRect(x + 2, iY + 2, PANEL_A - 4, h - 4, SColor(26, 36, 48)); // Dark light
//
//	// Canvas
//	gDraw.DrawRect(x + PANEL_A + 80, iY + 50, w - 80 * 2 - PANEL_A, h - 50 * 2, SColor(200));
//
//	if (tool == e_icon::rect || gMenu.mouseOver(x + (PANEL_A / 2) - (20 / 2), iY + 20, 20, 20))
//		gDraw.DrawRect(x + (PANEL_A / 2) - (20 / 2) - 5, iY + 20 - 5, 20 + 10, 20 + 10, SColor(0, 100));
//	else if (tool == e_icon::line || gMenu.mouseOver(x + (PANEL_A / 2) - (20 / 2), iY + 20 + 40, 20, 20))
//		gDraw.DrawRect(x + (PANEL_A / 2) - (20 / 2) - 5, iY + 20 + 40 - 5, 20 + 10, 20 + 10, SColor(0, 100));
//
//	Icon(e_icon::rect, x + (PANEL_A / 2) - (20 / 2), iY + 20);
//	Icon(e_icon::line, x + (PANEL_A / 2) - (20 / 2), iY + 20 + 40);
//
//	for (auto& rect : items)
//		rect->Draw();
//}
//
//void GUIEditor::HandleInput()
//{
//	if (gMenu.mb == e_mb::lclick && gMenu.mouseOver(x, y, w, 20))
//		eDragging = e_drag::menu;
//	else if (gMenu.mb != e_mb::ldown &&
//			(eDragging == e_drag::menu || eDragging == e_drag::canvas))
//		eDragging = e_drag::null;
//
//	if (eDragging == e_drag::menu)
//	{
//		x += gMenu.mouse.x - gMenu.pmouse.x, y += gMenu.mouse.y - gMenu.pmouse.y;
//		return;
//	}
//
//	if (gMenu.mb == e_mb::null)
//		eDragging = e_drag::null;
//
//	int iY = y + 20;
//	if (gMenu.mouseOver(x, iY, PANEL_A, h)) // Toolbar
//	{
//		if (gMenu.mb == e_mb::lclick)
//		{
//			if (gMenu.mouseOver(x + (PANEL_A / 2) - (20 / 2), iY + 20, 20, 20))
//				tool = e_icon::rect;
//			else if (gMenu.mouseOver(x + (PANEL_A / 2) - (20 / 2), iY + 20 + 40, 20, 20))
//				tool = e_icon::line;
//		}
//	}
//	else if (gMenu.mouseOver(GetCanvas())) // Canvas
//	{
//		if (gMenu.mb == e_mb::lclick)
//		{
//			if (tool == e_icon::rect)
//				eDragging = e_drag::canvas;
//		}
//	}
//
//	if (eDragging == e_drag::canvas)
//	{
//		if (tool == e_icon::rect)
//		{
//			if (gMenu.mb == e_mb::lclick)
//				items.push_back(new CRectangle(SColor(50, 100, 255), { gMenu.mouse.x, gMenu.mouse.y, 0, 0 }, &x));
//
//			CRectangle* rect;
//			if (items.back()->Cast(rect))
//				rect->bounds.w = (gMenu.mouse.x - x) - rect->bounds.x, rect->bounds.h = (gMenu.mouse.y - y) - rect->bounds.y;
//		}
//		else if (tool == e_icon::line)
//		{
//			if (gMenu.mb == e_mb::lclick)
//				items.push_back(new CLine(SColor(50, 100, 255), { gMenu.mouse.x, gMenu.mouse.y, 0, 0 }, &x));
//
//			CLine* line;
//			if (items.back()->Cast(line))
//				line->bounds.w = (gMenu.mouse.x - x) - line->bounds.x, line->bounds.h = (gMenu.mouse.y - y) - line->bounds.y;
//		}
//	}
//}
//
//CBounds GUIEditor::GetCanvas()
//{
//	return CBounds(x, y + 20 + 50, w - 80 * 2 - PANEL_A, h - 50 * 2);
//}
//
//void GUIEditor::Icon(e_icon Icon, int X, int Y)
//{
//	if (Icon == e_icon::rect)
//	{
//		gDraw.OutlineRect(X, Y, 20, 20, SColor(100));
//		gDraw.DrawRect(X + 2, Y + 2, 16, 16, SColor(100, 30));
//	}
//	else if (Icon == e_icon::line)
//	{
//		gDraw.DrawLine(X + 1, Y, X + 20, Y + 19, SColor(100, 10));
//		gDraw.DrawLine(X, Y, X + 20, Y + 20, SColor(100));
//		gDraw.DrawLine(X, Y + 1, X + 19, Y + 20, SColor(100, 10));
//	}
//}
//
//void CRectangle::Draw()
//{
//	CBounds newbounds = bounds;
//	if (parent)
//		newbounds.x += parent[0], newbounds.y += parent[1];
//
//	if (newbounds.w < 0)
//		newbounds.x += newbounds.w, newbounds.w *= -1;
//	if (newbounds.h < 0)
//		newbounds.y += newbounds.h, newbounds.h *= -1;
//
//	if (bOutline)
//		gDraw.OutlineRect(newbounds.x, newbounds.y, newbounds.w, newbounds.h, sColor);
//	else
//		gDraw.DrawRect(newbounds.x, newbounds.y, newbounds.w, newbounds.h, sColor);
//}
//
//void CLine::Draw()
//{
//	CBounds newbounds = bounds;
//	if (parent)
//		newbounds.x += parent[0], newbounds.y += parent[1];
//
//	gDraw.DrawLine(newbounds.x, newbounds.y, newbounds.x + newbounds.w, newbounds.y + newbounds.h, sColor);
//}