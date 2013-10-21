////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Geometry.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a loaded, render-ready representation of a 3D mesh.
//	This is independent from materials and textures.
////////////////////////////////////////////////////////////////////////////////
#pragma once


class IVertexBuffer;
class IIndexBuffer;


class cGeometry {
public:
	cGeometry(IVertexBuffer *VB, IIndexBuffer *IB);
	~cGeometry();

	const IIndexBuffer* GetIndexBuffer() const;
	const IVertexBuffer* GetVertexBuffer() const;

	struct tMatGroup {
		size_t id, indexOffset, indexCount;
	};
protected:
	IVertexBuffer *VB;
	IIndexBuffer *IB;
};