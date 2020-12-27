#pragma once

class CViewSetup;
class IViewRender;
class ITexture;

typedef int ClearFlags_t;
typedef int RenderViewInfo_t;

void renderCustomView(CViewSetup& view, ITexture* texture);

void __fastcall RenderView_hook(IViewRender* this0, int EDX, CViewSetup& view, CViewSetup& hudViewSetup,
	ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw);