////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Awesome geometry builder
//
////////////////////////////////////////////////////////////////////////////////

#include "_GeometryBuilder.h"
#include "../../Core/src/GAPI.h"
#include <cassert>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
//	Ctor

// default
_cGeometryBuilder::_cGeometryBuilder(IGraphicsApi* gApi)
:
vertices(nullptr),
indices(nullptr),
matGroups(nullptr),
nBytesVertices(0),
nIndices(0),
nMatGroups(0),
vSize(1),
gApi(gApi)
{
	return;
}

// move
_cGeometryBuilder::_cGeometryBuilder(_cGeometryBuilder&& other)
:
vertices	(other.vertices),
indices		(other.indices),
matGroups	(other.matGroups),
nBytesVertices(other.nBytesVertices),
nIndices	(other.nIndices),
nMatGroups	(other.nMatGroups),
vSize		(other.vSize),
gApi		(other.gApi)
{
	other.vertices = nullptr;
	other.indices = nullptr;
	other.matGroups = nullptr;
}

// copy
_cGeometryBuilder::_cGeometryBuilder(const _cGeometryBuilder& other) 
:
vertices(nullptr),
indices(nullptr),
matGroups(nullptr),
nBytesVertices(other.nBytesVertices),
nIndices	(other.nIndices),
nMatGroups	(other.nMatGroups),
vSize		(other.vSize),
gApi		(other.gApi)
{
	InsertBuf((uint8_t**)&vertices, 0, (uint8_t*)other.vertices, nBytesVertices, 0);
	InsertBuf(&indices, 0, other.indices, nIndices, 0);
	InsertBuf(&matGroups, 0, other.matGroups, other.nMatGroups, 0);
}

// destruct
_cGeometryBuilder::~_cGeometryBuilder() {
	if (vertices) delete[] vertices;
	if (indices) delete[] indices;
	if (matGroups) delete[] matGroups;
}


////////////////////////////////////////////////////////////////////////////////
// Insert/Remove stuff

void _cGeometryBuilder::SetVertexStride(size_t vSize) {
	this->vSize = vSize;
}

// vertex data
void _cGeometryBuilder::InsertVertices(const void* verts, size_t bytes, size_t offset) {
	InsertBuf((uint8_t**)&vertices, nBytesVertices, (uint8_t*)verts, bytes, offset);
	nBytesVertices += bytes;
}
void _cGeometryBuilder::EraseVertices(size_t offset, size_t bytes) {
	EraseBuf((uint8_t**)&vertices, nBytesVertices, bytes, offset);
	nBytesVertices -= bytes;
}
void* _cGeometryBuilder::GetVertices() {
	return vertices;
}
size_t _cGeometryBuilder::GetNumVertices() {
	return nBytesVertices;
}

// index data
void _cGeometryBuilder::InsertIndices(uint32_t* indices, size_t count, size_t position) {
	InsertBuf(&this->indices, nIndices, indices, count, position);
	nIndices += count;
}
void _cGeometryBuilder::EraseIndices(size_t position, size_t count) {
	EraseBuf(&indices, nIndices, count, position);
	nIndices -= count;
}
uint32_t* _cGeometryBuilder::GetIndices() {
	return indices;
}
size_t _cGeometryBuilder::GetNumIndices() {
	return nIndices;
}

// material groups
void _cGeometryBuilder::AddMatGroup(size_t id, size_t indexOffset, size_t indexCount) {
	tMatGroup grp;
	grp.id = id;
	grp.indexCount = indexCount;
	grp.indexOffset = indexOffset;

	InsertBuf(&matGroups, nMatGroups, &grp, 1, 0);
	nMatGroups++;
}
void _cGeometryBuilder::EraseMatGroup(size_t idx) {
	EraseBuf(&matGroups, nMatGroups, 1, idx);
	nMatGroups--;
}
auto _cGeometryBuilder::GetMatGroups() -> tMatGroup* {
	return matGroups;
}
size_t _cGeometryBuilder::GetNumMatGroups() {
	return nMatGroups;
}


////////////////////////////////////////////////////////////////////////////////
// Load files
void _cGeometryBuilder::LoadFile(const zsString& path) {
	throw std::runtime_error("Hogy a gecibe töltenénk már fájlt...?");
}


////////////////////////////////////////////////////////////////////////////////
// concat two geometries

/* maybe later
_cGeometryBuilder& _cGeometryBuilder::operator+=(const _cGeometryBuilder& rhs) {
	InsertBuf((uint8_t**)&vertices, nBytesVertices, (uint8_t*)rhs.vertices, rhs.nBytesVertices, 0);
	InsertBuf(&indices, 0, rhs.indices, nIndices, 0);

}

_cGeometryBuilder _cGeometryBuilder::operator+(const _cGeometryBuilder& rhs) const {

}
*/


