// FileWin32.h By Zsíroskenyér Team 2013.10.15 10:36
// FileParser.h By Németh Richárd 2013.07.23 21:56
// parsing files like .txt .bin .dat ...
#pragma once

#include "zsString.h"
#include <fstream>
#include <list>

class cFileWin32 {
public:
	void Close();
	void Clear();

	void DeleteFirstLines(size_t nLines);

	void Append(const cFileWin32& file);

	bool Find(const zsString& str);
	bool ReplaceAll(const zsString& repThat, const zsString& withThat);
	bool RemoveDuplicatedLines();

	zsString GetStringBefore(const zsString& str);
	std::list<zsString> GetLinesUnder(const zsString& str, const zsString& endLine);

	const zsString& GetLine();
	const std::list<zsString>& GetLines() const;
	size_t  GetNLines() const;
	bool IsEOF();

	cFileWin32(const zsString& file);
	cFileWin32();
protected:
	// The file stream, that we read up
	std::wfstream stream;

	zsString fileName;

	bool isEof;

	// Iterator for getting lines from cFileParser
	std::list<zsString>::iterator getLineIterator;
	std::list<zsString> lines;
};
