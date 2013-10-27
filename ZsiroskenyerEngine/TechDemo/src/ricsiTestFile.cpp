#include "testFiles.h"

// Visualities
#include "../../Core/src/IGraphicsEngine.h"
#include "../../GraphicsEngineRaster/src/ResourceManager.h"
#include "../../Core/src/Camera.h"
#include "../../Core/src/Game.h"
#include "../../Core/src/Level.h"
#include "../../Core/src/Entity.h"
#include "..\..\Core\src\Core.h"

// OS
#include "..\..\Core\src\IWindow.h"

// Math and utils
#include "..\..\Core\src\Factory.h"
#include "..\..\Core\src\math/Quat.h"
#include <vector>

#include "..\..\Core\src\Timer.h"

int ricsiMain() {

	// Create core
	cCore* core = cCore::GetInstance();

	// Get Modules
	IGraphicsEngine* gEngine = core->GetGraphicsEngine();
	IGraphicsApi* gApi = gEngine->GetGraphicsApi();
	ISceneManager* sceneManager = gEngine->GetSceneManager();

	// Window description
	IWindow::tDesc winDesc;
		winDesc.brush = IWindow::eBrush::RENDER_;
		winDesc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		winDesc.clientHeight = 600;
		winDesc.clientWidth = 800;
		winDesc.style = IWindow::eStyle::OVERLAPPED;

	// Create window
	IWindow* window = IWindow::Create(winDesc);

	// Set renderWindow
	gApi->SetWindow(window);

	// Create Camera
	cCamera cam(ZS_PIDIV2, (float)winDesc.clientWidth / winDesc.clientHeight, 0.01f, 5000.0f);
	sceneManager->SetActiveCamera(&cam);

	// Create object type dynamic physics BOX
	const float mass = 100.0f;
	cEntityType* boxType = core->CreateEntityType(L"dyn_box", L"objects/001Kurva.obj", L"materials/test.zsm", mass);
	//cEntityType* boxType = core->CreateEntityType(L"dyn_box", L"objects/box.dae", L"materials/test.zsm", mass);
	//cEntityType* boxType = core->CreateEntityType(L"dyn_box", L"objects/gallardo.dae", L"materials/test.zsm", mass);

	// Create entity
	const Vec3 position = Vec3(0, 3, 0);
	cEntity* entity = core->AddEntity(boxType, position);

	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		// Clear BB
		gApi->Clear(true, true);
	
		// Update engine
		core->Update(cTimer::getDeltaSeconds());

		// Render Scene
		gEngine->RenderSceneForward();

		// (game logic) z rotation
		static float zVal = 0.0f;
		zVal += 0.0007f;
		//entity->SetRotation(Quat::EulerAnglesToQuat(zVal, zVal + 0.4, zVal));
		entity->SetRotation(Quat::EulerAnglesToQuat(0, 0, zVal));

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	SAFE_DELETE(core);
	return 0;
}