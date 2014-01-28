/*
This is not junk!

This class is supposed to provide an interface for the outside-of-graphicsengine world
for geometry creation, and adding it to the database loaded by the resource manager.
The resource manager should also use this class for loading geometries.

Not very necessary at the moment, but implement it, because it'll be useful in the future.
*/


#include "../../Core/src/zsString.h"
#include "Geometry.h"

class cResourceManager;

class _cGeometryBuilder {
public:
	typedef cGeometry::tMatGroup tMatGroup;

	// constructors
	_cGeometryBuilder();
	~_cGeometryBuilder();
	// DELETED
	_cGeometryBuilder(const _cGeometryBuilder&) = delete;
	_cGeometryBuilder& operator=(const _cGeometryBuilder&) = delete;

	// vertex data
	void SetVertexStride(size_t vSize);
	void InsertVertices(const float* verts, size_t position, size_t count);
	void EraseVertices(size_t position, size_t count);
	// index data
	void InsertIndices(unsigned* indices, size_t position, size_t count);
	void EraseIndices(size_t position, size_t count);
	// material groups
	void AddMatGroup(size_t id, size_t indexOffset, size_t indexCount);
	void EraseMatGroup(size_t idx);

	// file handling
	void LoadFile(const zsString& path);

	// concat two geometries
	_cGeometryBuilder& Concat(const _cGeometryBuilder& rhs);

	// clone geometry
	void Clone(const _cGeometryBuilder& other);
	void Clone(const cGeometry* geom);

	// submit or reset geometry
	cGeometry* Create();
	void Reset();
private:
	
};