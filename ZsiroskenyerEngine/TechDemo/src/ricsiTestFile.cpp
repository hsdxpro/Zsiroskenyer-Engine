#include "testFiles.h"

#include "../../GraphicsCommon/src/IWindow.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../GraphicsCommon/src/IGraphicsEngine.h"
#include "../../GraphicsCommon/src/Camera.h"
#include "../../CommonLib/src/Factory.h"
#include "../../CommonLib/src/DLLLoaderWin32.h"
#include "../../CommonLib/src/math/Quat.h"

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

	// Create camera
	cCamera *cam = new cCamera(ZS_PIDIV2, (float)desc.clientWidth / desc.clientHeight, 0.01f, 5000.0f);
	mgrScene->SetActiveCamera(cam);

	// Create 3D object
	cEntity& entity = mgrScene->AddEntity(L"box.dae", L"material");
		entity.position = Vec3(0,40,0);
		entity.isVisible = true;

	// euler Z for rotationg entity
	
	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();

		mgrGEngine->RenderSceneForward();

		// Hardcoded rotation
		static float zVal = 0.0f;
		zVal += 0.001f;
		entity.rotation = Quat::EulerAnglesToQuat(Vec3(zVal, zVal + 0.4, zVal));

		// Present SwapChain
		gApi->Present();
	}
	
	// Free up
	mgrGEngine->Release();
	SAFE_DELETE(cam);
	return 0;
}