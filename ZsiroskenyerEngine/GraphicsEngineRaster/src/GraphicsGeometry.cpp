////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsGeometry.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsGeometry.h"
#include "../../Core/src/IVertexBuffer.h"
#include "../../Core/src/IIndexBuffer.h"

cGraphicsGeometry::cGraphicsGeometry(IVertexBuffer *VB, IIndexBuffer *IB) 
:VB(VB), IB(IB) {
}

cGraphicsGeometry::~cGraphicsGeometry() {
	IB->Release();
	VB->Release();
}

const IIndexBuffer* cGraphicsGeometry::GetIndexBuffer() const {
	return IB;
}
const IVertexBuffer* cGraphicsGeometry::GetVertexBuffer() const {
	return VB;
}