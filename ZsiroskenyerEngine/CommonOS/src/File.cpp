#include "File.h"
#include "../../Core/src/common.h"
#include "../../Core/src/Exception.h"
#include "../../Core/src/ILog.h"

// For determining size of a file
#include <sys/stat.h>
#include <windows.h>

cFile::cFile()
:isEof(false) {
}

cFile::cFile(const zsString& filePath, eFileOpenMode m)
:isEof(false), filePath(filePath) {
	lines.clear();

	// Open stream
	stream.open(filePath.c_str(), ConvertToNativeOpenMode(m));
	if(!stream.is_open()) {
		throw FileNotFoundException();
		ILog::GetInstance()->MsgBox(L"Can't open file: " + filePath);
	} else {
		// read up lines
		zsString lineStr;
		while(getline(stream, lineStr)) {
			lines.push_back(lineStr);
		}
	}

	// prepare for getLine() funcs
	getLineIterator = lines.begin();
	if(getLineIterator == lines.end())
		isEof = true;

	Close();
}
void cFile::Release() {
	delete this;
}

bool cFile::Clear() {
	std::ofstream os(filePath.c_str(), std::ios::trunc);
	ASSERT(os.is_open() == true);
	if (!os.is_open()) 	{
		return false;
	}
	os.close();
	return true;
}

bool cFile::Clear(const zsString& path) {
	std::ofstream os(path.c_str(), std::ios::trunc);
	ASSERT(os.is_open() == true);
	if (!os.is_open()) 	{
		return false;
	}
	os.close();
	return true;
}

void cFile::Close() {
	stream.clear();
	stream.close();
}

bool cFile::ReadBinary(const zsString& path, void* data_out, size_t dataSize) {
	std::fstream is(path.c_str(), std::ios::in |std::ios::binary);
	ASSERT(is.is_open() == true);

	if (! is.is_open())
		return false;

	static char ansiPath[256];
	zsString::ConvertUniToAnsi(path, ansiPath, 256);

	
	is.read((char*)data_out, dataSize);
	is.close();

	return true;
}

bool cFile::ReadBinary(void* data_out, size_t dataSize) {
	std::fstream is(filePath.c_str(), std::ios::in | std::ios::binary);
	ASSERT(is.is_open() == true);

	if (!is.is_open())
		return false;

	static char ansiPath[256];
	zsString::ConvertUniToAnsi(filePath, ansiPath, 256);


	is.read((char*)data_out, dataSize);
	is.close();

	return true;
}

