////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GeometryBuilder.h
//	2013.oct.11, Zsirkenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is a helper class that is responsible for building and manipulating
//	cGeometries on the run.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Common/src/zsString.h"
#include "../../GraphicsEngineRaster/src/Geometry.h"

class cGeometryBuilder {
public:
	struct tGeometryDesc {
		void* indices;
		void* vertices;

		size_t nIndices;
		size_t nVertices;

		size_t indexStride;
		size_t vertexStride;
	};

	// Loading .DAE geometry with Assimp 3d library
	tGeometryDesc LoadGeometryDAE(const zsString& fileName);

	cGeometryBuilder();
private:

};