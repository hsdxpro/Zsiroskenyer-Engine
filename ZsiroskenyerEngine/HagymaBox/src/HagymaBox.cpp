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

	// Need gApi, review why that shit needed...
	//IGraphicsApi* gApi = engineCore->GetGraphicsEngine()->GetGraphicsApi();

	// Main 3D scene for editing
	editorScene = engineCore->GetGraphicsEngine()->CreateScene();

	// Create gui system
	guiSystem = new cGuiSystem();

	// Create gui
	mainGui = guiSystem->CreateGui();

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
	SAFE_DELETE(engineCore);
}

void cHagymaBox::InitGui() {
	//cGuiImage* img = guiSystem->CreateImage(L"textures/cliff.jpg");
	//mainGui->Add(img, 100, 100);
}

void cHagymaBox::Update(float tDelta) {
	// Updating the gui system..

	// Editor Logic here :....
}