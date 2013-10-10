// zsString.h By Zsíroskenyér Team 2013.10.10 11:07
#pragma once

#define ZSSTRING_STACK_SIZE 256

#include "common.h"
#include <fstream>

class eiString {
public:
	eiString operator + (const eiString& str) const;
	eiString operator + (const WCHAR *str) const;

	eiString& operator += (const WCHAR *str);
	eiString& operator += (const WCHAR val);
	eiString& operator += (int val);
	eiString& operator += (float val);
	eiString& operator += (const eiString& str);

	eiString& operator = (const eiString& str);
	eiString& operator = (const WCHAR *str);

	void clear();
	void resize(uint32 newSize);

	void removeFromStart(uint16 numWCHARs);
	void addToStart(WCHAR ch);

	bool findStr(const WCHAR *str);
	bool strStr(const WCHAR *str, bool notCaseSensitive = false);

	void toAnsiChar(char *buffer, uint16 size) const;

	void *getLine(std::wifstream& inStream);
	const WCHAR *getDataAfterFirstWCHAR(const WCHAR ch) const;

	bool isEmpty();

	WCHAR *str();
	const WCHAR *strConst() const;

	// @TODO now only check the first WCHARacter
	bool operator < (const eiString& str) const;
	bool operator ==(const eiString& str) const;
	const WCHAR operator [] (uint32 idx) const;
	WCHAR& operator [] (uint32 idx);

	eiString(const eiString& str);
	eiString(const WCHAR *str);
	eiString(const char *str);
	eiString(WCHAR ch);
	eiString(int val);
	eiString(float val);
	eiString();
	~eiString();
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

// eiString + "asdasd"  works, but
// "asdasd" + eiString need to work too
eiString operator + (const WCHAR *cStr, const eiString& str);
eiString operator + (const CHAR *cStr, const eiString& str);