#include "FileUtil.h"
#include "StrUtil.h"

// For determining size of a file
#include <sys/stat.h>
#include <windows.h>
#include "common.h"

bool cFileUtil::Clear(const zsString& path) {
	std::ofstream os(path.c_str(), std::ios::trunc);

	if (!os.is_open())
		return false;

	os.close();
	return true;
}


bool cFileUtil::ReadBinary(std::ifstream& file, void* dataOut, size_t dataSize) {
	if (!file.is_open())
		return false;

	file.read((char*)dataOut, dataSize);
	return true;
}

bool cFileUtil::WriteBinary(std::ofstream& file, void* data, size_t dataSize) {
	if (!file.is_open())
		return false;

	file.write((char*)data, dataSize);
	return true;
}


int cFileUtil::GetSize(const zsString& filePath) {
	char ansiPath[256];
	cStrUtil::ToAnsi(filePath, ansiPath, 256);

	struct stat results;
	if (stat(ansiPath, &results) == 0)
		return results.st_size;
	else
		return -1;
}

std::list<zsString> cFileUtil::GetLines(std::ifstream& file) {
	file.clear();
	file.seekg(0, std::ios::beg);

	std::list<zsString> lines;
	std::string str;
	while (getline(file, str))
		lines.push_back(str.c_str());
	return lines;
}

bool cFileUtil::isFileExits(const zsString& str) {
	std::wfstream is(str.c_str(), std::ios_base::in);
	bool isOpen = is.is_open();
	is.close();
	return isOpen;
}