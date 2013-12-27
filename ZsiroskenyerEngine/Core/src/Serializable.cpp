#include "Serializable.h"

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

cSerializable& cSerializable::operator << (size_t val) {
	InternalGrowing(sizeof(size_t));
	memcpy(&data[size], &val, sizeof(size_t));
	size += sizeof(size_t);
	return *this;
}

cSerializable& cSerializable::operator << (const zsString& str) { 
	size_t dataSize = str.size() + 1;
	InternalGrowing(dataSize);
	memcpy(&data[size], str.c_str(), dataSize);
	size += dataSize;
	return *this; 
}

void cSerializable::InternalGrowing(size_t sizeToIncreaseWith) {
	if (capacity < size + sizeToIncreaseWith) {
		capacity = sizeToIncreaseWith < byteGrowing ? byteGrowing : sizeToIncreaseWith;
		data = (unsigned char*) realloc(data, capacity);
	}
}