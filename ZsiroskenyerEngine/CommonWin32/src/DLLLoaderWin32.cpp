// DLLLoaderWin32.cpp implementation
#include "DLLLoaderWin32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../Common/src/zsString.h"
#include "../../Common/src/common.h"

zsString cDLLLoaderWin32::GetExecutablePath() const {
	WCHAR buf[MAX_PATH];
	GetModuleFileName(NULL, buf, MAX_PATH);
	zsString s = buf;
	size_t idx = s.find_last_of('\\');
	s = s.substr(0, idx + 1);
	return s;
}

IDLLLoader::DLLMODULE cDLLLoaderWin32::LoadDLL(const zsString& libName) const {
	HMODULE dll = LoadLibrary((GetExecutablePath() + libName).c_str());
	if (!dll) {
		ZS_MSG((L"FAILED TO LOAD LIBRARY: " + libName).c_str());
		exit(1);
	}
	return dll;
}

IDLLLoader::DLLFUNCTION cDLLLoaderWin32::GetDLLFunction(IDLLLoader::DLLMODULE dll, const zsString& funcName) const {
	char ansiChars[256];

	wcstombs(ansiChars, funcName.c_str(), 256);

	auto function =  GetProcAddress((HMODULE)dll, ansiChars);
	if (!function) {
		MessageBoxW(NULL, ( L"FAILED TO GET PROC ADDRESS: " + funcName).c_str(), L"FUCK...", MB_OK|MB_ICONERROR);
		exit(2);
	}
	return (DLLFUNCTION)function;
}