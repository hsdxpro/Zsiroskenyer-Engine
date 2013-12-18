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

	// Main 3D scene for editing
	scene = engineCore->GetGraphicsEngine()->CreateScene();
	scene->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, (float)w->GetClientWidth() / w->GetClientHeight(), 0.01f, 5000.0f);

	// Create gui system
	guiSystem = new cGuiSystem();

	// Create gui
	mainGui = guiSystem->CreateGui(engineCore->GetGraphicsEngine()->CreateScene());

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
	SAFE_DELETE(engineCore);
}

void cHagymaBox::InitGui() {
	cGuiImage* img = mainGui->CreateImage(L"materials/cliff.zsm");
}

void cHagymaBox::Update(float deltaT) {
	engineCore->Update(deltaT);
	engineCore->GetGraphicsEngine()->GetGraphicsApi()->Present();
	// Updating the gui system..

	// Editor Logic here :....
}

cCore* cHagymaBox::GetEngineCore() {
	return engineCore;
}

cCamera& cHagymaBox::GetCamera() {
	return scene->GetCamera();
}
