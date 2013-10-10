// zsString.h By Zsíroskenyér Team 2013.10.10 11:07
#pragma once

#define ZSSTRING_STACK_SIZE 256

#include "common.h"
#include <fstream>

class zsString {
public:
	zsString operator + (const zsString& str) const;
	zsString operator + (const WCHAR *str) const;

	zsString& operator += (const WCHAR *str);
	zsString& operator += (const WCHAR val);
	zsString& operator += (int val);
	zsString& operator += (float val);
	zsString& operator += (const zsString& str);

	zsString& operator = (const zsString& str);
	zsString& operator = (const WCHAR *str);

	void Clear();
	void Resize(uint32 newSize);

	void RemoveFromStart(uint16 numWCHARs);
	void AddToStart(WCHAR ch);

	bool FindStr(const WCHAR *str);
	bool StrStr(const WCHAR *str, bool notCaseSensitive = false);

	void ToAnsiChar(char *buffer, uint16 size) const;

	void *GetLine(std::wifstream& inStream);
	const WCHAR *GetDataAfterFirstChar(const WCHAR ch) const;

	bool IsEmpty();

	WCHAR *Str();
	const WCHAR *StrConst() const;

	// @TODO now only check the first WCHARacter
	bool operator < (const zsString& str) const;
	bool operator ==(const zsString& str) const;
	const WCHAR operator [] (uint32 idx) const;
	WCHAR& operator [] (uint32 idx);

	zsString(const zsString& str);
	zsString(const WCHAR *str);
	zsString(const char *str);
	zsString(WCHAR ch);
	zsString(int val);
	zsString(float val);
	zsString();
	~zsString();
protected:
	// Memory from stack instead of the heap to improve performance
	WCHAR staticData[ZSSTRING_STACK_SIZE];

	// If we run out of staticData (stack), we allocate from heap
	WCHAR *dynamicData;

	// Point to the start of staticData all the time
	WCHAR *firstCharPtr;

	// Point to the end of staticData all the time
	WCHAR *lastCharPtr;
};

// zsString + "asdasd"  works, but
// "asdasd" + zsString need to work too
zsString operator + (const WCHAR *cStr, const zsString& str);
zsString operator + (const CHAR *cStr, const zsString& str);