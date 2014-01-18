#include "StrUtil.h"

void cStrUtil::ToAnsi(const zsString& str, char* dst, size_t nChars) {
	wcstombs(dst, str.c_str(), nChars);
}

size_t cStrUtil::ToUnsigned(const zsString& str) {
	std::wstringstream ss;
	ss << str.c_str();
	size_t res;
	ss >> res;
	return res;
}

bool cStrUtil::Contains(const zsString& in, const zsString& that) {
	if (in.find(that.c_str()) != std::wstring::npos)
		return true;
	else
		return false;
}

bool cStrUtil::Contains(const std::list<zsString>& fileLines, const zsString& that) {
	auto iter = fileLines.begin();
	while (iter != fileLines.end()) {
		if (iter->find(that.c_str()) != std::wstring::npos)
			return true;
		iter++;
	}
	return false;
}

// returns - 1 : string not found
// return >= 0 : index of the founded str in zsString
int cStrUtil::Find(const zsString& in, const zsString& that) {
	return in.find(that.c_str());
	/*
	size_t mainIndex = 0;
	wchar_t const* self = in.c_str();
	wchar_t ch = '\0';
	while ((ch = self[mainIndex]) != '\0') {
		if (that[0] == ch) {
			bool equal = true;
			size_t secIndex = 0;
			size_t fIndex = mainIndex;
			while (that[secIndex] != '\0') {
				ch = self[fIndex];
				if (that[secIndex] != ch) {
					equal = false;
					break;
				}
				fIndex++;
				secIndex++;
			}
			if (equal)
				return mainIndex;

		}
		mainIndex++;
	}
	return -1;
	*/
}

// returns - 1 : string not found
// return >= 0 : index of the founded str in zsString
int cStrUtil::Find(const zsString& str, wchar_t ch) {
	wchar_t const* strP = str.c_str();
	size_t idx = 0;
	while (strP[idx] != '\0')
	if (strP[idx++] == ch)
		return idx;

	return -1;
}

bool cStrUtil::Begins(const zsString& str, const zsString& withThat) {
	bool match = true;

	wchar_t const* tmp1 = str.c_str();
	wchar_t const* tmp2 = withThat.c_str();

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

	return match;
}

void cStrUtil::CutNumberFromEnd(char* src) {
	// Move to end
	while (*src != '\0')
		src++;

	// terminate in numbers (cutting)
	while (isdigit(*--src))
		*src = '\0';
}

// Gather string between left and right characters, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
void cStrUtil::Between(zsString& strOut, wchar_t left, wchar_t right) {
	wchar_t const* str = strOut.c_str();
	size_t leftIdx = 0;

	// Reach left bound
	while (str[leftIdx] != left || str[leftIdx] == '\0')
		leftIdx++;

	// Reach right bound
	size_t rightIdx = leftIdx;
	while (str[rightIdx] != right || str[rightIdx] == '\0')
		rightIdx++;

	strOut = strOut.substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
}

// Gather string between left and right strings, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
void cStrUtil::Between(zsString& strOut, const wchar_t* left, const wchar_t* right) {
	wchar_t const* str = strOut.c_str();

	size_t leftIdx = 0;
	size_t helperIdx = 0;
	size_t startIdx;

	// Reach index of var:left str in c_str()
	while (str[leftIdx] != '\0') {
		// Save left idx for BackUps
		startIdx = leftIdx;

		// Search var:left string in c_str() from current char
		helperIdx = 0;
		while (left[helperIdx] != '\0') {

			// left string part not equal with current c_str() char
			if (left[helperIdx] != str[leftIdx])
				break;

			helperIdx++;
			leftIdx++;
		}

		// Yeah found left string, terminate loop
		if (left[helperIdx] == '\0') {
			break;
		}

		// Backup left idx
		leftIdx = startIdx;

		leftIdx++;
	}



	// Reach index of var:left str in c_str()
	size_t rightIdx = leftIdx;
	while (str[rightIdx] != '\0') {
		// Save left idx for BackUps
		startIdx = rightIdx;

		// Search var:left string in c_str() from current char
		helperIdx = 0;
		while (right[helperIdx] != '\0') {

			// left string part not equal with current c_str() char
			if (right[helperIdx] != str[rightIdx])
				break;

			helperIdx++;
			rightIdx++;
		}

		// Backup rightidx
		rightIdx = startIdx;

		// Yeah found var:right string, terminate loop
		if (right[helperIdx] == '\0') {
			break;
		}

		rightIdx++;
	}

	// ex   azt_bazt    (left:azt, right:bazt)
	//leftIdx = index of 't'
	// rightIDx = index of 'b'
	strOut = strOut.substr(leftIdx, rightIdx - leftIdx);
}

