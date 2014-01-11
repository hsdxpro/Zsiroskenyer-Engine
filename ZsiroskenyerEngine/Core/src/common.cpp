#include "common.h"

void Realloc(void* mem, size_t oldSize, size_t newSize) {
	// Save old memory
	char* tmpData = new char[newSize];
	memcpy(tmpData, mem, oldSize);
	SAFE_DELETE_ARRAY(mem);
	mem = new char[newSize];
	memcpy(mem, tmpData, oldSize);
}
