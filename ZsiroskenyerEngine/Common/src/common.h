// common.h By Zsíroskenyér team 2013.10.10 1:51
#pragma once

#include <cassert>
#include <iostream>

//@TODO need ILog, cLogWin32, etc....
#define zsPrintError(x) (x)

typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned char uint8;
typedef signed char int8;
typedef wchar_t WCHAR;
typedef char CHAR;

typedef unsigned short ushort;
typedef unsigned char uchar;

#define NULL 0

#if (defined(_DEBUG) || defined(DEBUG))
#define ZSASSERT(condition) assert(condition)
#define ZSASSERT_MSG(condition, errorMsg) \
	if (condition != true) { \
		MessageBoxW(0, errorMsg, L"ErrorMsg", MB_OK); \
		assert(condition); \
	}
#define ZS_MSG(errorMsg) MessageBoxW(0, errorMsg, L"ErrorMsg", MB_OK)
#define zsPrintDebug(msg) std::wcerr<<msg<<std::endl;
#else
#define ZSASSERT(x, y) {}
#define ZSASSERT(x) {}
#define zsPrintDebug(msg) {}
#endif



#define SAFE_DELETE(x) if(x != NULL) {delete x; x = NULL;}
#define SAFE_DELETE_ARRAY(x) if(x != NULL) {delete[] x; x = NULL;}
#define SAFE_FREE(x) if(x != NULL) { free(x); x = NULL;}
#define SAFE_RELEASE(x) if(x != NULL) { x->Release(); x = NULL;}

#define ZS_NUMLIMITMAX(x) std::numeric_limits<x>::max()

#define ZS_PI			3.14159265358979
#define ZS_PI2			6.28318530717958
#define ZS_PIDIV2		1.57079632679489
#define ZS_PIDIV4		0.78539816339744

#define ZS_MAX_CHAR		127
#define ZS_MAX_UCHAR	255
#define ZS_MAX_SHORT	32767
#define ZS_MAX_USHORT	65535
#define ZS_MAX_INT8		NR_MAX_CHAR
#define ZS_MAX_UINT8	NR_MAX_UCHAR
#define ZS_MAX_INT16	NR_MAX_SHORT
#define ZS_MAX_UINT16	NR_MAX_USHORT
#define ZS_MAX_INT32	2147483647
#define ZS_MAX_UINT32	4294967295
#define ZS_MAX_INT64	9223372036854775807
#define ZS_MAX_UINT64	18446744073709551615

#define ZS_MAX_CHAR_CYCLE	NR_MAX_CHAR + 1
#define ZS_MAX_UCHAR_CYCLE	NR_MAX_UCHAR + 1
#define ZS_MAX_SHORT_CYCLE	NR_MAX_SHORT + 1
#define ZS_MAX_USHORT_CYCLE	NR_MAX_USHORT +1
#define ZS_MAX_INT8_CYCLE	NR_MAX_CHAR_CYCLE
#define ZS_MAX_UINT8_CYCLE	NR_MAX_UCHAR_CYCLE
#define ZS_MAX_INT16_CYCLE	NR_MAX_SHORT_CYCLE
#define ZS_MAX_UINT16_CYCLE	NR_MAX_USHORT_CYCLE
#define ZS_MAX_INT32_CYCLE	NR_MAX_INT32 + 1
#define ZS_MAX_UINT32_CYCLE	NR_MAX_UINT32 + 1
#define ZS_MAX_INT64_CYCLE	NR_MAX_INT64 + 1
#define ZS_MAX_UINT64_CYCLE	NR_MAX_UINT64 + 1

#define ZS_MIN_CHAR		-NR_MAX_CHAR - 1
#define ZS_MIN_SHORT	-NR_MAX_SHORT - 1
#define ZS_MIN_INT16	 NR_MIN_SHORT
#define ZS_MIN_INT32	-NR_MAX_INT32 - 1
#define Zs_MIN_INT64	-NR_MAX_INT64 - 1
