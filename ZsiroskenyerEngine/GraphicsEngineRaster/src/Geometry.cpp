////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Geometry.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Geometry.h"
#include "../../Core/src/IVertexBuffer.h"
#include "../../Core/src/IIndexBuffer.h"

cGeometry::cGeometry(IVertexBuffer *VB, IIndexBuffer *IB, std::vector<tMatGroup>* mg)
:VB(VB), IB(IB) {
	if (mg)
		matGroups = *mg;
}

cGeometry::~cGeometry() {
	if (IB!=nullptr) IB->Release();
	if (VB!=nullptr) VB->Release();
}

void cGeometry::SetMatGroups(std::vector<tMatGroup>& g) {
	matGroups = g;
}

const IIndexBuffer* cGeometry::GetIndexBuffer() const {
	return IB;
}
const IVertexBuffer* cGeometry::GetVertexBuffer() const {
	return VB;
}

const std::vector<cGeometry::tMatGroup>& cGeometry::GetMatGroups() const {
	return matGroups;
}