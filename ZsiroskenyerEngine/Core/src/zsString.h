// zsString.h By Zsíroskenyér Team 2013.10.10 11:07
#pragma once
#pragma warning(disable: 4996)

#include <vector>
#include <sstream>

#ifdef STATIC_SIZE 
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
	void Resize(size_t newSize);

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
	const WCHAR operator [] (size_t idx) const;
	WCHAR& operator [] (size_t idx);

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


#else // STATIC_SIZE

#include "memory/tlsf.h"
#include <string>
#include <cstdint>
#include <stdexcept>
#include <functional>

#undef max

// allocator class
template <class T>
class TLSFAllocator {
public:
	// types
	typedef T value_type;
	typedef T* pointer;
	// construction
	TLSFAllocator() {};
	TLSFAllocator(const TLSFAllocator& other) {}
	TLSFAllocator(TLSFAllocator&& other) {}
	template <class U>
	TLSFAllocator(const TLSFAllocator<U>& other) {}
	template <class U>
	TLSFAllocator(TLSFAllocator<U>&& other) {}
	// allocate
	pointer allocate(size_t n);
	void deallocate(pointer ptr, size_t n);
	// equality
	bool operator==(const TLSFAllocator& other);
	bool operator!=(const TLSFAllocator& other);
	// rebind
	template<class U>
	struct rebind {
		typedef TLSFAllocator<U> other;
	};
	// other
	size_t max_size();
	inline void construct(pointer p, const T& t) { new(p) T(t); }
    inline void destroy(pointer p) { p->~T(); }

private:
	// internal pool
	static TLSF memPool;
};

// equality
template <class T>
bool TLSFAllocator<T>::operator==(const TLSFAllocator& other) {
		return &memPool == &other.memPool;
}
template <class T>
bool TLSFAllocator<T>::operator!=(const TLSFAllocator& other) {
	return !(*this==other);
}
// alloc
template <class T>
typename TLSFAllocator<T>::pointer TLSFAllocator<T>::allocate(size_t n) {
	pointer p = (pointer)memPool.allocate(sizeof(value_type)*n);
	if (p==nullptr)
		throw std::runtime_error("TLSF failure");
	return p;
}
template <class T>
void TLSFAllocator<T>::deallocate(pointer ptr, size_t n) {
	memPool.deallocate((void*)ptr);
}

template <class T>
size_t TLSFAllocator<T>::max_size() {
	return std::numeric_limits<size_type>::max();
}

template <class T>
TLSF TLSFAllocator<T>::memPool(4096, 5);

// zsString type
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, TLSFAllocator<wchar_t>> zsBasicString;


class zsString : public zsBasicString {
public:
	static void ConvertUniToAnsi(const zsString& src, char* dst, size_t nChars) {
		wcstombs(dst, src.c_str(), nChars);
	}
	
	void ConvertUniToAnsi(char* dst, size_t nChars) const {
		wcstombs(dst, c_str(), nChars);
	}

	void CutNumberFromEnd(char* src) {
		// Move to end
		while (*src != '\0')
			src++;

		// terminate in numbers (cutting)
		while (isdigit(*--src))
			*src = '\0';
	}

	// Gather string between left and right characters, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
	void Between(wchar_t left, wchar_t right) {
		wchar_t const* str = c_str();
		size_t leftIdx = 0;

		// Reach left bound
		while (str[leftIdx] != left || str[leftIdx] == '\0')
			leftIdx++;

		// Reach right bound
		size_t rightIdx = leftIdx;
		while (str[rightIdx] != right || str[rightIdx] == '\0')
			rightIdx++;

		*this = substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
	}

	void Between(wchar_t left, const wchar_t* rightDelims, size_t nRightDelims) {
		wchar_t const* str = c_str();
		size_t leftIdx = 0;

		// Reach left bound
		while (str[leftIdx] != left || str[leftIdx] == '\0')
			leftIdx++;

		// Reach right bound
		size_t rightIdx = leftIdx;
		size_t i = 0;
		while (str[rightIdx] != '\0')
		{
			for ( i = 0; i < nRightDelims; i++)
			if (str[rightIdx] == rightDelims[i])
				break;

			if (i != nRightDelims)
				break;

			rightIdx++;
		}

		*this = substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
	}

