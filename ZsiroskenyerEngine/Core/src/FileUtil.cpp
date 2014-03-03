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

void cFileUtil::ReplaceIncludeDirectives(const zsString& filePath, std::list<zsString>& fileLines_out) {

	std::list<zsString> includedFileList;
	ReplaceIncludeDirectivesRecursively(filePath, fileLines_out, includedFileList);
}

void cFileUtil::ReplaceIncludeDirectivesRecursively(const zsString& filePath, std::list<zsString>& fileLines_out, std::list<zsString>& includedFileList_out) {
	auto fileLines = cFileUtil::GetLines(filePath);

	auto basePath = cStrUtil::GetDirectory(filePath);

	for each(zsString s in fileLines) {
		// Search for "#include" in that line
		int chIdx = cStrUtil::Find(s, L"#include");

		// reach line that has "#include"
		if (chIdx >= 0) {
			// Included file
			zsString filePath = cStrUtil::Between(s.c_str() + chIdx, '"', '"');

			auto it = std::find(includedFileList_out.begin(), includedFileList_out.end(), filePath);
			if (it == includedFileList_out.end()) {
				includedFileList_out.push_back(filePath);
				ReplaceIncludeDirectivesRecursively(basePath + filePath, fileLines_out, includedFileList_out);
			}
		}
		else {
			fileLines_out.push_back(s);
		}
	}
}

void cFileUtil::Write(std::ofstream& o, void* data, uint32_t size)
{
	o.write((const char*)data, size);
}

void cFileUtil::Read(std::ifstream& i, const void* dataOut, uint32_t size)
{
	i.read((char*)dataOut, size);
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

std::list<zsString> cFileUtil::GetLines(const zsString& filePath) {
	std::ifstream is(filePath.c_str());
	auto lines = cFileUtil::GetLines(is);
	is.close();
	return lines;
}

bool cFileUtil::isFileExits(const zsString& str) {
	std::wfstream is(str.c_str(), std::ios_base::in);
	bool isOpen = is.is_open();
	is.close();
	return isOpen;
}