void cStrUtil::Between(zsString& strOut, wchar_t left, const wchar_t* rightDelims, size_t nRightDelims) {
	wchar_t const* str = strOut.c_str();
	size_t leftIdx = 0;

	// Reach left bound
	while (str[leftIdx] != left || str[leftIdx] == '\0')
		leftIdx++;

	// Reach right bound
	size_t rightIdx = leftIdx;
	size_t i = 0;
	while (str[rightIdx] != '\0')
	{
		for (i = 0; i < nRightDelims; i++)
		if (str[rightIdx] == rightDelims[i])
			break;

		if (i != nRightDelims)
			break;

		rightIdx++;
	}

	strOut = strOut.substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
}

zsString cStrUtil::SubStrLeft(const zsString& str, size_t pos, wchar_t leftBound, size_t leftCutOffset /*= 0*/) {
	wchar_t const* strP = str.c_str() + pos;
	size_t idxLeft = pos;
	while (*strP != leftBound)
	{
		strP--;
		idxLeft--;
	}

	return str.substr(idxLeft + leftCutOffset, (pos + 1) - idxLeft - leftCutOffset);
}

zsString cStrUtil::SubStrRight(const zsString& str, size_t pos, wchar_t rightBound, size_t rightCutOffset /*= 0*/) {
	wchar_t const* strP = str.c_str() + pos;
	size_t idxRight = pos;
	while (*strP != rightBound)
	{
		strP++;
		idxRight++;
	}

	return str.substr(pos, (idxRight + 1) - pos + rightCutOffset);
}

#pragma message("asdasdasdasdsa _todo")
zsString cStrUtil::TrimSpaceBounds(const zsString& str) {
	return zsString();
}

zsString cStrUtil::Between(const zsString& s, wchar_t left, const wchar_t* rightDelims, size_t nRightDelims) {
	wchar_t const* str = s.c_str();
	size_t leftIdx = 0;

	// Reach left bound
	while (str[leftIdx] != left || str[leftIdx] == '\0')
		leftIdx++;

	// Reach right bound
	size_t rightIdx = leftIdx;
	size_t i = 0;
	while (str[rightIdx] != '\0')
	{
		for (i = 0; i < nRightDelims; i++)
		if (str[rightIdx] == rightDelims[i])
			break;

		if (i != nRightDelims)
			break;

		rightIdx++;
	}

	return s.substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
}

#pragma message("asdasdasdasdsa _todo2")
std::list<size_t> cStrUtil::GetLines(const std::list<zsString>& strs, size_t startLineIdx, const zsString& containStr) {
	std::list<size_t> result;
	return result;
}

// Gather string between left and right characters, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
zsString cStrUtil::Between(const zsString& s, wchar_t left, wchar_t right) {
	wchar_t const* str = s.c_str();
	size_t leftIdx = 0;

	// Reach left bound
	while (str[leftIdx] != left || str[leftIdx] == '\0')
		leftIdx++;

	// Reach right bound
	size_t rightIdx = leftIdx;
	while (str[rightIdx] != right || str[rightIdx] == '\0')
		rightIdx++;

	return s.substr(leftIdx + 1, (rightIdx - 1) - leftIdx);
}

// Gather string between left and right strings, for ex. zsString ex = _asdasd; ex.Between('-',';') returns asdasd   
zsString cStrUtil::Between(const zsString& s, const wchar_t* left, const wchar_t* right) {
	wchar_t const* str = s.c_str();

	size_t leftIdx = 0;
	size_t helperIdx = 0;
	size_t startIdx;

	// Reach index of var:left str in c_str()
	while (str[leftIdx] != '\0') {
		// Save left idx for BackUps
		startIdx = leftIdx;

		// Search var:left string in c_str() from current char
		helperIdx = 0;
		while (left[helperIdx] != '\0') {

			// left string part not equal with current c_str() char
			if (left[helperIdx] != str[leftIdx])
				break;

			helperIdx++;
			leftIdx++;
		}

		// Yeah found left string, terminate loop
		if (left[helperIdx] == '\0') {
			break;
		}

		// Backup left idx
		leftIdx = startIdx;

		leftIdx++;
	}



	// Reach index of var:left str in c_str()
	size_t rightIdx = leftIdx;
	while (str[rightIdx] != '\0') {
		// Save left idx for BackUps
		startIdx = rightIdx;

		// Search var:left string in c_str() from current char
		helperIdx = 0;
		while (right[helperIdx] != '\0') {

			// left string part not equal with current c_str() char
			if (right[helperIdx] != str[rightIdx])
				break;

			helperIdx++;
			rightIdx++;
		}

		// Backup rightidx
		rightIdx = startIdx;

		// Yeah found var:right string, terminate loop
		if (right[helperIdx] == '\0') {
			break;
		}

		rightIdx++;
	}

	// ex   azt_bazt    (left:azt, right:bazt)
	//leftIdx = index of 't'
	// rightIDx = index of 'b'
	return  s.substr(leftIdx, rightIdx - leftIdx);
}

