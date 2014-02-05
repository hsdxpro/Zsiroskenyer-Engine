#pragma once

#include "HagymaBox.h"
#include "../../Core/src/IPhysicsEngine.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/Timer.h"
#include "../../Core/src/Camera.h"

#include "../../Core/src/zsString.h"
#include "../../Core/src/Core.h"
#include <windows.h>
#include "../../Core/src/GraphicsLight.h"
#include "../../Core/src/Entity.h"

#define CAM_MOVE_SPEED 20
#define PLAYER_MOVE_SPEED 4

// a lovely light circle
static const int sizeLightCircle = 20;
cGraphicsLight* lightCircle[sizeLightCircle];
// sun
cGraphicsLight* sun;

IPhysicsEngine* pEngine;

// I think we need an incredibly flexible GUI library.
// This library contain multifarious gui elements, to build the best user interface
// to iteract with. The lib uses Zsíroskenyér Engine's graphics api library to achieve
// platform independence.

// You can intergrate into your app, just create the gui system with Dx, OpenGL, or
// with some tracer engine. And it will do the job for you.
// Or you can implement some interfaces and that's all :)

// The gui system that i create is just test, so sources are stay in HagymaBox project

// Bullshit camera for demo
void updateDemo(cCamera& cam, float deltaT);

int main() {
	// Create window
	IWindow::tDesc winDesc;
		winDesc.brush = IWindow::eBrush::RENDER_;
		winDesc.captionName = L"HagymaBox 0.1";
		winDesc.clientHeight = 600;
		winDesc.clientWidth = 800;
		winDesc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* window = IWindow::Create(winDesc);

	
	cHagymaBox editor(window);
	cCore* core = editor.GetEngineCore();
	pEngine = editor.GetEngineCore()->GetPhysicsEngine();

	IGraphicsScene* s = editor.GetScene();
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
	sunLight->direction = Vec3(.5f, .5f, -0.1f).Normalize();
	sun = sunLight;

	// skylight
	Vec3 skyColor(0.1, 0.2, 0.3);
	skyColor /= 0.2126f*skyColor.x + 0.7152f*skyColor.y + 0.0722f*skyColor.z;
	skyColor *= 0.3f;
	skyLight->type = cGraphicsLight::AMBIENT;
	skyLight->color = skyColor;

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
	pointLight->color = Vec3(0.2, 0.2, 0.9) * 10;
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
		float H = (float)(&light - lightCircle) / (float)sizeLightCircle,
			S = 1.0f,
			L = 0.5f;
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


	const int nEntities = 9;
	// Static terrain
	static const zsString staticBaseNames[nEntities] = { L"coyote",
		L"crate",
		L"demo_cliff_fence",
		L"demo_ground",
		L"demo_house",
		L"demo_road",
		L"demo_tunnel",
		L"fake_tunnel",
		L"tower" };

	static const zsString staticExtension[nEntities] = { L".dae",
		L".dae",
		L".dae",
		L".dae",
		L".dae",
		L".dae",
		L".dae",
		L".dae",
		L".dae",
	};
	static const Vec3 objPos[nEntities] = {
		Vec3(3.72f, 30.19f, 1.54f), // coyote
		Vec3(0, 0, 0), // crate
		Vec3(0, 0, 0), // cliff
		Vec3(0, 0, 0), // ground
		Vec3(18.14f, 7.26f, 0.48f), // ház
		Vec3(0, 0, 0), // road
		Vec3(0, 0, 0), // tunnel
		Vec3(6.24f, 30.18f, 3.63f), // coyote tunnel painting
		Vec3(-20.95f, -17.15f, 8.00f), // tunnel
	};
	static const Vec3 objRot[nEntities] = {
		Vec3(90.0 / 180.0*ZS_PI, 0, -14.892 / 180.0*ZS_PI),
		Vec3(0, 0, 0),
		Vec3(0, 0, 0),
		Vec3(0, 0, 0),
		Vec3(0, 0, 0),
		Vec3(0, 0, 0),
		Vec3(0, 0, 0),
		Vec3(90.0 / 180.0*ZS_PI, 0, -6.64 / 180.0*ZS_PI),
		Vec3(0, 0, 0),
	};
	const float mass = 0.0;
	zsString basePath = L"../../Game Assets/";

	ASSERT(sizeof(staticBaseNames) > 0);
	cEntity* entities[nEntities];
	for (size_t i = 0; i < sizeof(staticBaseNames) / sizeof(staticBaseNames[0]); i++) {
		zsString geomPath = basePath + L"objects/" + staticBaseNames[i] + staticExtension[i];
		auto entity = core->AddEntity(s, geomPath, geomPath, basePath + L"materials/" + staticBaseNames[i] + L".zsm", mass);
		entity->SetPos(objPos[i]);
		Quat rot = Quat::EulerAnglesToQuat(objRot[i]);
		entity->SetRot(rot);
	}

	cEntity* tmp = core->AddEntity(s, basePath + L"objects/multi-mat_tetraeder.dae", basePath + L"objects/multi-mat_tetraeder.dae", basePath + L"materials/multi_mat_teszt.zsm", 0.0f);
	tmp->SetPos(Vec3(0, 0, 10));

	// Our player
	cEntity* player = core->AddEntity(s, basePath + L"objects/character.dae", basePath + L"objects/character.dae", basePath + L"materials/character.zsm", 10.0, false);
	player->SetPos(Vec3(0.3, -19, 0.35));

	cTimer t;
	IWindow::eMessage msg;
	while (window->IsOpened()) {
		while (window->HandleMessage(&msg));

		float deltaT = t.GetDeltaSeconds();
		size_t fps = cTimer::GetFps(deltaT);

		// Don't hog with set caption text... Fucking slow operation
		static float timer1 = 0.0;
		timer1 += deltaT;
		if (timer1 > 1.0f) {
			window->SetCaptionText(zsString(L"[Zsíroskenyér Engine 0.1 Beta]  FPS: ") + fps);
			timer1 = 0.0f;
		}

		updateDemo(editor.GetCamera(), deltaT);
		editor.Update(deltaT);
	}
}


bool noJump = false;
void updateDemo(cCamera& cam, float deltaT) {
	// Shooting boxes
	if (GetAsyncKeyState(VK_LBUTTON))
		pEngine->ShootBox(0.3f, cam.GetPos(), cam.GetDirFront(), 60); // This function in the interface is just for test purposes

	// CAMERA MOVING
	Vec3 deltaMove(0, 0, 0);
	if (GetAsyncKeyState('W'))
		deltaMove += cam.GetDirFront() * (CAM_MOVE_SPEED * deltaT);
	if (GetAsyncKeyState('S'))
		deltaMove += cam.GetDirBack()  * (CAM_MOVE_SPEED * deltaT);
	if (GetAsyncKeyState('A'))
		deltaMove += cam.GetDirLeft()  * (CAM_MOVE_SPEED * deltaT);
	if (GetAsyncKeyState('D'))
		deltaMove += cam.GetDirRight() * (CAM_MOVE_SPEED * deltaT);

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
	Matrix44 camRotMat = Matrix44RotationEuler(mouseDeltaY, 0, mouseDeltaX);

	// Rot front vec with that. apply target
	Vec3 frontVec(0, 1, 0);
	frontVec = frontVec * camRotMat;

	cam.SetTarget(cam.GetPos() + frontVec);
}