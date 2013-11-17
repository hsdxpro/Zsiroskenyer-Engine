#include "IFile.h"
#include "../../CommonOS/src/File.h"


bool IFile::isFileExits(const zsString& str) {
	return cFile::isFileExits(str);
}

IFile* IFile::Create(const zsString& filePath) {
	return new cFile(filePath);
}

void IFile::ReadBinary(const zsString path, void** data_out, size_t& dataSize_out) {
	cFile::ReadBinary(path, data_out, dataSize_out);
}