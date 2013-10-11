// IDLLLoader.h By Zsíroskenyér Team 2013.10.11 11:19
// Interface for DLL loading
#pragma once

#include "zsString.h"

class IDLLLoader {
public:
	typedef void* DLLMODULE;
	typedef int (*DLLFUNCTION)();

	virtual DLLMODULE LoadDLL(const zsString& libName) const = 0;
	virtual DLLFUNCTION GetDLLFunction(DLLMODULE dll, const zsString& funcName) const = 0;
};