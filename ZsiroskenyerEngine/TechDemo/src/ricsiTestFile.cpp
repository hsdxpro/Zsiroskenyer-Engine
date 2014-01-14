#include "testFiles.h"

// Visualities
#include "../../Core/src/IGraphicsEngine.h"
#include "../../Core/src/GraphicsLight.h"
#include "../../GraphicsEngineRaster/src/ResourceManager.h"
#include "../../Core/src/Camera.h"
#include "../../Core/src/Game.h"
#include "../../Core/src/Level.h"
#include "../../Core/src/Entity.h"
#include "../../Core/src/Core.h"
			
// OS		
#include "../../Core/src/IWindow.h"

// Meth and utils
#include "../../Core/src/Factory.h"
#include "../../Core/src/math/Quat.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

#include "../../Core/src/Timer.h"

// TODO REMOVE THAT OR I KILL MYSELF
#include <windows.h>

using namespace std;

// TODO: hide yo' wife, hide yo' kids, hide yo' warnings
#pragma warning(disable: 4244)
#pragma warning(disable: 4305)

cEntity* player;
IPhysicsEngine* pEngine;

#define CAM_MOVE_SPEED 20
#define PLAYER_MOVE_SPEED 4

void updateDemo(cCamera& cam, float tDelta);

// a lovely light circle
static const int sizeLightCircle = 20;
cGraphicsLight* lightCircle[sizeLightCircle];

