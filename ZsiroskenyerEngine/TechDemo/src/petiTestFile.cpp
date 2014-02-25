#include <iostream>
#include <conio.h>
#include <random>

#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <list>

#include "../../Core/src/VertexFormat.h"

#include <Windows.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Main
int petiMain() {
	cVertexFormat fmt;
	vector<cVertexFormat::Attribute> attribs;
	cVertexFormat::Attribute a;

	a.bitsPerComponent = cVertexFormat::_32_BIT;
	a.nComponents = 3;
	a.semantic = cVertexFormat::POSITION;
	a.type = cVertexFormat::FLOAT;

	attribs.push_back(a);

	a.bitsPerComponent = cVertexFormat::_16_BIT;
	a.nComponents = 4;
	a.semantic = cVertexFormat::NORMAL;
	a.type = cVertexFormat::NORM;

	attribs.push_back(a);

	a.bitsPerComponent = cVertexFormat::_16_BIT;
	a.nComponents = 2;
	a.semantic = cVertexFormat::TEXCOORD;
	a.type = cVertexFormat::NORM;

	attribs.push_back(a);

	try {
		fmt.Create(attribs);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}

	auto dec = fmt.Decode();

	_getch();

	return 0;
}