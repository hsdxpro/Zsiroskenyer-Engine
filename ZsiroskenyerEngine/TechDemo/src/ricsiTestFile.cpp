#include "testFiles.h"

#include "../../GraphicsCommon/src/IWindow.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../GraphicsCommon/src/IGraphicsEngine.h"
#include "../../GraphicsCommon/src/Camera.h"
#include "../../CommonLib/src/Factory.h"
#include "../../CommonLib/src/DLLLoaderWin32.h"

int ricsiMain() {
	
	// Create interfaces
	IGraphicsEngine* mgrGEngine = Factory.CreateEngineGraphics();
	IManagerScene* mgrScene = mgrGEngine->GetSceneManager();
	IGraphicsApi* gApi = mgrGEngine->GetGraphicsApi();

	// Create basic window
	IWindow::tDesc desc;
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* myWindow = Factory.CreateWindow(desc);

	// Set Render Window
	gApi->SetWindow(myWindow);

	// Create camera for testing and use it
	cCamera *cam = new cCamera(ZS_PIDIV2, 1.0, 0.01f, 5000.0f);
	mgrScene->SetActiveCamera(cam);

	// Create 3D object for testing
	cEntity& entity = mgrScene->AddEntity(L"box.dae", L"material");
		entity.position = Vec3(1,1,1);
		entity.isVisible = true;

	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();
		mgrGEngine->RenderSceneForward();
		gApi->Present();
	}
	
	// Free up
	SAFE_DELETE(mgrGEngine);
	SAFE_DELETE(cam);
	return 0;
}