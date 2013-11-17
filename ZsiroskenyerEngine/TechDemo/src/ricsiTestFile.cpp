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

#define CAM_MOVE_SPEED 120
void freeCamUpdate(cCamera& cam, float tDelta);

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

	// Create character
	cEntity* entity = core->AddEntity(L"objects/character.obj", L"objects/character.obj", L"materials/character.zsm", 0.0);
	entity->SetPos(Vec3(0, 120, -130));

	// Create floor lol:D
	//cEntity* entity = core->AddEntity(L"objects/plane.dae", L"objects/plane.dae", L"materials/test.zsm", 0.0);
	//entity->SetPos(Vec3(0, 250, -60));
	//entity->SetScale(Vec3(100, 100, 1));
	//entity->SetRot(Quat::EulerAnglesToQuat(ZS_PIDIV2, 0, 0));

	// Create cubes
	std::vector<cEntity*> entities;
	cEntity* ent;
	for (int i = 0; i < 12; i++) {
		ent = core->AddEntity(L"objects/box.dae", L"objects/box.dae", L"materials/test.zsm", mass);
		ent->SetPos(Vec3(sin(float(i))*60, 120, cos(float(i))*60));
		entities.push_back(ent);
	}
	

	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		// Clear BB
		gApi->Clear(true, true);
	
		// Update everything
		float tDelta = cTimer::getDeltaSeconds();
		size_t fps = cTimer::GetFps(tDelta);
		freeCamUpdate(cam, tDelta);
		core->Update(tDelta);

		static float timer1 = 0.0;
		timer1 += tDelta;


		// Don't hog with set caption text... Fucking slow operation
		if (timer1 > 1.0f)
		{
			window->SetCaptionText(zsString(L"[Zsíroskenyér Engine 0.1 Beta]  FPS: ") + fps);
			timer1 = 0.0f;
		}

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

void freeCamUpdate(cCamera& cam, float tDelta) {
	Vec3 deltaMove(0, 0, 0);
	if (GetAsyncKeyState('W'))
		deltaMove += cam.GetDirFront() * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('S'))
		deltaMove += cam.GetDirBack() * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('A'))
		deltaMove += cam.GetDirLeft() * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('D'))
		deltaMove += cam.GetDirRight() * (CAM_MOVE_SPEED * tDelta);
	
	// Set new position
	cam.SetPos(cam.GetPos() + deltaMove);

	// Hold direction
	cam.SetDir(Vec3(0, 1, 0));
}