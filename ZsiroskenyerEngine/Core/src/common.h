// common.h By Zsíroskenyér team 2013.10.10 1:51
#pragma once

#include <cassert>
#include "ILog.h"

#define NULL 0

#if (defined(_DEBUG) || defined(DEBUG))
#define ASSERT(condition) assert(condition)
#define ASSERT_MSG(condition, errorMsg) \
	if (condition != true) { \
		ILog::GetInstance()->MsgBox(errorMsg); \
		assert(condition); \
	}
#else
#define ASSERT_MSG(x, y) {}
#define ASSERT(x) {}
#endif


#define SAFE_DELETE(x) if(x != NULL) {delete x; x = NULL;}
#define SAFE_DELETE_ARRAY(x) if(x != NULL) {delete[] x; x = NULL;}
#define SAFE_FREE(x) if(x != NULL) { free(x); x = NULL;}
#define SAFE_RELEASE(x) if(x != NULL) { x->Release(); x = NULL;}

#define ZS_MAX(x) std::numeric_limits<x>::max()
#define ZS_MIN(x) std::numeric_limits<x>::min()

#define ZS_PI			3.141592653589793238462643383279
#define ZS_PI2			6.28318530717958
#define ZS_PIDIV2		1.57079632679489
#define ZS_PIDIV4		0.78539816339744

