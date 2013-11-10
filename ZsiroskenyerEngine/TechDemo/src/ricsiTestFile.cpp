#include "testFiles.h"

// Visualities
#include "..\..\Core\src\IGraphicsEngine.h"
#include "..\..\GraphicsEngineRaster\src\ResourceManager.h"
#include "..\..\Core\src\Camera.h"
#include "..\..\Core\src\Game.h"
#include "..\..\Core\src\Level.h"
#include "..\..\Core\src\Entity.h"
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
	gEngine->SetActiveCamera(&cam);

	// Create physics entity
	const float mass = 0.0f;
	cEntity* entity = core->AddEntity(L"objects/box.dae", L"objects/box.dae", L"materials/test.zsm", mass);
	entity->SetPosition(Vec3(0, 50, 0));

	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		// Clear BB
		gApi->Clear(true, true);
	
		// Update engine
		core->Update(cTimer::getDeltaSeconds());

		// Render Scene
		gEngine->RenderSceneForward();

		// Debug rendering
		//core->DebugRender((unsigned long)cCore::eDebugRenderMode::PHYSICS_TRIANGLES);

		// (game logic) z rotation
		static float zVal = 0.0f;
		zVal += 0.002f;
		entity->SetRotation(Quat::EulerAnglesToQuat(0, 0, zVal));

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	SAFE_DELETE(core);
	return 0;
}