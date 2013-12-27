#include "Serializable.h"
#include "IFile.h"
#include "common.h"

cSerializable::cSerializable(size_t startSize, size_t byteGrowing)
:capacity(startSize), size(0), byteGrowing(byteGrowing) {
	data = (unsigned char*)malloc(startSize);
}

cSerializable::cSerializable(size_t startSize)
: capacity(startSize), size(0), byteGrowing(SERIALIZABLE_DEFAULT_BYTE_GROWING) {
	data = (unsigned char*)malloc(startSize);
}

cSerializable::cSerializable()
: data(NULL), capacity(0), size(0), byteGrowing(SERIALIZABLE_DEFAULT_BYTE_GROWING) {
}

cSerializable::~cSerializable() {
	free(data);
}

void cSerializable::WriteToFile(const zsString& str) {
	IFile* f = IFile::Create(str, eFileOpenMode::BINWRITE);
		f->WriteBinary(&size, sizeof(size_t));
		f->WriteBinary(&data, size);
	SAFE_RELEASE(f);
}
void cSerializable::ReadFromFile(const zsString& str) {
	IFile* f = IFile::Create(str, eFileOpenMode::BINREAD);
	size_t dataSize;
		f->ReadBinary(&dataSize, sizeof(size_t));
			InternalGrowing(-(int)size + dataSize);
		f->ReadBinary(&data, dataSize);
	SAFE_RELEASE(f);
}

cSerializable& cSerializable::operator << (size_t val) {
	InternalGrowing(sizeof(size_t));
	memcpy(&data[size - sizeof(size_t)], &val, sizeof(size_t));
	return *this;
}

cSerializable& cSerializable::operator << (const zsString& str) { 
	size_t dataSize = str.size() + 1;
	InternalGrowing(dataSize);
	memcpy(&data[size - dataSize], str.c_str(), dataSize);
	return *this; 
}

void cSerializable::InternalGrowing(size_t sizeToIncreaseWith) {
	if (capacity < size + sizeToIncreaseWith) {
		capacity = sizeToIncreaseWith < byteGrowing ? size + byteGrowing : size + sizeToIncreaseWith;
		size += sizeToIncreaseWith;
		data = (unsigned char*) realloc(data, capacity);
	}
}