int ricsiMain() {
	/*
	cSerializable ser;
	ser << L"STR1";
	ser << 5;
	ser << L"STR2";
	ser << 10;
	ser << L"STR3";

	zsString strs[3];
	size_t val1 = 0;
	size_t val2 = 0;
	ser >> strs[0];
	ser >> val1;
	ser >> strs[1];
	ser >> val2;
	ser >> strs[2];
*/

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
	IGraphicsScene* s = gEngine->CreateScene();
	s->GetCamera() = cCamera(cCamera::tProjPersp(/*0.5*3.141592653589*/1.15f, 1.75f), 0.01f, 5000.0f);

	s->GetState().hdr.enabled = true;


	// Add some fucking lights :)
	cGraphicsLight* sunLight = s->CreateLight();
	cGraphicsLight* skyLight = s->CreateLight();
	cGraphicsLight* secondSunLight = s->CreateLight();
	cGraphicsLight* thirdSunLight = s->CreateLight();	
	cGraphicsLight* pointLight = s->CreateLight();
	cGraphicsLight* spotLight = s->CreateLight();


	for (auto& light : lightCircle) {
		light = s->CreateLight();
	}

	// sunlight
	sunLight->type = cGraphicsLight::DIRECTIONAL;
	sunLight->color = Vec3(1, 1, 1);
	sunLight->direction = Vec3(0.5f, 0.5f, -0.5f).Normalize();
	
	// skylight
	skyLight->type = cGraphicsLight::AMBIENT;
	skyLight->color = Vec3(1, 1, 1);
	
	// other suns... well yeah
	secondSunLight->type = cGraphicsLight::DIRECTIONAL;
	secondSunLight->color = Vec3(1.2, 0.03, 0.95);
	secondSunLight->direction = Vec3(-0.8f, 0.0f, -0.2f).Normalize();

	thirdSunLight->type = cGraphicsLight::DIRECTIONAL;
	thirdSunLight->color = Vec3(0.2, 0.8f, 0.77f);
	thirdSunLight->direction = Vec3(0.8, -0.1f, -0.3f).Normalize();

	sunLight->enabled = true;
	secondSunLight->enabled = thirdSunLight->enabled = false;

	// a big point light
	pointLight->atten0 = 0.0f;
	pointLight->atten1 = 0.2f;
	pointLight->atten2 = 0.2f;
	pointLight->color = Vec3(0.2, 0.2, 0.9)*10;
	pointLight->position = Vec3(8, 8, 2);
	pointLight->range = 10.f;
	pointLight->type = cGraphicsLight::POINT;

	// searchlight :D
	spotLight->atten0 = 0.0f;
	spotLight->atten1 = 0.2f;
	spotLight->atten2 = 0.1f;
	spotLight->smallAngle = 24.0f / 180.0f*3.1415926f;
	spotLight->bigAngle = 30.0f / 180.0f*3.1415926f;
	spotLight->color = Vec3(1.0f, 0.95f, 0.88f)*10.f;
	spotLight->direction = Vec3(0.5, 0.5, -0.5).Normalize();
	spotLight->position = Vec3(0, 0, 10);
	spotLight->range = 80;
	spotLight->type = cGraphicsLight::SPOT;


	// circle of light muhaha
	for (auto& light : lightCircle) {
		light->type = cGraphicsLight::POINT;

		light->atten0 = 0.0f;
		light->atten1 = 0.5f;
		light->atten2 = 0.8f;

		light->position = Vec3(0, 0, 0);
		light->range = 2.5f;
		float H=(float)(&light-lightCircle)/(float)sizeLightCircle,
			S=1.0f,
			L=0.5f;
		float C = (1 - abs(2 * L - 1))*S;
		float H_ = H * 6.0f;
		float X = C*(1 - abs(fmod(H_, 2.0f) - 1.0f));
		Vec3 colorRGB;
		if (0.0f <= H_ && H_ < 1.0f)
			colorRGB.x = C, colorRGB.y = X, colorRGB.z = 0;
		else if (0.1f <= H_ && H_ < 2.0f)
			colorRGB.x = X, colorRGB.y = C, colorRGB.z = 0;
		else if (0.2f <= H_ && H_ < 3.0f)
			colorRGB.x = 0, colorRGB.y = C, colorRGB.z = X;
		else if (0.3f <= H_ && H_ < 4.0f)
			colorRGB.x = 0, colorRGB.y = X, colorRGB.z = C;
		else if (0.4f <= H_ && H_ < 5.0f)
			colorRGB.x = X, colorRGB.y = 0, colorRGB.z = C;
		else if (0.5f <= H_ && H_ <= 6.0f)
			colorRGB.x = C, colorRGB.y = 0, colorRGB.z = X;
		else
			colorRGB = Vec3(0, 0, 0);
		float m = L - 0.5*C;
		colorRGB += Vec3(m, m, m);
		light->color = colorRGB * 2.0f;
	}

	// Static terrain
	zsString staticBaseNames[9] = {		L"coyote",
										L"crate",
										L"demo_cliff_fence",
										L"demo_ground",
										L"demo_house",
										L"demo_road",
										L"demo_tunnel",
										L"fake_tunnel",
										L"tower",
									};

	zsString staticExtension[9] = {		L".dae",
										L".dae",
										L".dae",
										L".dae",
										L".dae",
										L".dae",
										L".dae",
										L".dae",
										L".dae",
									};

	const float mass = 0.0;
	zsString basePath = L"../../Game Assets/";

	ASSERT(sizeof(staticBaseNames) > 0);
	for (size_t i = 0; i < sizeof(staticBaseNames) / sizeof(staticBaseNames[0]); i++) {
		zsString geomPath = basePath + L"objects/" + staticBaseNames[i] + staticExtension[i];
		core.AddEntity(s, geomPath, geomPath, basePath + L"materials/" + staticBaseNames[i] + L".zsm", mass);
	}
	
	// Our player
	player = core.AddEntity(s, basePath + L"objects/character.dae", basePath + L"objects/character.dae", basePath + L"materials/character.zsm", 10.0, false);
	player->SetPos(Vec3(0.3, -19, 0.35));

	// Soft body test
	//cEntity* crate =  core.AddEntity(s, basePath + L"objects/crate.dae", basePath + L"objects/crate.dae", basePath + L"materials/crate.zsm", 10.0, true);
	//crate->SetPos(Vec3(9, 0, 40));

	// Main loop
	IWindow::eMessage msg;
	while(window->IsOpened()) {
		while (window->HandleMessage(&msg)) {
			if (msg == IWindow::eMessage::SIZE_CHANGED) {
#pragma message("Noob solution (float)GetSystemMetrics(SM_CXSCREEN), good solution is to get maximum resolution based on gpu output (monitor)")
				gEngine->Resize((float)GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
				s->GetCamera().SetAspectRatio((float)GetSystemMetrics(SM_CXSCREEN) / GetSystemMetrics(SM_CYSCREEN));
			}
		}
	
		// Update everything
		static cTimer t;
		float deltaT = t.GetDeltaSeconds();

		// Don't hog with set caption text... Fucking slow operation
		static float timer1 = 0.0;
		timer1 += deltaT;
		size_t fps = cTimer::GetFps(deltaT);
		if (timer1 > 1.0f)
		{
			window->SetCaptionText(zsString(L"[Zsíroskenyér Engine 0.1 Beta]  FPS: ") + fps);
			timer1 = 0.0f;
		}

		// Shader reloading
		if (GetAsyncKeyState('R') && GetAsyncKeyState(VK_LCONTROL))
			gEngine->ReloadResources();

		updateDemo(s->GetCamera(), deltaT);
		core.Update(deltaT);

		// Debug rendering
		//core.DebugRender(s, (unsigned long)cCore::eDebugRenderMode::PHYSICS_TRIANGLES);

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

	// update light circle
	static double elapsedTotal = 0.0f;
	elapsedTotal += tDelta/4.f;
	if (elapsedTotal >= 100.0)
		elapsedTotal -= 100.0;
	for (int i = 0; i < sizeLightCircle; i++) {
		double __dummy;
		float angle = modf(elapsedTotal+double(i)/double(sizeLightCircle), &__dummy);
		Vec3 v(10.0, 0.0, 0.0);
		Quat q(Vec3(0,0,1).Normalize(), ZS_PI2*angle);
		v = Quat::RotateVec3_2(v, q);
		lightCircle[i]->position = v;
	}
	

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
	Matrix44 camRotMat;
	camRotMat.RotationEuler(mouseDeltaY, 0, mouseDeltaX);

	// Rot front vec with that. apply target
	Vec3 frontVec(0, 1, 0);
	frontVec = frontVec * camRotMat;
	
	cam.SetTarget(cam.GetPos() + frontVec);
}