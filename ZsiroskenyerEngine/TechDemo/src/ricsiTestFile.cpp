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
#include <tchar.h> // even worse

cEntity* player = NULL;

#define CAM_MOVE_SPEED 20
void updateDemo(cCamera& cam, float tDelta);

int ricsiMain() {
	MessageBox(NULL,
		_T("A jobb gombot le kell nyomni és csak úgy forog a camera.\nEzt a szart meg töröld ki! :D"),
		_T("He, Ricsi!"),
		MB_ICONWARNING);


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
		core->AddEntity(geomPath, geomPath, basePath + L"materials/" + staticBaseNames[i] + L".zsm", mass);
	}
	
	// Our player
	player = core->AddEntity(basePath + L"objects/character.dae", basePath + L"objects/character.dae", basePath + L"materials/character.zsm", 1.0);
	player->SetPos(Vec3(9, 0, 40));

	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		// Clear BB
		gApi->Clear(true, true);
	
		// Update everything
		float tDelta = cTimer::getDeltaSeconds();
		size_t fps = cTimer::GetFps(tDelta);
		updateDemo(cam, tDelta);
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
		gEngine->Update();

		// Debug rendering
		core->DebugRender((unsigned long)cCore::eDebugRenderMode::PHYSICS_TRIANGLES);

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	SAFE_DELETE(core);
	return 0;
}

void updateDemo(cCamera& cam, float tDelta) {
	/*
	static Quat playerRot;
	Vec3 playerDir = Vec3(0, 1, 0) * playerRot;
	const Vec3 up(0, 0, 1);
	Vec3 playerPos(9, 0, 4);

// Controlling player with WSAD
	Vec3 deltaMove(0, 0, 0);
	if (GetAsyncKeyState('W'))
		deltaMove += playerDir  * tDelta;
	if (GetAsyncKeyState('S'))
		deltaMove += -playerDir  * tDelta;
	if (GetAsyncKeyState('A'))
		deltaMove += Vec3::Cross(playerDir, up)   * tDelta;
	if (GetAsyncKeyState('D'))
		deltaMove += Vec3::Cross(up, playerDir)   * tDelta;
	playerPos += deltaMove;

	player->SetPos(playerPos);
	*/
// CAMERA MOVING
	Vec3 deltaMove = Vec3(0, 0, 0);
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


	// CAMERA ROTATION...
	if (!GetAsyncKeyState(VK_RBUTTON)) {
		return;
	}
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

	mouseDeltaX += (float)(-currMousePos.x + lastMousePos.x) / pixelsPer360ROT * ZS_PI2;
	mouseDeltaY += (float)(-currMousePos.y + lastMousePos.y) / pixelsPer360ROT * ZS_PI2;

	lastMousePos = currMousePos;

	// Camera rotation
	Matrix44 camRotMat = Matrix44::RotationEuler(mouseDeltaY, 0, mouseDeltaX);

	// Rot front vec with that. apply target
	Vec3 frontVec(0, 1, 0);
	frontVec = frontVec * camRotMat;
	

	cam.SetTarget(cam.GetPos() + frontVec);
}