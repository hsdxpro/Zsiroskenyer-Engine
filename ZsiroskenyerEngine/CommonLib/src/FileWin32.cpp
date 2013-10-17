#include "FileWin32.h"
#include "common.h"

cFileWin32::cFileWin32()
:isEof(false) {
}

cFileWin32::cFileWin32(const zsString& file)
:isEof(false) {
	fileName = file;

	lines.clear();
	stream.clear();
	stream.close();
	// Open stream
	stream.open(fileName.c_str(), std::ios_base::in);
	if(!stream.is_open()) {
		//ZS_MSG(zsString(L"Can't open file: " + srcFile).c_str());
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
}


void cFileWin32::Clear() {
	std::ofstream os(fileName.c_str(), std::ios::trunc);
	os.close();
	lines.clear();
}

void cFileWin32::Close() {
	stream.clear();
	stream.close();
}

void cFileWin32::DeleteFirstLines(size_t nLines) {
	//ZSASSERT_MSG(nLines <= lines.size(), zsString(L"Can't delete more line from file than how many it have: " + zsString(fileName)).c_str());

	// Reopen stream
	stream.close();
	stream.open(fileName.c_str(), std::ios::trunc | std::ios_base::out);

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

void cFileWin32::Append(const cFileWin32& file) {
	stream.close();
	stream.open(fileName.c_str(), std::ios_base::app);
	auto iter = file.GetLines().begin();
	while(iter != file.GetLines().end()) {
		lines.push_back(*iter);
		stream << *iter << L"\n";
		iter++;
	}
	
	Close();
}

bool cFileWin32::Find(const zsString& str) {
	auto iter = lines.begin();
	while(iter != lines.end()) {
		if(iter->find(str.c_str()) != std::wstring::npos)
			return true;
		iter++;
	}
	return false;
}

bool cFileWin32::ReplaceAll(const zsString& repThat, const zsString& withThat) {
	// Reopen stream
	stream.close();
	stream.open(fileName.c_str(), std::ios::trunc | std::ios_base::out);

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

size_t cFileWin32::GetNLines() const {
	return lines.size();
}

const zsString& cFileWin32::GetLine() {
	zsString& data = *getLineIterator;
	getLineIterator++;
	if(getLineIterator == lines.end()) {
		isEof = true;
	}
	return data;
}

const std::list<zsString>& cFileWin32::GetLines() const {
	return lines;
}

bool cFileWin32::IsEOF() {
	return isEof;
}

bool cFileWin32::RemoveDuplicatedLines() {
	// Reopen stream
	stream.close();
	stream.open(fileName.c_str(), std::ios::trunc | std::ios_base::out);

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

zsString cFileWin32::GetStringBefore(const zsString& str) {
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

zsString cFileWin32::GetWordAfter(const zsString& str) {
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

std::list<zsString> cFileWin32::GetLinesUnder(const zsString& str, const zsString& endLine) {
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