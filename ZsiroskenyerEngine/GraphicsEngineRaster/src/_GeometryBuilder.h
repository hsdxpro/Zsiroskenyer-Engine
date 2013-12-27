/*
This is not junk!

This class is supposed to provide an interface for the outside-of-graphicsengine world
for geometry creation, and adding it to the database loaded by the resource manager.
The resource manager should also use this class for loading geometries.

Not very necessary at the moment, but implement it, because it'll be useful in the future.
*/


#include "../../Core/src/zsString.h"
#include "../../Core/src/IGraphicsEngine.h"
#include "Geometry.h"
#include "../../Core/src/math/math_all.h"
#include "ResourceReference.h"

class cResourceManager;

class _cGeometryBuilder {
public:
	struct tFace {
		unsigned v1, v2, v3;
		int matGroup;
	};
	typedef cGeometry::tMatGroup tMatGroup;

	// constructors
	_cGeometryBuilder(cResourceManager& rm);
	_cGeometryBuilder(const _cGeometryBuilder&) = delete;
	~_cGeometryBuilder();

	// assignement
	_cGeometryBuilder& operator=(const _cGeometryBuilder&) = delete;

	// vertex data
	void InsertVertices(const Vec3* verts, size_t position, size_t count);
	void EraseVertices(size_t position, size_t count);
	// index data
	void InsertFaces(tFace* faces, size_t position, size_t count);
	void InsertIndices(unsigned* indices, size_t position, size_t count);
	void EraseIndices(size_t position, size_t count);
	// material groups
	void AddMatGroup(size_t id, size_t indexOffset, size_t indexCount);
	tMatGroup& GetMatGroup(size_t idx);
	void EraseMatGroup(size_t idx);

	// file handling
	eGraphicsResult LoadFile(const zsString& path);

	// concat two geometries
	_cGeometryBuilder& Concat(const _cGeometryBuilder& rhs);
	_cGeometryBuilder& operator+=(const _cGeometryBuilder& rhs);
	_cGeometryBuilder  operator+(const _cGeometryBuilder& rhs);

	// clone geometry
	void Clone(const _cGeometryBuilder& other);
	void Clone(cGeometryRef geom);

	// submit or reset geometry
	cGeometryRef Submit(const zsString& name);
	void Reset();
private:
	cResourceManager& rm;
};