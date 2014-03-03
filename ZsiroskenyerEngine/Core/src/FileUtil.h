//FileUtil.h By Zsíroskenyér Team 2014.01.11 22:00
#pragma once

#include <fstream>
#include <list>
#include "zsString.h"

class cFileUtil {
public:
	static bool Clear(const zsString& path);

	static void ReplaceIncludeDirectives(const zsString& filePath, std::list<zsString>& fileLines_out);

// Binary writes
	static void Write(std::ofstream& o, void* data, size_t size);

	template<class T>
	static void Write(std::ofstream& o, const T& data);

	template<>
	static void Write<const wchar_t*>(std::ofstream& o, const wchar_t* const& data);

// Binary reads
	static void Read(std::ifstream& i, const void* dataOut, uint32_t size);

	template<class T>
	static void Read(std::ifstream& i, T& out);

// Conditional things
	static bool isFileExits(const zsString& str);

// Get infos
	static std::list<zsString> GetLines(std::ifstream& file);
	static std::list<zsString> GetLines(const zsString& filePath);
	static int GetSize(const zsString& path);
};

template<class T>
static void cFileUtil::Write(std::ofstream& o, const T& data)
{
	o.write((const char*)&data, sizeof(T));
}

template<>
static void cFileUtil::Write<const wchar_t*>(std::ofstream& o, const wchar_t* const& data)
{
	o.write((const char*)data, wcslen(data) * sizeof(wchar_t));
}

template<class T>
static void cFileUtil::Read(std::ifstream& i, T& out)
{
	i.read((char*)&out, sizeof(T));
}
