////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a loaded, render-ready representation of a 3D mesh.
//	This is independent from materials and textures.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "..\..\CommonLib\src\common.h"
#include "..\..\CommonLib\src\math\vec3.h"

class cGeometry {
public:
	cGeometry(Vec3 *verticesPos, uint32 *indices,  uint32 nVertex, uint32 nIndex);

	Vec3 *getVerticesPos() const;
	uint32 *getIndices() const;

	uint32 getNVertex() const;
	uint32 getNIndex() const;
protected:
	Vec3	*verticesPos;
	uint32		nVertex;

	uint32		*indices;
	uint32		nIndex;
};