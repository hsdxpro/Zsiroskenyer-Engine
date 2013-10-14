////////////////////////////////////////////////////////////////////////////////
//	GraphicsCommon/src/VertexFormat.cpp
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "VertexFormat.h"

void cVertexFormat::SetAttribute(eVertexAttributes attrib, bool value) {
	value ? AddAttribute(attrib) : ClearAttribute(attrib);
}

void cVertexFormat::AddAttribute(eVertexAttributes attrib) {
	attribs |= (unsigned)attrib;
}

void cVertexFormat::ClearAttribute(eVertexAttributes attrib) {
	attribs &= (~(unsigned)attribs);
}

unsigned cVertexFormat::Get() {
	return attribs;
}


cVertexFormat::cVertexFormat() : attribs(0) {
}