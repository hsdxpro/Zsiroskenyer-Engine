////////////////////////////////////////////////////////////////////////////////
//	2013.oct.11, Zsirkenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This is a helper class that is responsible for building and manipulating
//	cGeometries on the run.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/zsString.h"

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

	// Free up dynamic memory in the result !
	static tGeometryDesc LoadGeometry(const zsString& filePath);
	cGeometryBuilder();
};