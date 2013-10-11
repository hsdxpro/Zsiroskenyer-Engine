// IDLLLoader.h By Zsíroskenyér Team 2013.10.11 11:19
// Interface for DLL loading
#pragma once

#include <string>

class IDLLLoader {
public:
	typedef void* DLLMODULE;
	typedef int (*DLLFUNCTION)();

	virtual DLLMODULE LoadDLL(const std::string& libName) const = 0;
	virtual DLLFUNCTION GetDLLFunction(DLLMODULE dll, const std::string& funcName) const = 0;
};