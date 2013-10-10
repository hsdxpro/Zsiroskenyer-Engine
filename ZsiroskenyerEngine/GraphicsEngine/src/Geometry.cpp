////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Geometry.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "Geometry.h"


cGeometry::cGeometry(Vec3 *verticesPos, uint32 *indices,  uint32 nVertex, uint32 nIndex)
	:verticesPos(verticesPos),nVertex(nVertex), indices(indices), nIndex(nIndex) {
}

Vec3 *cGeometry::getVerticesPos() const {
	return verticesPos;
}

uint32 *cGeometry::getIndices() const {
	return indices;
}

uint32 cGeometry::getNVertex() const {
	return nVertex;
}

uint32 cGeometry::getNIndex() const {
	return nIndex;
}

