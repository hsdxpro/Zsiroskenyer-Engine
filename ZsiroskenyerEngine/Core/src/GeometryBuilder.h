////////////////////////////////////////////////////////////////////////////////
//	2013.oct.11, Zsirkenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This is a helper class that is responsible for building and manipulating
//	cGeometries on the run.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/zsString.h"
#include "VertexFormat.h"

class cGeometryBuilder {
public:
	struct tGeometryDesc {
		struct tMatGroup {
			size_t id, indexOffset, indexCount;
		};

		void* indices;
		void* vertices;

		size_t nIndices;
		size_t nVertices;

		size_t indexStride;
		cVertexFormat vertexFormat;

		std::vector<tMatGroup> matGroups;
	};

	// Free up dynamic memory in the result !
	static tGeometryDesc LoadGeometry(const zsString& filePath);
	cGeometryBuilder();
};