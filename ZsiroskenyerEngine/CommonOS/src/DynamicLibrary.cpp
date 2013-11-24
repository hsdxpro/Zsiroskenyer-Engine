////////////////////////////////////////////////////////////////////////////////
//	Win32 dynamic library implementation
//
////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../../Core/src/DynamicLibrary.h"


DllHandle LoadDynamicLibrary(const wchar_t* fileName) {
	HMODULE hModule = LoadLibraryW(fileName);
	return (DllHandle)hModule;
}
DllHandle LoadDynamicLibrary(const char* fileName) {
	HMODULE hModule = LoadLibraryA(fileName);
	return (DllHandle)hModule;
}


void UnloadDynamicLibrary(DllHandle module) {
	FreeLibrary((HMODULE)module);
}


void* GetFunctionAddress(DllHandle module, const char* funcName) {
	HMODULE hModule = (HMODULE)module;
	void* address = GetProcAddress(hModule, funcName);
}