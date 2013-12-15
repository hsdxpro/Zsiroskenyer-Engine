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
#include <cstdlib>
#include <iostream>

#include "..\..\Core\src\Timer.h"

// TODO REMOVE THAT OR I KILL MYSELF
#define NOMINMAX
#include <windows.h>
#include <tchar.h> // even worse
cEntity* player;
IPhysicsEngine* pEngine;

#define CAM_MOVE_SPEED 20
#define PLAYER_MOVE_SPEED 4

void updateDemo(cCamera& cam, float tDelta);

int ricsiMain() {
	// Window description
	IWindow::tDesc winDesc;
		winDesc.brush = IWindow::eBrush::RENDER_;
		winDesc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		winDesc.clientHeight = 600;
		winDesc.clientWidth = 800;
		winDesc.style = IWindow::eStyle::OVERLAPPED;

	// Create window
	IWindow* window = IWindow::Create(winDesc);

	// Create core
	tGraphicsConfig gCfg;
	gCfg.rasterEngine.gxApi = tGraphicsConfig::eGraphicsApi::D3D11; // MERT AZT AKAROK HASZNÁLNI BAZDMEG :D

	cCore core(window, window->GetClientWidth(), window->GetClientHeight(), gCfg);

	// Get Modules
	IGraphicsEngine* gEngine = core.GetGraphicsEngine();
	IGraphicsApi* gApi = gEngine->GetGraphicsApi(); 
	pEngine = core.GetPhysicsEngine();


	// Create scene with camera
	IGraphicsScene* s = gEngine->CreateScene();// gEngine->SetActiveCamera(&cam);
	s->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, (float)winDesc.clientWidth / winDesc.clientHeight, 0.01f, 5000.0f);

	// Static terrain
	zsString staticBaseNames[9] = { "coyote",
									"crate",
									"demo_cliff_fence",
									"demo_ground",
									"demo_house",
									"demo_road",
									"demo_tunnel",
									"fake_tunnel",
									"tower"
								  };

	const float mass = 0.0;
	zsString basePath = L"../../Game Assets/";

	ASSERT(sizeof(staticBaseNames) > 0);
	for (size_t i = 0; i < sizeof(staticBaseNames) / sizeof(staticBaseNames[0]); i++) {
		zsString geomPath = basePath + L"objects/" + staticBaseNames[i] + L".dae";
		core.AddEntity(s, geomPath, geomPath, basePath + L"materials/" + staticBaseNames[i] + L".zsm", mass);
	}
	
	// Our player
	player = core.AddEntity(s, basePath + L"objects/character.dae", basePath + L"objects/character.dae", basePath + L"materials/character.zsm", 10.0, false);
	player->SetPos(Vec3(9, 0, 20));

	cEntity* crate =  core.AddEntity(s, basePath + L"objects/crate.dae", basePath + L"objects/crate.dae", basePath + L"materials/crate.zsm", 10.0, true);
	crate->SetPos(Vec3(9, 0, 40));

	//TESZT01BRANCH TESZT VÁÁ :) !

	// Main loop
	IWindow::eMessage msg;
	while(window->IsOpened()) {
		while (window->HandleMessage(&msg)) {
			//if (msg == IWindow::eMessage::SIZE_CHANGED)
				//gEngine->Resize(window->GetClientWidth(), window->GetClientHeight());	
		}

		// Clear BB
		gApi->Clear(true, true);
	
		// Update everything
		static cTimer t;
		float tDelta = t.getDeltaSeconds();

		updateDemo(s->GetCamera(), tDelta);
		core.Update(tDelta);

		// Don't hog with set caption text... Fucking slow operation
		static float timer1 = 0.0;
		timer1 += tDelta;
		size_t fps = cTimer::GetFps(tDelta);
		if (timer1 > 1.0f)
		{
			window->SetCaptionText(zsString(L"[Zsíroskenyér Engine 0.1 Beta]  FPS: ") + fps);
			timer1 = 0.0f;
		}

		// Shader reloading
		if (GetAsyncKeyState('R') && GetAsyncKeyState(VK_LCONTROL))
			gEngine->ReloadResources();

		// Render Scene
		gEngine->Update(tDelta);

		// Debug rendering
		//core.DebugRender((unsigned long)cCore::eDebugRenderMode::PHYSICS_TRIANGLES);

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	return 0;
}


