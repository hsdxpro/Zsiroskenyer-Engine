#pragma once

#include "HagymaBox.h"
#include "../../Core/src/IPhysicsEngine.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/Timer.h"
#include "../../Core/src/Camera.h"

#include "../../Core/src/zsString.h"
#include "../../Core/src/Core.h"
#include <windows.h>

#define CAM_MOVE_SPEED 20
#define PLAYER_MOVE_SPEED 4
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
	pEngine = editor.GetEngineCore()->GetPhysicsEngine();

	cTimer t;
	IWindow::eMessage msg;
	while (window->IsOpened()) {
		while (window->HandleMessage(&msg));

		float deltaT = t.getDeltaSeconds();
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
	Matrix44 camRotMat = Matrix44::RotationEuler(mouseDeltaY, 0, mouseDeltaX);

	// Rot front vec with that. apply target
	Vec3 frontVec(0, 1, 0);
	frontVec = frontVec * camRotMat;

	cam.SetTarget(cam.GetPos() + frontVec);
}