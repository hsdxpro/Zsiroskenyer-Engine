#include "ILog.h"

#include "../../CommonOS/src/Log.h"
#include "common.h"

ILog* ILog::instance = NULL;

ILog* ILog::GetInstance() {
	if(ILog::instance == NULL)
		ILog::instance = new cLog();
	return ILog::instance;
}