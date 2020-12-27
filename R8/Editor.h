//#pragma once
//#include "Menu.h"
//
//enum class e_drawing : byte
//{
//	null,
//	rect,
//	line
//};
//
//class CDrawing
//{
//public:
//	CBounds bounds;
//	const int* parent;
//	SColor sColor;
//
//	inline e_drawing GetType() const { return type; }
//	template <class Type>
//	bool Cast(Type* &Drawing)
//	{
//		if (type != Drawing->type)
//			return false;
//		Drawing = (Type*)this;
//		return true;
//	}
//
//	virtual void Draw() = 0;
//protected:
//	e_drawing type = e_drawing::null;
//	CDrawing() {}
//};
//
//class CRectangle : public CDrawing
//{
//public:
//	bool bOutline;
//
//	void Draw();
//
//	CRectangle(SColor Color, CBounds Bounds = CBounds(), const int* ParentXY = nullptr, bool Outlined = false)
//	{
//		sColor = Color, bounds = Bounds, parent = ParentXY, bOutline = Outlined;
//		type = e_drawing::rect;
//		if (ParentXY)
//			bounds.x -= ParentXY[0], bounds.y -= ParentXY[1];
//	}
//};
//
//class CLine : public CDrawing
//{
//public:
//	void Draw();
//
//	CLine(SColor Color, CBounds Bounds = CBounds(), const int* ParentXY = nullptr)
//	{
//		sColor = Color, bounds = Bounds, parent = ParentXY;
//		type = e_drawing::line;
//		if (ParentXY)
//			bounds.x -= ParentXY[0], bounds.y -= ParentXY[1];
//	}
//};
//
//class GUIEditor : public BaseWindow
//{
//public:
//	void Draw();
//	void HandleInput();
//
//	GUIEditor(const char* Name, int W = 200, int H = 100, int X = 500, int Y = 500)
//	{
//		name = Name, x = X, y = Y, w = W, h = H;
//	}
//private:
//	enum class e_icon : byte
//	{
//		null,
//		rect,
//		line
//	};
//	enum class e_drag : byte
//	{
//		null,
//		menu,
//		canvas
//	};
//
//	e_drag eDragging = e_drag::null;
//
//	vector<CDrawing*> items;
//	CBounds GetCanvas();
//
//	e_icon tool;
//	void Icon(e_icon Icon, int X, int Y);
//};
//extern GUIEditor gEditor;