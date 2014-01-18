//FileUtil.h By Zsíroskenyér Team 2014.01.11 22:00
#pragma once

#include <fstream>
#include <list>
#include "zsString.h"

class cFileUtil {
public:
	static bool Clear(const zsString& path);

	static bool ReadBinary(std::ifstream& file, void* data, size_t dataSize);

	static bool WriteBinary(std::ofstream& file, void* data, size_t dataSize);


	static bool isFileExits(const zsString& str);

	static std::list<zsString> GetLines(std::ifstream& file);
	static int GetSize(const zsString& path);
};