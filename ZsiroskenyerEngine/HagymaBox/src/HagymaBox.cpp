#include "HagymaBox.h"

#include "GUISystem.h"
#include "Gui.h"
#include "GuiImage.h"

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

	// Create gui
	mainGui = guiSystem->CreateGui(engineCore->GetGraphicsEngine()->CreateScene(), w->GetClientWidth(), w->GetClientHeight());
	
	IGraphicsScene* s = mainGui->GetScene();

	cGraphicsLight* sunLight = s->CreateLight();
	cGraphicsLight* secondSunLight = s->CreateLight();
	cGraphicsLight* thirdSunLight = s->CreateLight();
	cGraphicsLight* skyLight = s->CreateLight();
	cGraphicsLight* pointLight = s->CreateLight();

	sunLight->type = cGraphicsLight::DIRECTIONAL;
	sunLight->color = Vec3(0.9f, 0.8f, 0.77f);
	sunLight->direction = Vec3(0.5f, 0.5f, -0.5f).Normalize();

	secondSunLight->type = cGraphicsLight::DIRECTIONAL;
	secondSunLight->color = Vec3(1.2f, 0.03f, 0.95f);
	secondSunLight->direction = Vec3(-0.8f, 0.0f, -0.2f).Normalize();

	thirdSunLight->type = cGraphicsLight::DIRECTIONAL;
	thirdSunLight->color = Vec3(0.2f, 0.8f, 0.77f);
	thirdSunLight->direction = Vec3(0.8f, -0.1f, -0.3f).Normalize();

	skyLight->type = cGraphicsLight::AMBIENT;
	skyLight->color = Vec3(0.2f, 0.3f, 0.35f);

	pointLight->atten0 = pointLight->atten1 = pointLight->atten2 = 0.0f;
	pointLight->color = Vec3(0.2f, 0.2f, 0.9f);
	pointLight->position = Vec3(8.0f, 8.0f, 2.0f);
	pointLight->range = 20.f;
	pointLight->type = cGraphicsLight::POINT;

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
	SAFE_DELETE(engineCore);
}

void cHagymaBox::InitGui() {
	mainGui->CreateImage(L"materials/cliff.zsm", 0, 0, 400, 300);
	mainGui->CreateImage(L"materials/cliff.zsm", 400, 0, 400, 300);
	mainGui->CreateImage(L"materials/cliff.zsm", 0, 300, 400, 300);
	mainGui->CreateImage(L"materials/cliff.zsm", 400, 300, 400, 300);
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
