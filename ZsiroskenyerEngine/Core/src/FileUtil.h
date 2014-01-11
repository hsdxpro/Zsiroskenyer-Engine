//FileUtil.h By Zsíroskenyér Team 2014.01.11 22:00
#pragma once

#include <fstream>
#include <list>
#include "zsString.h"

class cFileUtil {
public:
	static bool Clear(const zsString& path);

	static bool ReadBinary(const zsString& path, void* data, size_t dataSize);

	static bool WriteBinary(const zsString& path, void* data, size_t dataSize);

	static void DeleteFirstLines(size_t nLines);

	static bool Contains(std::wfstream& file, const zsString& str);
	static bool ReplaceAll(const zsString& repThat, const zsString& withThat);
	static bool RemoveDuplicatedLines();
	static bool isFileExits(const zsString& str);

	static zsString GetStringBefore(const zsString& str);
	static zsString GetWordAfter(const zsString& str);

	static std::list<zsString> GetLinesBetween(const zsString& str, const zsString& endLine);
	static std::list<zsString> GetLinesBeginsWith(const zsString& str);
	static std::list<zsString> GetLinesBeginsWithBetween(const zsString& str, const zsString& left, const zsString& right);

	static std::list<zsString> GetLines(std::wfstream& file);
	static size_t GetNLines();
	static size_t GetSize();
	static size_t GetSize(const zsString& path);
};