////////////////////////////////////////////////////////////////////////////////
// clone geometry
_cGeometryBuilder& _cGeometryBuilder::operator=(_cGeometryBuilder&& other) {
	auto gApi = this->gApi;
	this->~_cGeometryBuilder();
	new (this) _cGeometryBuilder(std::move(other));
	this->gApi = gApi;
	return *this;
}
_cGeometryBuilder& _cGeometryBuilder::operator=(const _cGeometryBuilder& other) {
	auto gApi = this->gApi;
	this->~_cGeometryBuilder();
	new (this) _cGeometryBuilder(other);
	this->gApi = gApi;
	return *this;
}


////////////////////////////////////////////////////////////////////////////////
// validation

bool _cGeometryBuilder::ValidateVertices() {
	return nBytesVertices%vSize == 0;
}

bool _cGeometryBuilder::ValidateIndices() {
	size_t maxIndex = 0;
	for (size_t i = 0; i < nIndices; i++) {
		maxIndex = (maxIndex>indices[i] ? maxIndex : indices[i]);
	}
	return maxIndex < nBytesVertices / vSize;
}

bool _cGeometryBuilder::ValidateMatGroups() {
	size_t maxIndex = 0;
	for (size_t i = 0; i < nMatGroups; i++) {
		maxIndex = matGroups[i].indexCount + matGroups[i].indexOffset;
	}
	return maxIndex < nIndices;
}

////////////////////////////////////////////////////////////////////////////////
// optimization

void _cGeometryBuilder::Optimize() {
	// muhahahahahahaha slow geometry ahhhaha
}

////////////////////////////////////////////////////////////////////////////////
// submit or reset geometry

cGeometry* _cGeometryBuilder::Create() {
	if (!ValidateIndices()) {
		throw std::runtime_error("vertex data is over-indexed");
	}
	else if (!ValidateVertices()) {
		throw std::runtime_error("there are truncated vertices");
	}
	else if (!ValidateMatGroups()) {
		throw std::runtime_error("mat groups contain inexistent indices");
	}
	else {
		Optimize();

		IIndexBuffer* ib;
		IVertexBuffer* vb;
		std::vector<tMatGroup> mg;

		eGapiResult r;
		r = gApi->CreateIndexBuffer(&ib, nIndices*sizeof(uint32_t), eUsage::DEFAULT, indices);
		if (r!=eGapiResult::OK) {
			throw std::runtime_error("failed to create index buffer");
		}
		r = gApi->CreateVertexBuffer(&vb, nBytesVertices, eUsage::DEFAULT, vertices);
		if (r != eGapiResult::OK) {
			throw std::runtime_error("failed to create vertex buffer");
		}

		mg.resize(nMatGroups);
		for (size_t i = 0; i < nMatGroups; i++) {
			mg.push_back(matGroups[i]);
		}

		cGeometry* geom = new cGeometry(vb, ib, &mg);
		return geom;
	}
}

void _cGeometryBuilder::Reset() {
	IGraphicsApi* gApi = this->gApi;
	this->~_cGeometryBuilder();
	new (this) _cGeometryBuilder(gApi);
}

////////////////////////////////////////////////////////////////////////////////
// Helpers

// buffer resizing
template <class T>
void _cGeometryBuilder::InsertBuf(T** buf, size_t bufSize, T* newData, size_t newDataSize, size_t offset) {
	assert(bufSize >= offset);

	T *oldBuf(*buf), *newBuf;

	newBuf = new T[bufSize + newDataSize];

	memcpy(newBuf, oldBuf, offset*sizeof(T));
	memcpy(newBuf + offset + newDataSize, oldBuf + offset, (bufSize - offset)*sizeof(T));
	memcpy(newBuf + offset, newData, newDataSize*sizeof(T));

	*buf = newBuf;
	delete[] oldBuf;
}
template <class T>
void _cGeometryBuilder::EraseBuf(T** buf, size_t bufSize, size_t rmDataSize, size_t offset) {
	assert(bufSize >= offset + rmDataSize);

	T *oldBuf(*buf), *newBuf;

	newBuf = new T[bufSize - rmDataSize];

	memcpy(newBuf, oldBuf, offset);
	memcpy(newBuf + offset, oldBuf + offset + rmDataSize, bufSize - rmDataSize);

	*buf = newBuf;
	delete[] oldBuf;
}
