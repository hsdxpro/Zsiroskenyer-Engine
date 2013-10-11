// DLLLoaderWin32.h By Zsíroskenyér Team 2013.10.11 11:25
// DLL loading on win32
#pragma once

#include "IDLLLoader.h"

class cDLLLoaderWin32 : public IDLLLoader {
public:
	IDLLLoader::DLLMODULE LoadDLL(const std::string& libName) const;
	IDLLLoader::DLLFUNCTION GetDLLFunction(IDLLLoader::DLLMODULE dll, const std::string& funcName) const;

protected:
	std::string GetExecutablePath() const;
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IDLLLoader* CreateDLLLoaderWin32() {
		return new cDLLLoaderWin32();
}