void cStrUtil::CutDirectory(zsString& strOut) {
	const wchar_t *str = strOut.c_str();
	size_t nDelimIdx = 0;
	size_t idx = 0;
	while (str[idx] != '\0') {
		idx++;

		if (str[idx] == '\\' || str[idx] == '/')
			nDelimIdx = idx;
	}
	nDelimIdx++; // go after delim..

	// Okay so we found the index of the last '\\' or '/'
	size_t newSize = strOut.size() - nDelimIdx; // TODO size() slow solution

	strOut = strOut.substr(nDelimIdx, newSize);
}

void cStrUtil::CutFrontFromDelim(zsString& strOut, wchar_t ch) {
	const wchar_t *str = strOut.c_str();
	size_t idx = 0;
	while (str[idx] != '\0' && str[idx++] != ch);
	strOut = strOut.substr(idx, strOut.size() - idx);
}

void cStrUtil::GetNumberFromEnd(char* src, char* buf_out) {
	// Move to end
	while (*src != '\0')
		src++;

	while (isdigit(*--src))
		*buf_out++ = *src;
	*buf_out = '\0';
}

void cStrUtil::GetWordBetween(zsString& str, char left, char right, char* buf_out) {
	wchar_t const* strP = str.c_str();

	// Leave spaces from left and try reach left bound
	while ((char)*strP == ' ' || (char)*strP != left)
		strP++;
	strP++;

	// Leave spaces
	while ((char)*strP == ' ')
		strP++;

	// Copy characters while not end of string or right bound reached
	while ((char)*strP != right && (char)*strP != '\0')
		*buf_out++ = (char)*strP++;

	// Terminate string
	*buf_out = '\0';
}

void cStrUtil::GetFloats(const zsString& str, std::vector<float>& floats_out) {
	// Clear output if it's not clear
	if (floats_out.size() != 0)
		floats_out.clear();

	wchar_t const* strP = str.c_str();
	while (!isdigit(*strP))
		strP++;

	size_t offset = 0;
	wchar_t* end = NULL;
	do {
		floats_out.push_back((float)wcstod(strP + offset, &end));
		offset = end - strP;
	} while (*end != '\0' && end != NULL);
}

zsString cStrUtil::GetDirectory(const zsString& str) {
	const wchar_t* strP = str.c_str();
	if (strP[0] == '\0')
		return zsString();


	// Copy whole string
	zsString directory = strP;

	// Null terminate from right to left, while not reaching directory
	size_t i = directory.size() - 1;
	for (; directory[i] != '\\' && directory[i] != '/' && i >= 0; i--);

	directory.resize(i + 1);

	return directory;
}

zsString cStrUtil::GetWordAfter(const std::list<zsString>& strs, const zsString& str) {
	size_t idx = 0;
	auto iter = strs.begin();
	while (iter != strs.end()) {
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

std::list<zsString> cStrUtil::GetLinesBetween(const std::list<zsString>& fileLines, const zsString& str, const zsString& endLine) {
	std::list<zsString> result;

	// For each line
	auto iter = fileLines.begin();
	while (iter != fileLines.end()) {
		size_t start_pos = iter->find(str);

		// Gound str
		if (start_pos != std::wstring::npos) {
			iter++; // Step further
			// Check for out of bound
			if (iter == fileLines.end())
				break;

			while (iter != fileLines.end()) {
				if (iter->find(endLine) != std::wstring::npos)
					break;
				result.push_back(*iter);
				iter++;
			}
			break;
		}
		iter++;
	}
	return result;
}

std::list<zsString> cStrUtil::GetLinesBeginsWith(const std::list<zsString>& fileLines, const zsString& str) {
	std::list<zsString> result;

	bool match = true;
	auto iter = fileLines.begin();
	while (iter != fileLines.end()) {
		match = true;
		wchar_t const* tmp1 = iter->c_str();
		wchar_t const* tmp2 = str.c_str();

		if (*tmp1 == '\0')
			match = false;

		while (*tmp2 != '\0') {
			if (*tmp1 != *tmp2) {
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

std::list<size_t> cStrUtil::GetLinesContainingAllStr(const std::list<zsString>& in, const std::list<zsString>& those) {
	std::list<size_t> result;

	size_t idx = 0;
	for (auto it = in.begin(); it != in.end(); it++, idx++) {
		
		// if that line contains all of the "those" words, then push the string
		auto thos = in.begin();
		for (; thos != in.end(); thos++)
			if (!cStrUtil::Contains(*it, *thos))
				break;

			if (thos == in.end())
				result.push_back(idx);
	}

	return result;
}