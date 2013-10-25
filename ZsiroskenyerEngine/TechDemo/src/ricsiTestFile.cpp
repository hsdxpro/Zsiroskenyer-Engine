#include "testFiles.h"

// Visualities
#include "../../Core/src/IGraphicsEngine.h"
#include "../../GraphicsEngineRaster/src/ResourceManager.h"
#include "../../Core/src/Camera.h"
#include "../../Core/src/Game.h"
#include "../../Core/src/Level.h"
#include "../../Core/src/Entity.h"
#include "../../Core/src/Core.h"

// OS
#include "../../Core/src/IWindow.h"

// Math and utils
#include "../../Core/src/Factory.h"
#include "../../Core/src/math/Quat.h"
#include <vector>

int ricsiMain() {

	// Create engine...
	cCore* core = cCore::GetInstance();
	IGraphicsEngine* gEngine = core->GetGraphicsEngine();
	// gApi, Ugly boy...
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

	// Create Game based on camera
	cGame ricsiGame(&cam);

	// Add, set new level
	cLevel level01;
	ricsiGame.SetActiveLevel(&level01);


	// Create 3D objects
	std::vector<cEntity*> entities;
	for(size_t i = 0; i < 1; i++)
		for(size_t j = 0; j < 1 ; j++) {
			//cEntity& e = core->AddEntity(L"objects/box.dae", L"materials/test.zsm", 100);
//				e.SetPosition(i * 10, 90, j * 10);
				//e.SetVisibility(true);
			//level01.add(&e);
	}	
	
	// Main loop
	while(window->IsOpened()) {
		window->PeekAllMessages();

		//gEngine->RenderLevel(&level01);

		// Hardcoded rotation 	( euler Z for rotationg entities ) (GAME LOGIC WHAOOO :D)
		static float zVal = 0.0f;
		zVal += 0.001f;
		for(cEntity *e : entities) {
			//e->SetRotation(Quat::EulerAnglesToQuat(zVal, zVal + 0.4, zVal);
		}

		// Present SwapChain
		gApi->Present();
	}
	
	SAFE_DELETE(window);
	SAFE_DELETE(core);
	return 0;
}