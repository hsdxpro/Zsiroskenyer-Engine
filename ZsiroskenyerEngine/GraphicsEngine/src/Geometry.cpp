////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Geometry.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h"

cGeometry::cGeometry(IVertexBuffer *VB, IIndexBuffer *IB) 
:VB(VB), IB(IB) {
}


const IIndexBuffer* cGeometry::GetIndexBuffer() const {
	return IB;
}
const IVertexBuffer* cGeometry::GetVertexBuffer() const {
	return VB;
}