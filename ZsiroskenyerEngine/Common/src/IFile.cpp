#include "IFile.h"
#include "../../CommonOS/src/File.h"


bool IFile::isFileExits(const zsString& str) {
	return cFile::isFileExits(str);
}

IFile* IFile::Create(const zsString& filePath) {
	return new cFile(filePath);
}
