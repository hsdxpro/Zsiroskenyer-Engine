#include "HagymaBox.h"

#include "GUISystem.h"
#include "Gui.h"
#include "GuiImage.h"
#include "GuiButton.h"

#include "../../Core/src/common.h"
#include "../../Core/src/Core.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/GraphicsLight.h"

cHagymaBox::cHagymaBox(IWindow* w) {
	// Create Zsíroskenyér Engine core
	tGraphicsConfig gCfg; gCfg.rasterEngine.gxApi = tGraphicsConfig::D3D11;
	engineCore = new cCore(w, w->GetClientWidth(), w->GetClientHeight(), gCfg);

	// 3D scene for editing
	//scene = engineCore->GetGraphicsEngine()->CreateScene(); MORE SCENES FAIL
	//scene->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, (float)w->GetClientWidth() / w->GetClientHeight(), 0.01f, 5000.0f);

	// Create gui system
	guiSystem = new cGuiSystem();

	// Create gui, developed in 1280 * 1024
	mainGui = guiSystem->CreateGui(engineCore->GetGraphicsEngine()->CreateScene(), 1280, 1024);
	
	IGraphicsScene* s = mainGui->GetScene();

	cGraphicsLight* ambient = s->CreateLight();
	
	ambient->type = cGraphicsLight::AMBIENT;
	ambient->color = Vec3(1.0f, 1.0f, 1.0f);

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
	SAFE_DELETE(engineCore);
}

void cHagymaBox::InitGui() {
	//cGuiButton* loadEntityBtn = mainGui->CreateButton(L"materials/buttonBG.zsm",0, 0, 1280, 1024);
	//loadEntityBtn->SetText(L"ALMA");

	mainGui->CreateImage(L"materials/cliff.zsm", 0, 0, 1280, 1024);
	//mainGui->CreateImage(L"materials/cliff.zsm", 400, 0, 400, 300);
	//mainGui->CreateImage(L"materials/cliff.zsm", 0, 300, 400, 300);
	//mainGui->CreateImage(L"materials/cliff.zsm", 400, 300, 400, 300);
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
	return mainGui->GetScene()->GetCamera();
	//return scene->GetCamera();
}
