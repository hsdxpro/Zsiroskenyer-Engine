////////////////////////////////////////////////////////////////////////////////
//	File: DllMain.cpp
//	Created:
//		2013. oct. 09.
//		by P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Content:
//		Windows boilerplate DllMain function.
//
////////////////////////////////////////////////////////////////////////////////


#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>


BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD reason, void* reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		return TRUE;;
	case DLL_PROCESS_DETACH:
		return TRUE;;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


#endif // #ifdef WIN_32