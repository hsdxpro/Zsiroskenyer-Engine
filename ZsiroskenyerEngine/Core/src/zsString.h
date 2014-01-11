// zsString.h By Zsíroskenyér Team 2013.10.10 11:07
#pragma once
#pragma warning(disable: 4996)

#include <sstream>
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
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>/*, TLSFAllocator<wchar_t>*/> zsBasicString;


class zsString : public zsBasicString {
public:
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