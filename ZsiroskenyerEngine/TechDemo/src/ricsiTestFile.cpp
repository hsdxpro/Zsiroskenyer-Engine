#include "testFiles.h"

#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"
#include "..\..\CommonLib\src\Factory.h"
#include "..\..\CommonLib\src\DLLLoaderWin32.h"

//@TODO REMOVE THIS FUCKING SHIT OR I KILL MYSELF
// NEED SOME INTERFACE, TO DETERMINE APPLICATION HANDLE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int ricsiMain() {
	
	// Create interfaces
	IGraphicsEngine* mgrGEngine = Factory.CreateEngineGraphics();
	IManagerScene* mgrScene = mgrGEngine->GetSceneManager();
	IGraphicsApi* dx11 = Factory.CreateGraphics();

	
	// Create basic window
	IWindow::tDesc desc;
		desc.appInstance = (IWindow::AppInstance)GetModuleHandle(NULL);
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;

//@TODO I KILL MYSELF
#undef CreateWindow

	IWindow* myWindow = Factory.CreateWindow(desc);

//@TODO NEM FUT LE LOL
	// Set Render Window
	dx11->SetWindow(myWindow);

	// Create 3D object for testing
	cEntity& entity = mgrScene->AddEntity(L"box.dae", L"material");
		entity.position = Vec3(1,1,1);
		entity.isVisible = true;

	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();
		dx11->BBClear();
		dx11->BBPresent();
	}
	
	// Free up
	SAFE_DELETE(mgrGEngine);
	SAFE_DELETE(mgrScene);
	SAFE_DELETE(dx11);
	
	return 0;
}