

#ifdef STATIC_SIZE

// Implementation
#include "zsString.h"
#include <sstream>

zsString::zsString()
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
}

zsString::zsString(WCHAR ch)
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		staticData[0] = ch;
		staticData[1] = '\0';
}

zsString::zsString(int val)

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

zsString::zsString(float val)
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

zsString::zsString(const WCHAR *str) 
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = str[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		}
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

zsString::zsString(const char *str) 
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		mbstowcs(firstCharPtr, str, ZSSTRING_STACK_SIZE);
}

zsString::zsString(const zsString& str)
	:firstCharPtr(staticData), lastCharPtr(staticData), dynamicData(NULL) {
		uint32 idx = 0;
		WCHAR ch = '\0';
		while((ch = str[idx]) != '\0') {
			firstCharPtr[idx] = ch;
			idx++;
		} 
		lastCharPtr = &(firstCharPtr[idx] = ch);
}

zsString::~zsString() {
	firstCharPtr = NULL;
	lastCharPtr = NULL;
	SAFE_DELETE(dynamicData);
}

zsString& zsString::operator += (const zsString& str) {
	return (*this += str.firstCharPtr);
}

zsString& zsString::operator += (const WCHAR *str) {
	uint32 idx = 0;
	WCHAR ch = '\0';
	while((ch = str[idx]) != '\0') {
		lastCharPtr[idx] = ch;
		idx++;
	}
	lastCharPtr = &(lastCharPtr[idx] = ch);
	return *this;
}

zsString& zsString::operator += (int val) {
	*this += zsString(val);
	return *this;
}

zsString& zsString::operator += (const WCHAR val) {
	*lastCharPtr = val;
	lastCharPtr++; 
	*lastCharPtr = '\0';
	return *this;
}

zsString& zsString::operator += (float val) {
	*this += zsString(val);
	return *this;
}

bool zsString::operator < (const zsString& str) const {
	return wcscmp(firstCharPtr,str.firstCharPtr) < 0;
}

bool zsString::operator ==(const zsString& str) const {
	return wcscmp(firstCharPtr, str.firstCharPtr) == 0;
}

void *zsString::GetLine(std::wifstream& inStream) {
	return inStream.getline(firstCharPtr, ZSSTRING_STACK_SIZE);
}

const WCHAR *zsString::GetDataAfterFirstChar(const WCHAR ch) const {
	uint32 idx = 0;
	while(firstCharPtr[idx] != ch)
		idx++;
	return &firstCharPtr[idx + 1];
}

const WCHAR zsString::operator [] (uint32 idx) const {
	if(idx < ZSSTRING_STACK_SIZE) {
		return firstCharPtr[idx];
	} else {
		return dynamicData[idx];
	}
}

WCHAR& zsString::operator [] (uint32 idx) {
	if(idx < ZSSTRING_STACK_SIZE) {
		return firstCharPtr[idx];
	} else {
		return dynamicData[idx];
	}
}

bool zsString::IsEmpty() {
	return firstCharPtr[0] == '\0';
}

WCHAR *zsString::Str() {
	return firstCharPtr;
}

const WCHAR *zsString::StrConst() const {
	return firstCharPtr;
}

zsString zsString::operator + (const zsString& str) const{
	zsString result = *this;
	result += str;
	return result;
}

zsString zsString::operator + (const WCHAR *str) const {
	zsString result = *this;
	result += str;
	return result;
}

void zsString::RemoveFromStart(uint16 numWCHARs) {
	firstCharPtr++;
}

void zsString::AddToStart(WCHAR ch) {
	memmove(&firstCharPtr[1], &firstCharPtr[0], sizeof(WCHAR));
	*firstCharPtr = ch;
	*(++lastCharPtr) = '\0';
}

bool zsString::FindStr(const WCHAR* str) {
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

bool zsString::StrStr(const WCHAR *str, bool notCaseSensitive) {
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

void zsString::ToAnsiChar(char *buffer, uint16 size) const {
	wcstombs(buffer, firstCharPtr, size);
}


zsString& zsString::operator = (const zsString& str) {
	*this = str.firstCharPtr;
	return *this;
}

zsString& zsString::operator = (const WCHAR *str) {
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

void zsString::Resize(uint32 newSize) {
	if(newSize > ZSSTRING_STACK_SIZE) {
		dynamicData = (WCHAR*)realloc(dynamicData, newSize - ZSSTRING_STACK_SIZE);
	}
}

void zsString::Clear() {
	firstCharPtr = lastCharPtr = &(staticData[0] = '\0');
}

zsString operator + (const WCHAR *cStr, const zsString& str) {
	return zsString(cStr) + str;
}

zsString operator + (const CHAR *cStr, const zsString& str) {
	return zsString(cStr) + str;
}




#else // STATIC_SIZE

#include "zsString.h"


#endif // STATIC_SIZE