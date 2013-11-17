// IFile by Zsíroskenyér Team 2013.10.22 9:46
// Used for interfacing different file accessing on platforms
#pragma once

#include <list>
#include "zsString.h"

class IFile {
public:
	virtual void Release() = 0;

	// Closes the file stream
	virtual void Close() = 0; 

	// Clear contents of the file
	virtual void Clear() = 0;

	// Deletes nLines line from the top of the file
	virtual void DeleteFirstLines(size_t nLines) = 0;

	// Append another file to my endings
	virtual void Append(const IFile& file) = 0;

	// return true if found the string
	virtual bool Find(const zsString& str) = 0;

	// Replace all strings repThat to withThat in the file
	virtual bool ReplaceAll(const zsString& repThat, const zsString& withThat) = 0;

	// Remove lines that duplicated
	virtual bool RemoveDuplicatedLines() = 0;

	// Get string before first founded string "str" in the file
	virtual zsString GetStringBefore(const zsString& str) = 0;

	// Get word (whitespace delim) after first founded string "str" in the file
	virtual zsString GetWordAfter(const zsString& str) = 0;

	// Get list of lines which is between str and endLine
	virtual std::list<zsString> GetLinesUnder(const zsString& str, const zsString& endLine) = 0;

	// Gor each call you get the next line of the file, at creation you are at start
	virtual const zsString& GetLine() = 0;
	
	// Get list of line Strings
	virtual const std::list<zsString>& GetLines() const = 0;

	// How many lines that file has
	virtual size_t  GetNLines() const = 0;

	// Get byte size of the file
	virtual size_t GetSize() const = 0;

	// We reach the end of the file stream ? It's possible with GetLine() func
	virtual bool IsEOF() const = 0;


// STATIC UTILITY
	// Create new instance of file
	static IFile* Create(const zsString& filePath);

	// file Exists?
	static bool isFileExits(const zsString& str);

	// Read up binary file to buffer
	static bool ReadBinary(const zsString& path, void* data_out, const size_t& dataSize);

	// Write data to binary file
	static bool WriteBinary(const zsString& path, void* data, const size_t& dataSize);

	// Clear file to 0 bytes (empty it)
	static bool Clear(const zsString& path);

	// Get byte size of the file
	static size_t GetSize(const zsString& path);
};