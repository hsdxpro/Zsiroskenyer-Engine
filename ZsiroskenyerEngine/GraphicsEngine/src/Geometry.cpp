////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Geometry.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h"
#include "../../GraphicsCommon/src/IVertexBuffer.h"
#include "../../GraphicsCommon/src/IIndexBuffer.h"

cGeometry::cGeometry(IVertexBuffer *VB, IIndexBuffer *IB) 
:VB(VB), IB(IB) {
}

cGeometry::~cGeometry() {
	IB->Release();
	VB->Release();
}

const IIndexBuffer* cGeometry::GetIndexBuffer() const {
	return IB;
}
const IVertexBuffer* cGeometry::GetVertexBuffer() const {
	return VB;
}