#include "testFiles.h"

#include "../../GraphicsCommon/src/IWindow.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../GraphicsCommon/src/IGraphicsEngine.h"
#include "../../GraphicsCommon/src/Camera.h"
#include "../../CommonLib/src/Factory.h"
#include "../../CommonLib/src/DLLLoaderWin32.h"
#include "../../CommonLib/src/math/Quat.h"

#include <vector>

int ricsiMain() {
	
	// Create interfaces
	IGraphicsEngine* mgrGEngine = Factory.CreateEngineGraphics();
	IManagerScene* mgrScene = mgrGEngine->GetSceneManager();
	IGraphicsApi* gApi = mgrGEngine->GetGraphicsApi();

	// Create basic window
	IWindow::tDesc desc;
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zs�roskeny�r Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* myWindow = Factory.CreateWindow(desc);

	// Set Render Window
	gApi->SetWindow(myWindow);

	// Create camera
	cCamera *cam = new cCamera(ZS_PIDIV2, (float)desc.clientWidth / desc.clientHeight, 0.01f, 5000.0f);
	mgrScene->SetActiveCamera(cam);

	std::vector<cEntity*> entities;
	// Create 3D object

	for(size_t i = 0; i < 1 ; i++)
		for(size_t j = 0; j < 1 ; j++) {
			cEntity& e =  mgrScene->AddEntity(L"box.dae", L"material");
			entities.push_back(&e);
			e.position = Vec3(i * 10, 90, j * 10);
			e.isVisible = true;
		}

	
	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();

		mgrGEngine->RenderSceneForward();

		// Hardcoded rotation 	( euler Z for rotationg entities )
		static float zVal = 0.0f;
		zVal += 0.001f;
		for(cEntity* e : entities) {
			e->rotation = Quat::EulerAnglesToQuat(Vec3(zVal, zVal + 0.4, zVal));
		}

		// Present SwapChain
		gApi->Present();
	}
	
	// Free up
	mgrGEngine->Release();
	SAFE_DELETE(cam);
	return 0;
}