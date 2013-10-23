////////////////////////////////////////////////////////////////////////////////
//	GraphicsCommon/src/VertexFormat.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class declaration is worth a thousand words...
////////////////////////////////////////////////////////////////////////////////
#pragma once

enum class eVertexAttribute : unsigned {
	POSITION	= 1,
	NORMAL		= 2,
	TEXCOORD	= 4,
	COLOR		= 8,
	TANGENT		= 16,
	BONE		= 32,
};


class cVertexFormat {
public:
	void SetAttribute(eVertexAttribute attrib, bool value);
	void AddAttribute(eVertexAttribute attrib);
	void ClearAttribute(eVertexAttribute attrib);

	unsigned Get();

	cVertexFormat();
private:
	unsigned attribs;
};