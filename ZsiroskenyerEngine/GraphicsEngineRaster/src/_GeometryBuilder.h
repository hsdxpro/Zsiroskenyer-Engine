////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Awesome geometry builder
//
//	This class provides a robust way to create cGeometries. Used for loading
//	files, by the resource manager. Exposed to DLL users via an interface.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../Core/src/zsString.h"
#include "Geometry.h"

#include <cstdint>

class IGraphicsApi;

class _cGeometryBuilder {
public:
	typedef cGeometry::tMatGroup tMatGroup;

	// constructors
	_cGeometryBuilder(IGraphicsApi* gApi);
	_cGeometryBuilder(_cGeometryBuilder&& other);
	_cGeometryBuilder(const _cGeometryBuilder& other);
	~_cGeometryBuilder();

	// vertex data
	void SetVertexStride();
	void InsertVertices(const void* verts, size_t bytes, size_t offset = 0);
	void EraseVertices(size_t offset, size_t bytes);
	void* GetVertices();
	size_t GetNumVertices();
	// index data
	void InsertIndices(uint32_t* indices, size_t count, size_t position = 0);
	void EraseIndices(size_t position, size_t count);
	uint32_t* GetIndices();
	size_t GetNumIndices();
	// material groups
	void AddMatGroup(size_t id, size_t indexOffset, size_t indexCount);
	void EraseMatGroup(size_t idx);
	tMatGroup* GetMatGroups();
	size_t GetNumMatGroups();

	// file handling
	void LoadFile(const zsString& path);

	// concat two geometries
	/* maybe later
	_cGeometryBuilder& operator+=(const _cGeometryBuilder& rhs);
	_cGeometryBuilder operator+(const _cGeometryBuilder& rhs) const;
	*/

	// clone geometry
	_cGeometryBuilder& operator=(_cGeometryBuilder&& other);
	_cGeometryBuilder& operator=(const _cGeometryBuilder& other);

	// submit or reset geometry
	cGeometry* Create();
	void Reset();
private:
	// is this all okay?
	bool ValidateVertices();
	bool ValidateIndices();
	bool ValidateMatGroups();

	// optimize
	void Optimize();

	// memory management
	template <class T>
	void InsertBuf(T** buf, size_t bufSize, T* newData, size_t newDataSize, size_t offset);
	template <class T>
	void EraseBuf(T** buf, size_t bufSize, size_t rmDataSize, size_t rmOffset);

	// memory store
	void* vertices;
	uint32_t* indices;
	tMatGroup* matGroups;
	size_t nBytesVertices;
	size_t nIndices;
	size_t nMatGroups;

	// misc
	unsigned vertexSize;
	IGraphicsApi* gApi;
};