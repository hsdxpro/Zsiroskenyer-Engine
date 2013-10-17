////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "ManagerResource.h"

// .DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"

// Vertex cache optimisation
#include "../../CommonLib/src/tipsify.h"

// Common and math
#include "../../CommonLib/src/common.h"
#include "../../CommonLib/src/math/vec3.h"

// Graphics api
#include "../../GraphicsCommon/src/IGraphicsApi.h"
#include "../../CommonLib/src/FileWin32.h"

#include "../../GraphicsCommon/src/Exception.h"

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager

// load/unload geometries
cGeometryRef cManagerResource::LoadGeometry(const zsString& fileName) {
	cGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(fileName);
	if (it==geometries.left.end()) {

		// Create geometry based on file extension
		zsString fileExtension = fileName.substr(fileName.length() - 3, 3);
		if(fileExtension == L"dae") {
			geom = LoadGeometryDAE(fileName);
		}
		

		// insert into database
		geometries.insert(GeometryMapT::value_type(fileName, geom));
	}
	else {
		geom = it->second;
	}

	return cGeometryRef(this, geom);
}
void cManagerResource::UnloadGeometry(const cGeometry* geometry) {
	auto it = geometries.right.find(const_cast<cGeometry*>(geometry));
	delete it->first;
	geometries.right.erase(it);
}

cGeometry* cManagerResource::LoadGeometryDAE(const zsString& fileName) {
	Assimp::Importer importer;

	if(!cFileWin32::isFileExits(fileName)) {
		throw FileNotFoundException();
		//ZS_MSG(zsString(L"Can't load DAE file: " + fileName).c_str());
	}
	
	// read up dae scene
	char ansiFileName[256];
	wcstombs(ansiFileName, fileName.c_str(), 256);
	const aiScene* scene = importer.ReadFile(ansiFileName, (aiProcessPreset_TargetRealtime_Quality | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder)^aiProcess_FindInvalidData);
	if(scene == NULL)
		return NULL;

	int nVertex = 0;
	int nIndex = 0;
	int nMeshes = scene->mNumMeshes;
	aiMesh** meshes = scene->mMeshes;

	for(uint32 i = 0; i < nMeshes; i++) {
		aiMesh *mesh = meshes[i];
		nVertex += mesh->mNumVertices;
		nIndex += mesh->mNumFaces * 3;
	}

	// read up vertex + index
	struct baseVertex {
		Vec3 pos;
		Vec3 normal;
		bool operator == (const baseVertex& v) {return pos == v.pos && normal == v.normal;}
	};

	baseVertex *vertices= new baseVertex[nVertex];
	unsigned *indices = new unsigned[nIndex];
	int indexI = 0;
	int vertexI = 0;
	int vertexOffset = 0;
	for(uint32 i = 0; i < nMeshes;i++) {
		aiMesh* mesh = meshes[i];
		aiVector3D* currVertices	= mesh->mVertices;
		aiVector3D* currNormals		= mesh->mNormals;
		//aiVector3D* currTangents	= mesh->mTangents;
		//aiVector3D* currTxcoords	= mesh->mTextureCoords[0]; // @TODO not general algorithm, wee need to handle more UV channels


		// read indices
		aiFace* faces = mesh->mFaces;
		int nFaces = mesh->mNumFaces;
		for(uint32 j = 0; j < nFaces; indexI += 3, j++) {
			aiFace& face = faces[j];
			indices[indexI]		= face.mIndices[0] + vertexI; // each mesh start indexing from 0 in .dae
			indices[indexI + 1]	= face.mIndices[1] + vertexI;
			indices[indexI + 2]	= face.mIndices[2] + vertexI;
		}

		// Ver
		for(uint32 j = 0; j < mesh->mNumVertices; vertexI++, j++) {
			aiVector3D pos = currVertices[j];
			aiVector3D norm = currNormals[j];
			vertices[vertexI].pos = Vec3(pos.x, pos.y, pos.z);
			vertices[vertexI].normal = Vec3(norm.x, norm.y, norm.z);
		}
	}

	// Finally Index reordering for optimal post vertex cache
	size_t* reorderedIndices = new size_t[nIndex];
	reorderedIndices = tipsify(indices,nIndex / 3,nVertex, 16); // for vertexCache size 16
	delete[] indices;

	zsPrintDebug(L"ManagerResource::LoadGeometry -> " + fileName);
	size_t vertexStride = sizeof(float) * 6;
	size_t indexStride = sizeof(unsigned);

	IVertexBuffer *vb = gApi->CreateVertexBuffer(nVertex, vertexStride, eBufferUsage::IMMUTABLE, vertices);
	IIndexBuffer *ib = gApi->CreateIndexBuffer(nIndex * indexStride, eBufferUsage::IMMUTABLE, reorderedIndices);
	return new cGeometry(vb, ib);
}

// load/unload materials
cMaterialRef cManagerResource::LoadMaterial(const zsString& fileName) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(fileName);
	if (it==materials.left.end()) {
		mtl = new cMaterial;
		// try to load material

		// TODO: add loading code here!
		// throw a FileNotFound or an InvalidData exception on failure

		// insert into database
		materials.insert(MaterialMapT::value_type(fileName, mtl));
	}
	else {
		mtl = it->second;
	}
	
	return cMaterialRef(this, mtl);
}
void cManagerResource::UnloadMaterial(const cMaterial* material) {
	auto it = materials.right.find(const_cast<cMaterial*>(material));
	delete it->first;
	materials.right.erase(it);
}


// constructors
cManagerResource::cManagerResource(IGraphicsApi* gApi) : gApi(gApi) {
}
cManagerResource::~cManagerResource() {
}


////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
cGeometryRef::cGeometryRef(cManagerResource* rm, cGeometry* ptr)
	:
	shared_ptr(ptr, [this](cGeometry* g){this->rm->UnloadGeometry(g);} ), 
	rm(rm)
{
}
cGeometryRef::cGeometryRef(const cGeometryRef& other) 
	:
	shared_ptr(other),
	rm(other.rm)
{
}
cGeometryRef::cGeometryRef()
	:
	shared_ptr(nullptr),
	rm(nullptr)
{
}

cGeometryRef& cGeometryRef::operator=(const cGeometryRef& other) {
	shared_ptr<cGeometry>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cGeometryRef::operator==(const cGeometryRef& other) {
	return std::operator==(*this, other);
}

cGeometry* cGeometryRef::get() const {
	return shared_ptr<cGeometry>::get();
}



// material reference
cMaterialRef::cMaterialRef(cManagerResource* rm, cMaterial* ptr)
	:
	shared_ptr(ptr, [this](cMaterial* m){this->rm->UnloadMaterial(m);} ), 
	rm(rm)
{
}
cMaterialRef::cMaterialRef(const cMaterialRef& other) 
	:
	shared_ptr(other),
	rm(other.rm)
{
}
cMaterialRef::cMaterialRef()
	:
	shared_ptr(nullptr),
	rm(nullptr)
{
}

cMaterialRef& cMaterialRef::operator=(const cMaterialRef& other) {
	shared_ptr<cMaterial>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cMaterialRef::operator==(const cMaterialRef& other) {
	return std::operator==(*this, other);
}

cMaterial* cMaterialRef::get() const {
	return shared_ptr<cMaterial>::get();
}