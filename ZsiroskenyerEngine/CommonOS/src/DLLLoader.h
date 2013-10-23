// DLLLoader.h By Zsíroskenyér Team 2013.10.11 11:25
// DLL loading on win32
#pragma once

#include "../../Core/src/IDLLLoader.h"

class cDLLLoader : public IDLLLoader {
public:
	IDLLLoader::DLLMODULE LoadDLL(const zsString& libName) const override;
	IDLLLoader::DLLFUNCTION GetDLLFunction(IDLLLoader::DLLMODULE dll, const zsString& funcName) const override;

protected:
	zsString GetExecutablePath() const;
};