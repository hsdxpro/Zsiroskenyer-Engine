////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Awesome geometry builder
//
////////////////////////////////////////////////////////////////////////////////

#include "_GeometryBuilder.h"

#include "../../Core/src/GAPI.h"

// DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"
#include "../../Core/src/tipsify.h"

// Utils
#include "../../Core/src/StrUtil.h"
#include "../../Core/src/FileUtil.h"

// Math
#include "../../Core/src/math/Vec3.h"
#include "../../Core/src/math/Vec2.h"

// Exception
#include <cassert>
#include <stdexcept>
#include "../../Core/src/Exception.h"

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
gApi(gApi)
{
	return;
}

// move
_cGeometryBuilder::_cGeometryBuilder(_cGeometryBuilder&& other)
:
vertices		(other.vertices),
indices			(other.indices),
matGroups		(other.matGroups),
nBytesVertices	(other.nBytesVertices),
nIndices		(other.nIndices),
nMatGroups		(other.nMatGroups),
vertexFormat	(other.vertexFormat),
gApi			(other.gApi)
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
vertexFormat(other.vertexFormat),
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

void _cGeometryBuilder::SetVertexFormat(cVertexFormat format) {
	this->vertexFormat = format;
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
cVertexFormat _cGeometryBuilder::GetVertexFormat() const {
	return vertexFormat;
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
	Reset();

	Assimp::Importer importer;

	if (!cFileUtil::isFileExits(path)) {
		ILog::GetInstance()->MsgBox(L"Can't open file: " + path);
		throw FileNotFoundException();
	}

	// read up dae scene
	char ansiFilePath[256];
	cStrUtil::ToAnsi(path, ansiFilePath, 256);
	const aiScene* scene = importer.ReadFile(ansiFilePath, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ImproveCacheLocality | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_FlipWindingOrder);
	if (scene == NULL) {
		ILog::GetInstance()->MsgBox(L"Can't found 3D model: " + path);
		throw FileNotFoundException();
	}

	size_t nMeshes = scene->mNumMeshes;
	aiMesh** meshes = scene->mMeshes;

	nIndices = 0;
	nMatGroups = nMeshes;

	size_t nVertices = 0;

	// Count indices, vertices.  Gather material groups
	matGroups = new tMatGroup[nMatGroups];
	for (size_t i = 0; i < nMeshes; i++) {
		aiMesh *mesh = meshes[i];

		size_t nMeshIndices = mesh->mNumFaces * 3;

		matGroups[i].id = mesh->mMaterialIndex;
		matGroups[i].indexOffset = nIndices;
		matGroups[i].indexCount = nMeshIndices;

		nVertices += mesh->mNumVertices;
		nIndices += nMeshIndices;
	}

	// DEFINE VERTEX STRUCTURE HERE.... @TODO REMOVE IT OR I KILL MYSELF
	struct baseVertex {
		Vec3 pos;
		Vec3 normal;
		Vec3 tangent;
		Vec2 tex;
		bool operator == (const baseVertex& v) { return pos == v.pos && normal == v.normal && tangent == v.tangent && tex == v.tex; }
	};

	std::vector<cVertexFormat::Attribute> attribs;

	cVertexFormat::Attribute a;
	// POSITION
	a.bitsPerComponent = cVertexFormat::_32_BIT;
	a.nComponents = 3;
	a.semantic = cVertexFormat::POSITION;
	a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// NORMAL
	a.bitsPerComponent = cVertexFormat::_32_BIT;
	a.nComponents = 3;
	a.semantic = cVertexFormat::NORMAL;
	a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// TANGENT
	a.bitsPerComponent = cVertexFormat::_32_BIT;
	a.nComponents = 3;
	a.semantic = cVertexFormat::COLOR;
	a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// TEX0
	a.bitsPerComponent = cVertexFormat::_32_BIT;
	a.nComponents = 2;
	a.semantic = cVertexFormat::TEXCOORD;
	a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// The vertex format !!!
	vertexFormat.Create(attribs);

	// Geometry read up
	baseVertex* verticesPtr = new baseVertex[nVertices];
	vertices = verticesPtr;
	indices = new uint32_t[nIndices];

	// Super TMP Vec3 for usage
	aiVector3D* supTmpVec;

	size_t indexI = 0;
	size_t vertexI = 0;
	size_t vertexOffset = 0;

	for (size_t i = 0; i < nMeshes; i++) {
		aiMesh* mesh = meshes[i];
		for (size_t j = 0; j < mesh->mNumFaces; indexI += 3, j++) {
			aiFace& face = mesh->mFaces[j];

			// For each face index
			for (size_t k = 0; k < 3; k++) {
				if (face.mNumIndices < 3)
					break;

				unsigned vertIdx = face.mIndices[k];
				// Index data
				indices[indexI + k] = vertIdx + vertexI;

				// Vertex Data
				if (mesh->HasPositions()) {
					supTmpVec = &mesh->mVertices[vertIdx];
					verticesPtr[vertIdx + vertexI].pos = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasNormals()) {
					supTmpVec = &mesh->mNormals[vertIdx];
					verticesPtr[vertIdx + vertexI].normal = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasTangentsAndBitangents()) {
					supTmpVec = &mesh->mTangents[vertIdx];
					verticesPtr[vertIdx + vertexI].tangent = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				// @TODO not general algorithm, wee need to handle more UV channels
				supTmpVec = &mesh->mTextureCoords[0][vertIdx];
				verticesPtr[vertIdx + vertexI].tex = Vec2(supTmpVec->x, -supTmpVec->y); // UV flip y
			}
		}
		vertexI += mesh->mNumVertices;
	}

	nBytesVertices = nVertices * sizeof(baseVertex);
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
	return nBytesVertices%vertexFormat.GetByteSize() == 0;
}

bool _cGeometryBuilder::ValidateIndices() {
	size_t maxIndex = 0;
	for (size_t i = 0; i < nIndices; i++) {
		maxIndex = (maxIndex>indices[i] ? maxIndex : indices[i]);
	}
	return maxIndex < nBytesVertices / vertexFormat.GetByteSize();
}

bool _cGeometryBuilder::ValidateMatGroups() {
	intptr_t maxIndex = -1;
	for (size_t i = 0; i < nMatGroups; i++) {
		maxIndex = matGroups[i].indexCount - 1 + matGroups[i].indexOffset;
	}
	return maxIndex < nIndices;
}

////////////////////////////////////////////////////////////////////////////////
// optimization

void _cGeometryBuilder::Optimize() {

	// Tipsify index reordering for less overlap rendering and  optimal post vertex cache
	uint32_t* reorderedIndices;
	reorderedIndices = new uint32_t[nIndices];
	
	reorderedIndices = tipsify(indices, nIndices / 3, nBytesVertices / vertexFormat.GetByteSize(), 16); // vertexCache size  = 16  is ideal
	SAFE_DELETE_ARRAY(indices);

	indices = reorderedIndices;
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
		r = gApi->CreateIndexBuffer(&ib, eUsage::DEFAULT, nIndices * sizeof(uint32_t), indices);
		if (r!=eGapiResult::OK) {
			throw std::runtime_error("failed to create index buffer");
		}
		r = gApi->CreateVertexBuffer(&vb, eUsage::DEFAULT, vertexFormat, nBytesVertices, vertices);
		if (r != eGapiResult::OK) {
			ib->Release();
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
