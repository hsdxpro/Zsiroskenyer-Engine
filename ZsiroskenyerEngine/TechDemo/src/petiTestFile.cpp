#include "testFiles.h"
#include <Windows.h>


#include "..\..\GraphicsEngine\src\IGraphicsEngine.h"



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



int petiMain() {
	// load dll

	HMODULE hDll = LoadDLL("GraphicsEngine.dll");
	auto CreateGraphicsEngine = (IGraphicsEngine*(*)())GetDLLFunction(hDll, "CreateGraphicsEngine");

	IGraphicsEngine* engine = CreateGraphicsEngine();
	ISceneManager* sceneManager = engine->GetSceneManager();

	cEntity& entity = sceneManager->AddEntity(L"geometry", L"material");


	return 0;
}