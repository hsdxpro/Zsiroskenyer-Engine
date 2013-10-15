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

	// uniq Lines
	std::list<zsString> uniqLines;

	bool foundDuplicates = false;
	auto iterI = lines.begin();
	while(iterI != lines.end()) {
		auto iterJ = lines.begin();
		while(iterJ != lines.end()) {
			if(iterJ != iterI) {
				if(*iterJ == *iterI) {
					iterJ = lines.erase(iterJ);
					foundDuplicates = true;
				}
			}
			iterJ++;
		}
		iterI++;
	}

	// Write to file
	iterI = lines.begin();
	while(iterI != lines.end()) {
		stream << *iterI << L"\n";
		iterI++;
	}

	Close();
	return foundDuplicates;
}