#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"

//#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <conio.h>

std::string GetExecutablePath();
HMODULE LoadDLL(const std::string& libName);
FARPROC GetDLLFunction(HMODULE dll, const std::string& funcName);


int ricsiMain() {
	zsString a = L"kakI";


	// Load DLLs
	HMODULE D3D11Dll = LoadDLL("RendererD3D11.dll");
	HMODULE graphicsEngine = LoadDLL("GraphicsEngine.dll");

	// Get function for IGraphicsApi
	auto CreateGraphicsD3D11 = GetDLLFunction(D3D11Dll, "CreateGraphicsD3D11");

	
	// Get function for IWindow
	typedef IWindow* (*func)(const IWindow::tDesc& winDesc);
	auto createWindowWin32 = (func)GetDLLFunction(graphicsEngine, "CreateWindowWin32");

	// Create basic window
	IWindow::tDesc desc;
	/*
		desc.appInstance = (int32*)GetModuleHandle(NULL);
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = zsString("EnerIce Engine [Tech Demo]");
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;
		*/
	IWindow* myWindow = (IWindow*)createWindowWin32(desc);

	// Create Dx11 device
	IGraphicsApi * dx11 = (IGraphicsApi*)CreateGraphicsD3D11();

	dx11->Present();

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
	HMODULE dll = LoadLibraryA((GetExecutablePath() + "RendererD3D11.dll").c_str());
	if (!dll) {
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