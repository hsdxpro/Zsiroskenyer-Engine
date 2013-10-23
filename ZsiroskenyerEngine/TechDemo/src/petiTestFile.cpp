#include "testFiles.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <iostream>

using namespace std;


#include "../../Core/src/IGraphicsEngine.h"

namespace peti_test {

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

} // namespace pet_test

using namespace peti_test;

int petiMain() {
	// load dll

	HMODULE hDll = LoadDLL("GraphicsEngine.dll");
	auto CreateGraphicsEngine = (IGraphicsEngine*(*)())GetDLLFunction(hDll, "CreateGraphicsEngine");

	IGraphicsEngine* engine = CreateGraphicsEngine();
	ISceneManager* sceneManager = engine->GetManagerScene();

	cGraphicsEntity& entity = sceneManager->AddEntity(L"geometry", L"material");

	entity.position = Vec3(1,1,1);
	entity.isVisible = true;


	return 0;
}