	void CutDirectory() {
		const wchar_t *str = c_str();
		size_t nDelimIdx = 0;
		size_t idx = 0;
		while (str[idx] != '\0') {
			idx++;

			if (str[idx] == '\\' || str[idx] == '/')
				nDelimIdx = idx;
		}
		nDelimIdx++; // go after delim..

		// Okay so we found the index of the last '\\' or '/'
		size_t newSize = size() - nDelimIdx; // TODO size() slow solution

		*this = substr(nDelimIdx, newSize);
	}
	
	void CutFrontFromDelim(wchar_t ch) {
		const wchar_t *str = c_str();
		size_t idx = 0;
		while (str[idx] != '\0' && str[idx++] != ch);
		*this = substr(idx, size() - idx);
	}

	void GetNumberFromEnd(char* src, char* buf_out) {
		// Move to end
		while (*src != '\0')
			src++;

		while (isdigit(*--src))
			*buf_out++ = *src;
		*buf_out = '\0';
	}

	void GetWordBetween(char left, char right, char* buf_out) {
		wchar_t const* str = c_str();

		// Leave spaces from left and try reach left bound
		while ((char)*str == ' ' || (char)*str != left)
			str++;
		str++;

		// TODO WRITE THE FUCKING ASSERT SOMEHOW

		// Leave spaces
		while ((char)*str == ' ')
			str++;

		// Copy characters while not end of string or right bound reached
		while ((char)*str != right && (char)*str != '\0')
			*buf_out++ = (char)*str++;

		// Terminate string
		*buf_out = '\0';
	}

	void GetFloats(std::vector<float>& floats_out) const {
		// Clear output if it's not clear
		if(floats_out.size() != 0) 
			floats_out.clear();

		wchar_t const* str = c_str();
		while (!isdigit(*str))
			str++;

		size_t offset = 0;
		wchar_t* end = NULL;
		do {
			floats_out.push_back((float)wcstod(str + offset, &end));
			offset = end - str;
		} while (*end != '\0' && end != NULL);
	}

	zsString GetDirectory() const {
		const wchar_t* str = c_str();
		if (c_str()[0] == '\0')
			return zsString();

		
		// Copy whole string
		zsString directory = c_str();
		
		// Null terminate from right to left, while not reaching directory
		size_t i = directory.size() - 1;
		for ( ; directory[i] != '\\' && directory[i] != '/' && i >= 0; i--);

		directory.resize(i + 1);

		return directory;
	}

	// constructors
	zsString() : zsBasicString() {};
	zsString(size_type s, value_type c) : zsBasicString(s,c) {};
	zsString(const zsBasicString& other, size_type pos, size_type count=npos) : zsBasicString(other, pos, count) {}
	zsString(const value_type* s, size_type count) : zsBasicString(s, count) {}
	zsString(const value_type* s) : zsBasicString(s) {}

	template <class InputIt>
	zsString(InputIt first, InputIt last) : zsBasicString(first, last) {}

	zsString(const zsBasicString& other) : zsBasicString(other) {}
	zsString(zsBasicString&& other) : zsBasicString(other) {}
		// zsString(std::initializer_list<valut_type> init) : zsBasicString(init) {}

	// Construct from numbers
	template<class ValType>
	zsString(ValType val) {
		std::wstringstream ss;
		ss << val;
		auto str = ss.str();
		for (size_t i = 0; i < str.length(); i++)
			zsBasicString::operator += (str[i]);
	}

	// operators
	zsString& operator = (const zsBasicString& str) { zsBasicString::operator=(str); return *this; }
	zsString& operator = (zsBasicString&& str)		{ zsBasicString::operator=(str); return *this; }
	zsString& operator = (const value_type* s)		{ zsBasicString::operator=(s);	 return *this; }
	zsString& operator = (value_type ch)			{ zsBasicString::operator=(ch);  return *this; }

	zsString& operator += (const zsBasicString& str)	{ zsBasicString::operator+=(str); return *this; }
	zsString& operator += (value_type ch)				{ zsBasicString::operator+=(ch);  return *this; }
	zsString& operator += (value_type* s)				{ zsBasicString::operator+=(s);	  return *this; }

	//template <class ValType>
	zsString operator + (size_t val) { zsString str; str += zsString(val); return str; }
};

template <>
struct std::hash<zsString> {
	size_t operator()(const zsString& str) const {
		return std::hash<zsBasicString>()(str);
	}
};

#endif // STATIC_SIZE
