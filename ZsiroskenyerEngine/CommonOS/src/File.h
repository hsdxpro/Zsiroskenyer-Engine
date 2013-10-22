// File.h By Zsíroskenyér Team 2013.10.15 10:36
// parsing files like .txt .bin .dat ...
#pragma once

#include "../../Common/src/IFile.h"
#include <fstream>
#include <list>

class cFile : public IFile {
public:
	void Close() override;
	void Clear() override;

	void DeleteFirstLines(size_t nLines) override;

	void Append(const IFile& file) override;

	bool Find(const zsString& str) override;
	bool ReplaceAll(const zsString& repThat, const zsString& withThat) override;
	bool RemoveDuplicatedLines() override;

	zsString GetStringBefore(const zsString& str) override;
	zsString GetWordAfter(const zsString& str) override;

	std::list<zsString> GetLinesUnder(const zsString& str, const zsString& endLine) override;

	const zsString& GetLine() override;
	const std::list<zsString>& GetLines() const override;
	size_t  GetNLines() const override;

	bool IsEOF() const override;
	static bool isFileExits(const zsString& str);

	cFile(const zsString& file);
	cFile();
protected:
	// The file stream, that we read up
	std::wfstream stream;

	zsString filePath;

	bool isEof;

	// Iterator for getting lines from cFileParser
	std::list<zsString>::iterator getLineIterator;
	std::list<zsString> lines;
};
