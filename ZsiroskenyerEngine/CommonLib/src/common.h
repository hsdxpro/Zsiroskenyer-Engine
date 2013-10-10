// common.h By Zsíroskenyér team 2013.10.10 1:51
#pragma once

#include <cassert>
#include <iostream>

//@TODO need ILog, cLogWin32, etc....
#define zsErrorMsg(x) (x)

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

#ifndef NULL
#define NULL 0
#endif

#if (defined(_DEBUG) || defined(DEBUG))
#define ASSERT(condition, errorMsg) std::cout<<errorMsg<<std::endl; assert(condition)
#define ASSERT(condition) assert(condition)
#else
#define ASSERT(x, y) {}
#define ASSERT(x) {}
#endif

#define SAFE_DELETE(x) if(x != NULL) {delete x; x = NULL;}
#define SAFE_DELETE_ARRAY(x) if(x != NULL) {delete[] x; x = NULL;}
#define SAFE_FREE(x) if(x != NULL) { free(x); x = NULL;}
#define SAFE_RELEASE(x) if(x != NULL) { x->Release(); x = NULL;}

#define NR_MAX_CHAR		127
#define NR_MAX_UCHAR	255
#define NR_MAX_SHORT	32767
#define NR_MAX_USHORT	65535
#define NR_MAX_INT8		NR_MAX_CHAR
#define NR_MAX_UINT8	NR_MAX_UCHAR
#define NR_MAX_INT16	NR_MAX_SHORT
#define NR_MAX_UINT16	NR_MAX_USHORT
#define NR_MAX_INT32	2147483647
#define NR_MAX_UINT32	4294967295
#define NR_MAX_INT64	9223372036854775807
#define NR_MAX_UINT64	18446744073709551615

#define NR_MAX_CHAR_CYCLE	NR_MAX_CHAR + 1
#define NR_MAX_UCHAR_CYCLE	NR_MAX_UCHAR + 1
#define NR_MAX_SHORT_CYCLE	NR_MAX_SHORT + 1
#define NR_MAX_USHORT_CYCLE	NR_MAX_USHORT +1
#define NR_MAX_INT8_CYCLE	NR_MAX_CHAR_CYCLE
#define NR_MAX_UINT8_CYCLE	NR_MAX_UCHAR_CYCLE
#define NR_MAX_INT16_CYCLE	NR_MAX_SHORT_CYCLE
#define NR_MAX_UINT16_CYCLE	NR_MAX_USHORT_CYCLE
#define NR_MAX_INT32_CYCLE	NR_MAX_INT32 + 1
#define NR_MAX_UINT32_CYCLE	NR_MAX_UINT32 + 1
#define NR_MAX_INT64_CYCLE	NR_MAX_INT64 + 1
#define NR_MAX_UINT64_CYCLE	NR_MAX_UINT64 + 1

#define NR_MIN_CHAR		-NR_MAX_CHAR - 1
#define NR_MIN_SHORT	-NR_MAX_SHORT - 1
#define NR_MIN_INT16	NR_MIN_SHORT
#define NR_MIN_INT32	-NR_MAX_INT32 - 1
#define NR_MIN_INT64	-NR_MAX_INT64 - 1