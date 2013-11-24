#include "HagymaBox.h"

#include "GUISystem.h"
#include "Gui.h"
#include "GuiImage.h"
#include "GuiRenderer.h"

#include "../../Core/src/common.h"
#include "../../Core/src/Core.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/IGraphicsApi.h"

cHagymaBox::cHagymaBox(IWindow* w) {
	// Create Zsíroskenyér Engine core
	tGraphicsConfig gCfg; gCfg.rasterEngine.gxApi = tGraphicsConfig::D3D11;
	engineCore = new cCore(w, w->GetClientWidth(), w->GetClientHeight(), gCfg);

	IGraphicsApi* gApi = engineCore->GetGraphicsEngine()->GetGraphicsApi();
	guiRenderer = new cGuiRenderer(gApi, engineCore->GetGraphicsEngine()->GetShaderManager());

	// Create gui system
	guiSystem = new cGuiSystem(gApi);

	mainGui = guiSystem->CreateGui();

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
	SAFE_DELETE(engineCore);
	SAFE_DELETE(guiRenderer);
}

void cHagymaBox::InitGui() {
	cGuiImage* img = guiSystem->CreateImage(L"textures/cliff.jpg");
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
