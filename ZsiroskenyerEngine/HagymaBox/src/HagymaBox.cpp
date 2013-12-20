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

	// 3D scebe for editing and guu
	mainGui = guiSystem->CreateGui(engineCore->GetGraphicsEngine()->CreateScene());
	//scene = engineCore->GetGraphicsEngine()->CreateScene(); MORE SCENES FAIL
	//scene->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, (float)w->GetClientWidth() / w->GetClientHeight(), 0.01f, 5000.0f);
	mainGui->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, (float)w->GetClientWidth() / w->GetClientHeight(), 0.01f, 5000.0f);

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
	return mainGui->GetCamera();
	//return scene->GetCamera();
}
