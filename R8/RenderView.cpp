/*#include "RenderView.h"
#include "R8_ITexture.h"
#include "Misc.h"

static IMaterialSystem* iMat = (IMaterialSystem*)gBase.MatSystem;

//-----------------------------------------------------------------------------
// Flags passed in with view setup
//-----------------------------------------------------------------------------
enum ClearFlags_t
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};

void renderCustomView(CViewSetup& view, ITexture* texture) {
	auto renderCtx = iMat->GetRenderContext();

	renderCtx->PushRenderTargetAndViewport();
	renderCtx->SetRenderTarget(texture);

	hooks->RenderView_old(client->g_pViewRender, view, view,
		VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, 0);

	renderCtx->PopRenderTargetAndViewport();
	renderCtx->Release();
}

void __fastcall RenderView_hook(IViewRender* this0, int EDX, CViewSetup& view, CViewSetup& hudViewSetup,
	ClearFlags_t nClearFlags, RenderViewInfo_t whatToDraw) {

	// Render main view to screen
	hooks->RenderView_old(this0, view, hudViewSetup, nClearFlags, whatToDraw);

	// Render mirrorcam texture
	CViewSetup mirrorView = view;
	mirrorView.x = mirrorView.oldX = 0;
	mirrorView.y = mirrorView.oldY = 0;
	mirrorView.width = mirrorView.oldWidth = mirrorWidth;
	mirrorView.height = mirrorView.oldHeight = mirrorHeight;
	mirrorView.angles = Vector(0, 0, 0);
	mirrorView.m_flAspectRatio = float(mirrorView.width) / float(mirrorView.height);

	renderCustomView(mirrorView, mirrorTexture);
}*/