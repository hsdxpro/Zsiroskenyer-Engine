////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a loaded, render-ready representation of a 3D mesh.
//	This is independent from materials and textures.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IVertexBuffer.h"
#include "IIndexBuffer.h"

class cGeometry {
public:
	cGeometry(IVertexBuffer *VB, IIndexBuffer *IB);

protected:
	IVertexBuffer *VB;
	IIndexBuffer *IB;
};