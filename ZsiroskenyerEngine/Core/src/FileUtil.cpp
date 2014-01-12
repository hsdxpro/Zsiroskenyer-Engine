#include "FileUtil.h"
#include "StrUtil.h"

// For determining size of a file
#include <sys/stat.h>
#include <windows.h>
#include "common.h"

bool cFileUtil::Clear(const zsString& path) {
	std::ofstream os(path.c_str(), std::ios::trunc);
	ASSERT(os.is_open() == true);
	if (!os.is_open())         {
		return false;
	}
	os.close();
	return true;
}

bool cFileUtil::ReadBinary(std::fstream& file, void* dataOut, size_t dataSize) {
	ASSERT(file.is_open() == true);

	if (!file.is_open())
		return false;

	file.read((char*)dataOut, dataSize);

	return true;
}

bool cFileUtil::WriteBinary(std::fstream& file, void* data, size_t dataSize) {
	ASSERT(file.is_open() == true);

	// Fail to open
	if (!file.is_open()) {
		return false;
	}

	file.write((char*)data, dataSize);
	file.close();
	return true;
}

void cFileUtil::DeleteFirstLines(std::wfstream& file, size_t nLines) {
	// Lines of file
	auto lines = cFileUtil::GetLines(file);

	// Move iteraton behind nLines
	auto iter = lines.begin();
	int idx = 0;
	while (idx != nLines) {
		iter++;
		idx++;
	}

	// Write lines after nLines
	while (iter != lines.end()) {
		file << *iter << L"\n";
		iter++;
	}
}

bool cFileUtil::Contains(std::wfstream& file, const zsString& str) {
	auto lines = cFileUtil::GetLines(file);
	auto iter = lines.begin();
	while (iter != lines.end()) {
		if (iter->find(str.c_str()) != std::wstring::npos)
			return true;
		iter++;
	}
	return false;
}

bool cFileUtil::ReplaceAll(std::wfstream& file, const zsString& repThat, const zsString& withThat) {
	auto lines = cFileUtil::GetLines(file);

	// Replace strings
	auto iter = lines.begin();
	bool foundReplace = false;
	while (iter != lines.end()) {
		size_t start_pos = iter->find(repThat);
		while (start_pos != std::string::npos) {
			iter->replace(start_pos, repThat.size(), withThat);
			start_pos = iter->find(repThat);
			foundReplace = true;
		}
		iter++;
	}

	// Write lines
	iter = lines.begin();
	while (iter != lines.end()) {
		file << *iter << L"\n";
		iter++;
	}

	return foundReplace;
}

size_t cFileUtil::GetNLines(std::wfstream& file) {
	return cFileUtil::GetLines(file).size();
}

size_t cFileUtil::GetSize(const zsString& filePath) {
	char ansiPath[256];
	cStrUtil::ConvertUniToAnsi(filePath, ansiPath, 256);

	//path.tozsString::
	struct stat results;
	if (stat(ansiPath, &results) == 0)
		return results.st_size;
	else
	{
		ASSERT(0);
		return 0;
	}
}

bool cFileUtil::RemoveDuplicatedLines(std::wfstream& file) {
	auto lines = cFileUtil::GetLines(file);

	// Lines that are duplicated
	std::list<zsString> duplicatedLines;

	// Uniq lines
	std::list<zsString> uniqLines;
	bool foundDuplicates = false;
	auto iterI = lines.begin();
	while (iterI != lines.end()) {
		// there is no duplication for that line (iterI)
		if (std::find(duplicatedLines.begin(), duplicatedLines.end(), *iterI) == duplicatedLines.end())
			uniqLines.push_back(*iterI);

		auto iterJ = lines.begin();
		while (iterJ != lines.end()) {
			if (iterJ != iterI) {
				if (*iterJ == *iterI) {
					duplicatedLines.push_back(*iterI); // remember duplicated strings
					foundDuplicates = true;
					break;
				}
			}
			iterJ++;
		}
		iterI++;
	}


	// Write to file
	iterI = uniqLines.begin();
	lines.clear();
	while (iterI != uniqLines.end()) {
		lines.push_back(*iterI);
		file << *iterI << L"\n";
		iterI++;
	}

	return foundDuplicates;
}

std::list<zsString> cFileUtil::GetLines(std::wfstream& file) {
	file.clear();
	file.seekg(0, std::ios::beg);

	std::list<zsString> lines;
	zsString str;
	while (getline(file, str))
		lines.push_back(str);
	return lines;
}

bool cFileUtil::isFileExits(const zsString& str) {
	std::wfstream is(str.c_str(), std::ios_base::in);
	bool isOpen = is.is_open();
	is.close();
	return isOpen;
}

zsString cFileUtil::GetStringBefore(std::wfstream& file, const zsString& str) {
	auto lines = cFileUtil::GetLines(file);
	auto iter = lines.begin();
	while (iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if (start_pos != std::wstring::npos) {
			return zsString(iter->substr(0, start_pos));
		}
		iter++;
	}
	return zsString();
}

zsString cFileUtil::GetWordAfter(std::wfstream& file, const zsString& str) {
	auto lines = cFileUtil::GetLines(file);
	size_t idx = 0;
	auto iter = lines.begin();
	while (iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if (start_pos != std::wstring::npos) {
			start_pos += str.size();
			idx = start_pos;
			while ((*iter)[idx] != ' ') {
				idx++;
			}
			return zsString(iter->substr(start_pos, idx - start_pos));
		}
		iter++;
	}
	return zsString();
}

std::list<zsString> cFileUtil::GetLinesBetween(std::wfstream& file, const zsString& str, const zsString& endLine) {
	auto lines = cFileUtil::GetLines(file);
	std::list<zsString> result;

	auto iter = lines.begin();
	while (iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if (start_pos != std::wstring::npos) {
			iter++;
			while (*iter != endLine) {
				result.push_back(*iter);
				iter++;
			}
			break;
		}
		iter++;
	}
	return result;
}

std::list<zsString> cFileUtil::GetLinesBeginsWith(std::wfstream& file, const zsString& str) {
	auto lines = cFileUtil::GetLines(file);
	std::list<zsString> result;

	auto iter = lines.begin();
	bool match = true;
	while (iter != lines.end()) {
		match = true;
		wchar_t const* tmp1 = iter->c_str();
		wchar_t const* tmp2 = str.c_str();

		if (*tmp1 == '\0')
			match = false;

		while (*tmp2 != '\0')
		{
			if (*tmp1 != *tmp2)
			{
				match = false;
				break;
			}

			tmp1++;
			tmp2++;
		}

		if (match)
			result.push_back(*iter);

		iter++;
	}
	return result;
}

std::list<zsString> cFileUtil::GetLinesBeginsWithBetween(std::wfstream& file, const zsString& str, const zsString& left, const zsString& right) {
	std::list<zsString> res = GetLinesBeginsWith(file, str);
	for (auto i = res.begin(); i != res.end(); i++)
		cStrUtil::Between(*i, left.c_str(), right.c_str());
	return res;
}