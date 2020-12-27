#pragma once

class ISurface;
typedef struct CScreenSize_t
{
	int height;
	int width;

} CScreenSize;
extern CScreenSize gScreenSize;

void __fastcall Hooked_PaintTraverse(void* pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
void Intro();