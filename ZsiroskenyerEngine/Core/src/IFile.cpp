#include "IFile.h"
#include "../../CommonOS/src/File.h"


bool IFile::isFileExits(const zsString& str) {
	return cFile::isFileExits(str);
}

IFile* IFile::Create(const zsString& filePath) {
	return new cFile(filePath);
}

bool IFile::ReadBinary(const zsString& path, void* data_out, size_t dataSize) {
	return cFile::ReadBinary(path, data_out, dataSize);
}

bool IFile::WriteBinary(const zsString& path, void* data, size_t dataSize) {
	return cFile::WriteBinary(path, data, dataSize);
}

bool IFile::Clear(const zsString& path) {
	return cFile::Clear(path);
}

size_t IFile::GetSize(const zsString& path) {
	return cFile::GetSize(path);
}
 