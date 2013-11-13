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

// TODO REMOVE THAT OR I KILL MYSELF
#include <windows.h>

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

	
	const float mass = 500.0;

	// Create floor lol:D
	cEntity* entity = core->AddEntity(L"objects/plane.dae", L"objects/plane.dae", L"materials/test.zsm", 0.0);
	entity->SetPos(Vec3(0, 250, 0));
	//entity->SetScale(Vec3(100, 100, 1));

	// Create cubes
	std::vector<cEntity*> entities;
	cEntity* ent;
	for (int i = 0; i < 12; i++) {
		ent = core->AddEntity(L"objects/box.dae", L"objects/box.dae", L"materials/test.zsm", mass);
		ent->SetPos(Vec3(sin(float(i))*60, 120, cos(float(i))*60));
		ent->SetRot(Quat(Vec3(0, 0, 1), 0));
		entities.push_back(ent);
	}
	

	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		// Clear BB
		gApi->Clear(true, true);
	
		// Update everything
		float tDelta = cTimer::getDeltaSeconds();
		core->Update(tDelta);

		// Shader reloading
		if (GetAsyncKeyState('R') && GetAsyncKeyState(VK_LCONTROL))
			gEngine->ReloadResources();

		// Render Scene
		gEngine->RenderSceneForward();

		// Debug rendering
		//core->DebugRender((unsigned long)cCore::eDebugRenderMode::PHYSICS_TRIANGLES);

		// (game logic) z rotation
		static float zVal = 0.0f;
		zVal += ZS_PI / 4 * tDelta;
		for each (auto e in entities)
		{
			e->SetRot(Quat::EulerAnglesToQuat(0, 0, zVal));
		}
		entity->SetRot(Quat::EulerAnglesToQuat(0, 0, -zVal));

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	SAFE_DELETE(core);
	return 0;
}