bool noJump = false; // don't commit suicide upon seeing this :D
void updateDemo(cCamera& cam, float tDelta) {
/*
	static Quat playerRot;
	Vec3 playerDir = Vec3(0, 1, 0) * playerRot;
	const Vec3 up(0, 0, 1);

// Controlling player with WSAD
	Vec3 deltaMove(0, 0, 0);
	if (GetAsyncKeyState('W'))
		deltaMove += playerDir * tDelta;
	if (GetAsyncKeyState('S'))
		deltaMove += -playerDir * tDelta;
	if (GetAsyncKeyState('A'))
		deltaMove += Vec3::Cross(up, playerDir)  * tDelta;
	if (GetAsyncKeyState('D'))
		deltaMove += Vec3::Cross(playerDir, up)  * tDelta;

	Vec3 playerPos = player->GetPos();
	player->SetPos(playerPos + deltaMove);
	cam.SetPos(playerPos + Vec3(0, -0.75, 2));
*/

// Shooting boxes
	//if (((short)GetAsyncKeyState(VK_LBUTTON)) & 0x80) // Press detect doesn't work :(
	if (GetAsyncKeyState(VK_LBUTTON))
		pEngine->ShootBox(0.3f, cam.GetPos(), cam.GetDirFront(), 60); // This function in the interface is just for test purposes


// CAMERA MOVING
	Vec3 deltaMove(0, 0, 0);
	if (GetAsyncKeyState('W'))
		deltaMove += cam.GetDirFront() * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('S'))
		deltaMove += cam.GetDirBack()  * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('A'))
		deltaMove += cam.GetDirLeft()  * (CAM_MOVE_SPEED * tDelta);
	if (GetAsyncKeyState('D'))
		deltaMove += cam.GetDirRight() * (CAM_MOVE_SPEED * tDelta);
	
	// Set new position
	cam.SetPos(cam.GetPos() + deltaMove);
	cam.SetTarget(cam.GetTarget() + deltaMove);


	// CAMERA ROTATION...
	// get delta mouse
	static POINT lastMousePos;
	static POINT currMousePos;

	static bool firstRun = true;
	if (firstRun) {
		GetCursorPos(&lastMousePos);
		firstRun = false;
	}
	GetCursorPos(&currMousePos);

	const int pixelsPer360ROT = 600;

	static float mouseDeltaX = 0;
	static float mouseDeltaY = 0;

	if (GetAsyncKeyState(VK_RBUTTON)) {
		if (!noJump) {
			noJump = true;
			lastMousePos = currMousePos;
			return;
		}
	}
	else {
		noJump = false;
		return;
	}

	mouseDeltaX += (float)(-currMousePos.x + lastMousePos.x) / pixelsPer360ROT * ZS_PI2;
	mouseDeltaY += (float)(-currMousePos.y + lastMousePos.y) / pixelsPer360ROT * ZS_PI2;
	
	lastMousePos = currMousePos;

	// Camera rotation
	float maxLimit = (float)85.0 / 180.0 * ZS_PI;
	if (mouseDeltaY < -maxLimit)
		mouseDeltaY = -maxLimit;
	if (mouseDeltaY > maxLimit)
		mouseDeltaY = maxLimit;
	Matrix44 camRotMat = Matrix44::RotationEuler(mouseDeltaY, 0, mouseDeltaX);

	// Rot front vec with that. apply target
	Vec3 frontVec(0, 1, 0);
	frontVec = frontVec * camRotMat;
	
	cam.SetTarget(cam.GetPos() + frontVec);
}