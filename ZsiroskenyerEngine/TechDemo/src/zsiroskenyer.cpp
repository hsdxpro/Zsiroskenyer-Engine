/*#include "..\..\GraphicsEngine\src\WindowWin32.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"*/
#include "..\..\CommonLib\src\zsString.h"

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <conio.h>

std::string GetExecutablePath();
HMODULE LoadDLL(const std::string& libName);
FARPROC GetDLLFunction(HMODULE dll, const std::string& funcName);


int main() {
	// zsString test
	



	/*
	// Load DLL
	HMODULE D3D11Dll = LoadDLL("RendererD3D11.dll");

	// Get function from dll
	auto CreateGraphicsD3D11 = GetDLLFunction(D3D11Dll, "CreateGraphicsD3D11");

	// Create D3D11 renderer
	IGraphicsApi * dx11 = (IGraphicsApi*)CreateGraphicsD3D11();
	dx11->Present();
	*/

	getch();

	return 0;
}

/*
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
	auto function =  GetProcAddress(dll, "CreateGraphicsD3D11");
	if (!function) {
		MessageBoxA(NULL, "FAILED TO GET PROC ADDRESS", "FUCK...", MB_OK|MB_ICONERROR);
		exit(2);
	}
	return function;
}
*/