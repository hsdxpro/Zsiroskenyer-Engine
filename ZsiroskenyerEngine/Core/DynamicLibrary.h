////////////////////////////////////////////////////////////////////////////////
//	The interface for working with dynamic libraries.
//	To be implemented in OS specific static libraries.
////////////////////////////////////////////////////////////////////////////////



typedef unsigned long long DllHandle;



DllHandle LoadDynamicLibrary(const wchar_t* fileName);
void UnloadDynamicLibrary(DllHandle module);

