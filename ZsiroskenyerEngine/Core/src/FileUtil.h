//FileUtil.h By Zsíroskenyér Team 2014.01.11 22:00
#pragma once

#include <fstream>
#include <list>
#include "zsString.h"

class cFileUtil {
public:
	static bool Clear(const zsString& path);

	static bool ReadBinary(std::fstream& file, void* data, size_t dataSize);

	static bool WriteBinary(std::fstream& file, void* data, size_t dataSize);

	static void DeleteFirstLines(std::wfstream& file, size_t nLines);

	static bool Contains(std::wfstream& file, const zsString& str);
	static bool ReplaceAll(std::wfstream& file, const zsString& repThat, const zsString& withThat);
	static bool RemoveDuplicatedLines(std::wfstream& file);
	static bool isFileExits(const zsString& str);

	static zsString GetStringBefore(std::wfstream& file, const zsString& str);
	static zsString GetWordAfter(std::wfstream& file, const zsString& str);

	static std::list<zsString> GetLinesBetween(std::wfstream& file, const zsString& str, const zsString& endLine);
	static std::list<zsString> GetLinesBeginsWith(std::wfstream& file, const zsString& str);
	static std::list<zsString> GetLinesBeginsWithBetween(std::wfstream& file, const zsString& str, const zsString& left, const zsString& right);

	static std::list<zsString> GetLines(std::wfstream& file);
	static size_t GetSize(const zsString& path);
	static size_t GetNLines(std::wfstream& file);
};