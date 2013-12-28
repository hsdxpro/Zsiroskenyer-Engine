#include "Serializable.h"
#include "IFile.h"
#include "common.h"

cSerializable::cSerializable(size_t startSize, size_t byteGrowing)
:capacity(startSize), readIdx(0), size(0), byteGrowing(byteGrowing) {
	data = (unsigned char*)malloc(startSize);
}

cSerializable::cSerializable(size_t startSize)
: capacity(startSize), readIdx(0), size(0), byteGrowing(SERIALIZABLE_DEFAULT_BYTE_GROWING) {
	data = (unsigned char*)malloc(startSize);
}

cSerializable::cSerializable()
: data(NULL), capacity(0), readIdx(0), size(0), byteGrowing(SERIALIZABLE_DEFAULT_BYTE_GROWING) {
}

cSerializable::~cSerializable() {
	free(data);
}

void cSerializable::WriteToFile(const zsString& str) {
	IFile* f = IFile::Create(str, eFileOpenMode::BINWRITE);
		f->WriteBinary(&size, sizeof(size_t));
		f->WriteBinary(data, size);
	SAFE_RELEASE(f);
}
void cSerializable::ReadFromFile(const zsString& str) {
	IFile* f = IFile::Create(str, eFileOpenMode::BINREAD);
	size_t dataSize;
		f->ReadBinary(&dataSize, sizeof(size_t));
			InternalGrowing(dataSize + sizeof(size_t));
		f->ReadBinary(data, dataSize + sizeof(size_t));
	SAFE_RELEASE(f);
	readIdx += sizeof(size_t);
}

cSerializable& cSerializable::operator << (size_t val) {
	InternalGrowing(size + sizeof(size_t));
	memcpy(&data[size - sizeof(size_t)], &val, sizeof(size_t));
	return *this;
}
cSerializable& cSerializable::operator << (const zsString& str) { 
	size_t dataSize = str.size() + 1;
	InternalGrowing(size + dataSize);
	memcpy(&data[size - dataSize], str.c_str(), dataSize);
	return *this; 
}


cSerializable& cSerializable::operator >> (size_t& valOut) {
	valOut = *(size_t*)(data + readIdx);
	readIdx += sizeof(size_t);
	return *this;
}
cSerializable& cSerializable::operator >> (zsString& strOut) {
	strOut = zsString(data + readIdx);
	readIdx += strOut.size() + 1;
	return *this;
}

void cSerializable::InternalGrowing(size_t newSize) {
	// Capacity is to small to store data
	size = newSize;
	if (capacity < newSize) {
		//size_t fit = (((newSize % byteGrowing) != 0) ? byteGrowing - (newSize % byteGrowing) : 0);
		capacity = newSize + (((newSize % byteGrowing) != 0) ? byteGrowing - (newSize % byteGrowing) : 0); // newSize is multiple of byteGrowing, ceil
		data = (unsigned char*) realloc(data, capacity);
	}
}