bool cFile::WriteBinary(const zsString& path, void* data, size_t dataSize) {
	std::ofstream os(path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	ASSERT(os.is_open() == true);

	// Fail to open
	if (!os.is_open()) {
		return false;
	}

	os.write((char*)data, dataSize);
	os.close();
	return true;
}

bool cFile::WriteBinary(void* data, size_t dataSize) {
	std::ofstream os(filePath.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	ASSERT(os.is_open() == true);

	// Fail to open
	if (!os.is_open()) {
		return false;
	}

	os.write((char*)data, dataSize);
	os.close();
	return true;
}

void cFile::DeleteFirstLines(size_t nLines) {
	//ZSASSERT_MSG(nLines <= lines.size(), zsString(L"Can't delete more line from file than how many it have: " + zsString(filePath)).c_str());

	// Reopen stream
	stream.close();
	stream.open(filePath.c_str(), std::ios::trunc | std::ios::out);

	// Move iteraton behind nLines
	auto iter = lines.begin();
	int idx = 0;
	while(idx != nLines) {
		iter++;
		idx++;
	}

	// Write lines after nLines
	while(iter != lines.end()) {
		stream << *iter << L"\n";
		iter++;
	}

	Close();
}

void cFile::Append(const IFile& file) {
	stream.close();
	stream.open(filePath.c_str(), std::ios::app);
	auto iter = file.GetLines().begin();
	while(iter != file.GetLines().end()) {
		lines.push_back(*iter);
		stream << *iter << L"\n";
		iter++;
	}
	
	Close();
}

bool cFile::Contains(const zsString& str) {
	auto iter = lines.begin();
	while(iter != lines.end()) {
		if(iter->find(str.c_str()) != std::wstring::npos)
			return true;
		iter++;
	}
	return false;
}

bool cFile::ReplaceAll(const zsString& repThat, const zsString& withThat) {
	// Reopen stream
	stream.close();
	stream.open(filePath.c_str(), std::ios::trunc | std::ios::out);

	// Replace strings
	auto iter = lines.begin();
	bool foundReplace = false;
	while(iter != lines.end()) {
		size_t start_pos = iter->find(repThat);
		while(start_pos != std::string::npos) {
			iter->replace(start_pos, repThat.size(), withThat);
			start_pos = iter->find(repThat);
			foundReplace = true;
		}
		iter++;
	}

	// Write lines
	iter = lines.begin();
	while(iter != lines.end()) {
		stream << *iter << L"\n";
		iter++;
	}

	Close();
	return foundReplace;
}

size_t cFile::GetNLines() const {
	return lines.size();
}

size_t cFile::GetSize() const {
	static char ansiPath[256];
	zsString::ConvertUniToAnsi(filePath, ansiPath, 256);

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

size_t cFile::GetSize(const zsString& path) {
	static char ansiPath[256];
	zsString::ConvertUniToAnsi(path, ansiPath, 256);

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

const zsString& cFile::GetLine() {
	zsString& data = *getLineIterator;
	getLineIterator++;
	if(getLineIterator == lines.end()) {
		isEof = true;
	}
	return data;
}

const std::list<zsString>& cFile::GetLines() const {
	return lines;
}

bool cFile::IsEOF() const {
	return isEof;
}

bool cFile::isFileExits(const zsString& str) {
	std::wfstream is(str.c_str(), std::ios::in);
	bool isOpen = is.is_open();
	is.close();
	return isOpen;
}

bool cFile::RemoveDuplicatedLines() {
	// Reopen stream
	stream.close();
	stream.open(filePath.c_str(), std::ios::trunc | std::ios::out);

	// Lines that are duplicated
	std::list<zsString> duplicatedLines;
	std::list<zsString> uniqLines;

	bool foundDuplicates = false;
	auto iterI = lines.begin();
	while(iterI != lines.end()) {
		// there is no duplication for that line (iterI)
		if(std::find(duplicatedLines.begin(), duplicatedLines.end(), *iterI) == duplicatedLines.end())
			uniqLines.push_back(*iterI);

		auto iterJ = lines.begin();
		while(iterJ != lines.end()) {
			if(iterJ != iterI) {
				if(*iterJ == *iterI) {
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
	while(iterI != uniqLines.end()) {
		lines.push_back(*iterI);
		stream << *iterI << L"\n";
		iterI++;
	}

	Close();
	return foundDuplicates;
}

zsString cFile::GetStringBefore(const zsString& str) {
	auto iter = lines.begin();
	while(iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if(start_pos != std::wstring::npos) {
			return zsString(iter->substr(0, start_pos));
		}
		iter++;
	}
	return zsString();
}

zsString cFile::GetWordAfter(const zsString& str) {
	size_t idx = 0;
	auto iter = lines.begin();
	while(iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if(start_pos != std::wstring::npos) {
			start_pos += str.size();
			idx = start_pos;
			while((*iter)[idx] != ' ') {
				idx++;
			}
			return zsString(iter->substr(start_pos, idx - start_pos));
		}
		iter++;
	}
	return zsString();
}

std::list<zsString> cFile::GetLinesBetween(const zsString& str, const zsString& endLine) {
	std::list<zsString> result;
	auto iter = lines.begin();
	while(iter != lines.end()) {
		size_t start_pos = iter->find(str);
		if(start_pos != std::wstring::npos) {
			iter++;
			while(*iter != endLine) {
				result.push_back(*iter);
				iter++;
			}
			break;
		}
		iter++;
	}
	return result;
}

std::list<zsString> cFile::GetLinesBeginsWith(const zsString& str) {
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

std::list<zsString> cFile::GetLinesBeginsWithBetween(const zsString& str, const zsString& left, const zsString& right) {
	std::list<zsString> res = GetLinesBeginsWith(str);
	for (auto i = res.begin(); i != res.end(); i++)
		i->Between(left.c_str(), right.c_str());
	return res;
}

//-----------------------INTERNALS------------------------------//
std::ios::openmode cFile::ConvertToNativeOpenMode(eFileOpenMode m) {
	std::ios::openmode result;
	switch (m) {
		case eFileOpenMode::WRITE:
			result = std::ios::out;
			break;
		case eFileOpenMode::BINWRITE:
			result = std::ios::out | std::ios::binary;
			break;
		case eFileOpenMode::READ:
			result = std::ios::in;
			break;
		case eFileOpenMode::BINREAD:
			result = std::ios::in | std::ios::binary;
			break;
	}
	return result;
}