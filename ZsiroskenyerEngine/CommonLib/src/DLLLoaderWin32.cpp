// DLLLoaderWin32.cpp implementation
#include "DLLLoaderWin32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::string cDLLLoaderWin32::GetExecutablePath() const {
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string s = buf;
	size_t idx = s.find_last_of('\\');
	s = s.substr(0, idx + 1);
	return s;
}

IDLLLoader::DLLMODULE cDLLLoaderWin32::LoadDLL(const std::string& libName) const {
	HMODULE dll = LoadLibraryA((GetExecutablePath() + libName).c_str());
	if (!dll) {
		HRESULT error = GetLastError();
		MessageBoxA(NULL, "FAILED TO LOAD LIBRARY", "FUCK...", MB_OK|MB_ICONERROR);
		exit(1);
	}
	return dll;
}

IDLLLoader::DLLFUNCTION cDLLLoaderWin32::GetDLLFunction(IDLLLoader::DLLMODULE dll, const std::string& funcName) const {
	auto function =  GetProcAddress((HMODULE)dll, funcName.c_str());
	if (!function) {
		MessageBoxA(NULL, "FAILED TO GET PROC ADDRESS", "FUCK...", MB_OK|MB_ICONERROR);
		exit(2);
	}
	return (DLLFUNCTION)function;
}