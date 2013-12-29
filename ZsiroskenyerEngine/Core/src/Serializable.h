//Serializable.h By Zsíroskenyér Team 2013.12.27 13:22
//Serialize data to binary, then you can read up..
#pragma once

#include "zsString.h"

#define SERIALIZABLE_DEFAULT_BYTE_GROWING 32

class cSerializable
{
public:
	// Writing data to file
	void WriteToFile(const zsString& str);

	// Reading data from file
	void ReadFromFile(const zsString& str);

	// Writing data
	cSerializable& operator << (size_t val);
	cSerializable& operator << (const zsString& str);

	// Reading data
	cSerializable& operator >> (size_t& valOut);
	cSerializable& operator >> (zsString& strOut);

	cSerializable(size_t startSize, size_t byteGrowing);
	cSerializable(size_t startSize);
	cSerializable();
	~cSerializable();

protected:
	void InternalGrowing(size_t newSize);

protected:
	unsigned char* data;
	size_t readIdx;
	size_t byteGrowing;
	size_t size;
	size_t capacity;
};