#include "testFiles.h"

#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"
#include "..\..\CommonLib\src\Factory.h"
#include "..\..\CommonLib\src\DLLLoaderWin32.h"

int ricsiMain() {
	
	// Create interfaces
	IGraphicsEngine* mgrGEngine = Factory.CreateEngineGraphics();
	IManagerScene* mgrScene = mgrGEngine->GetSceneManager();
	IGraphicsApi* dx11 = Factory.CreateGraphics();

	
	// Create basic window
	IWindow::tDesc desc;
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* myWindow = Factory.CreateWindow(desc);

	// Set Render Window
	dx11->SetWindow(myWindow);

	// Create 3D object for testing
	cEntity& entity = mgrScene->AddEntity(L"box.dae", L"material");
		entity.position = Vec3(1,1,1);
		entity.isVisible = true;

	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();
		dx11->Present();
	}
	
	// Free up
	SAFE_DELETE(mgrGEngine);
	SAFE_DELETE(mgrScene);
	SAFE_DELETE(dx11);
	
	return 0;
}