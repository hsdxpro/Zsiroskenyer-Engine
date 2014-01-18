//StrUtil.h By Zsíroskenyér Team 2014.01.11 20:19
#pragma once

#include "zsString.h"
#include <vector>
#include <list>
#include <fstream>

class cStrUtil {
public:
	static void ToAnsi(const zsString& src, char* dst, size_t nChars);
	static size_t ToUnsigned(const zsString& str);

	static bool Contains(const zsString& in, const zsString& that);
	static bool Contains(const std::list<zsString>& strs, const zsString& str);

	// returns - 1 : string not found
	// return >= 0 : index of the founded str in zsString
	static int Find(const zsString& in, const zsString& that);

	// returns - 1 : string not found
	// return >= 0 : index of the founded str in zsString
	static int Find(const zsString& str, wchar_t ch);

	static bool Begins(const zsString& str, const zsString& withThat);

	static void CutNumberFromEnd(char* src);
	static void CutDirectory(zsString& strOut);
	static void CutFrontFromDelim(zsString& strOut, wchar_t ch);

	// Gather string between left and right characters, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
	static void Between(zsString& strOut, wchar_t left, wchar_t right);

	// Gather string between left and right strings, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
	static void Between(zsString& strOut, const wchar_t* left, const wchar_t* right);

	static void Between(zsString& strOut, wchar_t left, const wchar_t* rightDelims, size_t nRightDelims);
	static zsString SubStrLeft(const zsString& str, size_t pos, wchar_t leftBound, size_t leftCutOffset = 0);
	static zsString SubStrRight(const zsString& str, size_t pos, wchar_t rightBound, size_t rightCutOffset = 0);

	static zsString TrimSpaceBounds(const zsString& str);

	// Gather string between left and right characters, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
	static zsString Between(const zsString& s, wchar_t left, wchar_t right);  
	static zsString Between(const zsString& s, const wchar_t* left, const wchar_t* right);
	static zsString Between(const zsString& s, wchar_t left, const wchar_t* rightDelims, size_t nRightDelims);

	static std::list<size_t> GetLines(const std::list<zsString>& strs, size_t startLineIdx, const zsString& containStr);
	static void GetNumberFromEnd(char* src, char* buf_out);
	static void GetWordBetween(zsString& str, char left, char right, char* buf_out);
	static void GetFloats(const zsString& str, std::vector<float>& floats_out);
	static zsString GetDirectory(const zsString& str);
	static zsString GetWordAfter(const std::list<zsString>& strs, const zsString& str);
	static std::list<zsString> GetLinesBetween(const std::list<zsString>& strs, const zsString& str, const zsString& endLine);
	static std::list<zsString> GetLinesBeginsWith(const std::list<zsString>& strs, const zsString& str);
	static std::list<size_t> GetLinesContainingAllStr(const std::list<zsString>& in, const std::list<zsString>& those);
};