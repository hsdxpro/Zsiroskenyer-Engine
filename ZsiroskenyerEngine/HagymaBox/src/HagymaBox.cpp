#include "HagymaBox.h"

#include "GUISystem.h"
#include "Gui.h"
#include "GuiImage.h"

#include "../../Core/src/common.h"
#include "../../Core/src/Core.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/IGraphicsApi.h"

cHagymaBox::cHagymaBox(IWindow* w) {
	// Create Zsíroskenyér Engine core
	tGraphicsConfig gCfg; gCfg.rasterEngine.gxApi = tGraphicsConfig::D3D11;
	engineCore = new cCore(w, w->GetClientWidth(), w->GetClientHeight(), gCfg);

	// Create gui system
	guiSystem = new cGuiSystem();

	mainGui = guiSystem->CreateGui();

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
}

void cHagymaBox::InitGui() {
	ITexture2D* t;
	engineCore->GetGraphicsEngine()->GetGraphicsApi()->CreateTexture(&t, L"textures/cliff.jpg");

	cGuiImage* img = guiSystem->CreateImage(t);

	mainGui->Add(img, 100, 100);
}

void cHagymaBox::Update(float tDelta) {
	// Updating the gui system..

	// Editor Logic here :....

	// Rendering
	RenderGui();
}

void cHagymaBox::RenderGui() {
	
	// Render all guis
}
