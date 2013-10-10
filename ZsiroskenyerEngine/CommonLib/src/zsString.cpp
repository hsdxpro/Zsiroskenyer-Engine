// Implementation
#include "zsString.h"
#include <sstream>

eiString::eiString()
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
}

eiString::eiString(WCHAR ch)
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		staticData[0] = ch;
		staticData[1] = '\0';
}

eiString::eiString(int val)

	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		std::wstringstream ss; ss<<val;
		std::wstring& str = ss.str();
		const WCHAR *dataToCopy = str.c_str();
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = dataToCopy[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		}
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

eiString::eiString(float val)
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		std::wstringstream ss; ss<<val;
		std::wstring& str = ss.str();
		const WCHAR *dataToCopy = str.c_str();
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = dataToCopy[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		}
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

eiString::eiString(const WCHAR *str) 
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = str[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		}
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

eiString::eiString(const char *str) 
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		mbstowcs(firstCharPtr, str, ZSSTRING_STACK_SIZE);
}

eiString::eiString(const eiString& str)
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = str[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		} 
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

eiString::~eiString() {
	firstCharPtr = NULL;
	lastCharPtr = NULL;
	SAFE_DELETE(dynamicData);
}

eiString& eiString::operator += (const eiString& str) {
	return (*this += str.firstCharPtr);
}

eiString& eiString::operator += (const WCHAR *str) {
	uint32 idx = 0;
	WCHAR ch = '\0';
	while((ch = str[idx]) != '\0') {
		lastCharPtr[idx] = ch;
		idx++;
	}
	lastCharPtr = &(lastCharPtr[idx] = ch);
	return *this;
}

eiString& eiString::operator += (int val) {
	*this += eiString(val);
	return *this;
}

eiString& eiString::operator += (const WCHAR val) {
	*lastCharPtr = val;
	lastCharPtr++; 
	*lastCharPtr = '\0';
	return *this;
}

eiString& eiString::operator += (float val) {
	*this += eiString(val);
	return *this;
}

bool eiString::operator < (const eiString& str) const {
	return wcscmp(firstCharPtr,str.firstCharPtr) < 0;
}

bool eiString::operator ==(const eiString& str) const {
	return wcscmp(firstCharPtr, str.firstCharPtr) == 0;
}

void *eiString::getLine(std::wifstream& inStream) {
	return inStream.getline(firstCharPtr, ZSSTRING_STACK_SIZE);
}

const WCHAR *eiString::getDataAfterFirstWCHAR(const WCHAR ch) const {
	uint32 idx = 0;
	while(firstCharPtr[idx] != ch)
		idx++;
	return &firstCharPtr[idx + 1];
}

const WCHAR eiString::operator [] (uint32 idx) const {
	if(idx < ZSSTRING_STACK_SIZE) {
		return firstCharPtr[idx];
	} else {
		return dynamicData[idx];
	}
}

WCHAR& eiString::operator [] (uint32 idx) {
	if(idx < ZSSTRING_STACK_SIZE) {
		return firstCharPtr[idx];
	} else {
		return dynamicData[idx];
	}
}

bool eiString::isEmpty() {
	return firstCharPtr[0] == '\0';
}

WCHAR *eiString::str() {
	return firstCharPtr;
}

const WCHAR *eiString::strConst() const {
	return firstCharPtr;
}

eiString eiString::operator + (const eiString& str) const{
	eiString result = *this;
	result += str;
	return result;
}

eiString eiString::operator + (const WCHAR *str) const {
	eiString result = *this;
	result += str;
	return result;
}

void eiString::removeFromStart(uint16 numWCHARs) {
	firstCharPtr++;
}

void eiString::addToStart(WCHAR ch) {
	memmove(&firstCharPtr[1], &firstCharPtr[0], sizeof(WCHAR));
	*firstCharPtr = ch;
	*(++lastCharPtr) = '\0';
}

bool eiString::findStr(const WCHAR* str) {
	uint32 mainIndex = 0;
	WCHAR ch = '\0';
	while((ch = firstCharPtr[mainIndex]) != '\0') {
		if(str[0] == ch) {
			bool equal = true;
			uint32 secIndex = 0;
			uint32 fIndex = mainIndex;
			while(str[secIndex] != '\0') {
				ch = firstCharPtr[fIndex];
				if(str[secIndex] != ch) {
					equal = false;
					break;
				}
				fIndex++;
				secIndex++;
			}
			if(equal) {
				return true;
			}
		}
		mainIndex++;
	}
	return false;
}

bool eiString::strStr(const WCHAR *str, bool notCaseSensitive) {
	uint32 mainIdx = 0;
	uint32 str1Idx = 0;
	uint32 str2Idx = 0;
	while(firstCharPtr[mainIdx] != '\0') {
		str1Idx = mainIdx;
		str2Idx = 0;
		if(notCaseSensitive) {
			while((WCHAR)tolower((WCHAR)str[str2Idx]) != '\0' && (WCHAR)tolower((WCHAR)firstCharPtr[str1Idx]) == (WCHAR)tolower((WCHAR)str[str2Idx])) {
				str1Idx++;
				str2Idx++;
			}
		} else {
			while(str[str2Idx] != '\0' && firstCharPtr[str1Idx] == str[str2Idx]) {
				str1Idx++;
				str2Idx++;
			}
		}

		if(str[str2Idx] == '\0') {
			return true;
		}
		mainIdx++;
	}
	return false;
}

void eiString::toAnsiChar(char *buffer, uint16 size) const {
	wcstombs(buffer, firstCharPtr, size);
}


eiString& eiString::operator = (const eiString& str) {
	*this = str.firstCharPtr;
	return *this;
}

eiString& eiString::operator = (const WCHAR *str) {
	uint32 idx = 0;
	WCHAR ch = '\0';
	firstCharPtr = staticData;
	while( (ch = str[idx]) != '\0') {
		firstCharPtr[idx] = ch;
		idx++;
	}
	lastCharPtr = &(firstCharPtr[idx] = ch);
	return *this;
}

void eiString::resize(uint32 newSize) {
	if(newSize > ZSSTRING_STACK_SIZE) {
		dynamicData = (WCHAR*)realloc(dynamicData, newSize - ZSSTRING_STACK_SIZE);
	}
}

void eiString::clear() {
	firstCharPtr = lastCharPtr = &(staticData[0] = '\0');
}

eiString operator + (const WCHAR *cStr, const eiString& str) {
	return eiString(cStr) + str;
}

eiString operator + (const CHAR *cStr, const eiString& str) {
	return eiString(cStr) + str;
}