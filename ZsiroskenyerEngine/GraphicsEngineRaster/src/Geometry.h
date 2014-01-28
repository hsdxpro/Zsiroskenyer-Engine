////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a loaded, render-ready representation of a 3D mesh.
//	This is independent from materials and textures.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

class IVertexBuffer;
class IIndexBuffer;


class cGeometry {
	friend class _cGeometryBuilder;
public:
	struct tMatGroup {
		size_t id, indexOffset, indexCount;
	};

	cGeometry(IVertexBuffer *VB, IIndexBuffer *IB, std::vector<tMatGroup>* mg = NULL);
	~cGeometry();

	void SetMatGroups(std::vector<tMatGroup>& g);

	const IIndexBuffer* GetIndexBuffer() const;
	const IVertexBuffer* GetVertexBuffer() const;
	const std::vector<tMatGroup>& GetMatGroups() const;
protected:
	IVertexBuffer *VB;
	IIndexBuffer *IB;
	std::vector<tMatGroup> matGroups;
};