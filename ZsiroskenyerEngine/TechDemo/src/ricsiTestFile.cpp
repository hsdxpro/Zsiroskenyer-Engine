#include "testFiles.h"

#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <conio.h>


std::string GetExecutablePath();
HMODULE LoadDLL(const std::string& libName);
FARPROC GetDLLFunction(HMODULE dll, const std::string& funcName);


int ricsiMain() {
	// Load DLLs
	HMODULE hDllD3D11Dll = LoadDLL("RendererD3D11.dll");
	HMODULE hDllGraphicsEngine = LoadDLL("GraphicsEngine.dll");

	// Get function for IGraphicsApi, IGraphicsEngine, IWindow
	auto CreateGraphicsD3D11 = (IGraphicsApi*(*)())GetDLLFunction(hDllD3D11Dll, "CreateGraphicsD3D11");
	auto CreateGraphicsEngine = (IGraphicsEngine*(*)())GetDLLFunction(hDllGraphicsEngine, "CreateGraphicsEngine");
	auto CreateWindowWin32 = (IWindow* (*)(const IWindow::tDesc& winDesc))GetDLLFunction(hDllGraphicsEngine, "CreateWindowWin32");

	// Create interfaces
	IGraphicsEngine* mgrGEngine = (IGraphicsEngine*)CreateGraphicsEngine();
	ISceneManager* mgrScene = mgrGEngine->GetSceneManager();
	IGraphicsApi *dx11 = (IGraphicsApi*)CreateGraphicsD3D11();

	// Create basic window
	IWindow::tDesc desc;
		desc.appInstance = (int32*)GetModuleHandle(NULL);
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		//desc.style = IWindow::eStyle::OVERLAPPED;
		desc.style = (IWindow::eStyle)WS_OVERLAPPEDWINDOW;
	IWindow* myWindow = (IWindow*)CreateWindowWin32(desc);

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
	
	getch();
	return 0;
}

std::string GetExecutablePath() {
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string s = buf;
	size_t idx = s.find_last_of('\\');
	s = s.substr(0, idx + 1);
	return s;
}

HMODULE LoadDLL(const std::string& libName) {
	HMODULE dll = LoadLibraryA((GetExecutablePath() + libName).c_str());
	if (!dll) {
		HRESULT error = GetLastError();
		MessageBoxA(NULL, "FAILED TO LOAD LIBRARY", "FUCK...", MB_OK|MB_ICONERROR);
		exit(1);
	}
	return dll;
}

FARPROC GetDLLFunction(HMODULE dll, const std::string& funcName) {
	auto function =  GetProcAddress(dll, funcName.c_str());
	if (!function) {
		MessageBoxA(NULL, "FAILED TO GET PROC ADDRESS", "FUCK...", MB_OK|MB_ICONERROR);
		exit(2);
	}
	return function;
}