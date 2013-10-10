#include "..\..\GraphicsEngine\src\WindowWin32.h"
#include "..\..\GraphicsEngine\src\IGraphicsApi.h"

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

using namespace std;


string GetExecutablePath() {
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	string s = buf;
	size_t idx = s.find_last_of('\\');
	s = s.substr(0, idx+1);
	return s;
}



int main() {
	string currentDir = GetExecutablePath();
	HMODULE hGraphicsApi = LoadLibraryA((currentDir+"RendererD3D11.dll").c_str());
	if (!hGraphicsApi) {
		MessageBoxA(NULL, "FAILED TO LOAD LIBRARY", "FUCK...", MB_OK|MB_ICONERROR);
		return 1;
	}

	auto CreateGraphicsD3D11 = (IGraphicsApi*(*)())GetProcAddress(hGraphicsApi, "CreateGraphicsD3D11");
	if (!CreateGraphicsD3D11) {
		MessageBoxA(NULL, "FAILED TO GET PROC ADDRESS", "FUCK...", MB_OK|MB_ICONERROR);
		return 2;
	}


	IGraphicsApi * dx11 = CreateGraphicsD3D11();


	return 0;
}