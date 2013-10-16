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


using namespace std;



////////////////////////////////////////////////////////////////////////////////
//	ResourceManager

// load/unload geometries
cGeometryRef cManagerResource::LoadGeometry(const zsString& fileName) {
	cGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(fileName);
	if (it==geometries.left.end()) {

		// try to load geometry
		// TODO: add loading code here!
		// throw a FileNotFound or an InvalidData exception on failure

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
	//tVertexLayouts::tVertexNro::vertex* vertices = new tVertexLayouts::tVertexNro::vertex[nVertex];
	Vec3 *verticesPos = new Vec3[nVertex];
	uint32 *indices = new uint32[nIndex];
	int indexI = 0;
	int vertexI = 0;
	int vertexOffset = 0;
	for(uint32 i = 0; i < nMeshes;i++) {
		aiMesh* mesh = meshes[i];
		aiVector3D* currVertices	= mesh->mVertices;
		//aiVector3D* currNormals		= mesh->mNormals;
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

		// read vertex transforms
		aiNode *root = scene->mRootNode;
		bool meshHaveBakedTrans = true;
		if(strcmp(root->mName.C_Str(),"RootNode") == 0)
			meshHaveBakedTrans = false;

		aiVector3D scaling(1.0f, 1.0f, 1.0f);
		aiQuaternion rotation(1.0f, 0.0f, 0.0f, 0.0f);
		aiVector3D position(0.0f, 0.0f, 0.0f);

		if(!meshHaveBakedTrans) {
			aiMatrix4x4 meshTrans;
			aiNode *meshTransNode = scene->mRootNode->mChildren[i];
			if(scene->mRootNode->mNumChildren > i) {
				meshTrans *= meshTransNode->mTransformation;
				if(meshTransNode->mNumChildren > 0)
					meshTrans *= meshTransNode->mChildren[0]->mTransformation;
			}
			meshTrans.Decompose(scaling, rotation, position);
			position.y *= -1;
			swap(position.y, position.z);
		} else {
			aiNode *rootNode = scene->mRootNode;
			aiNode *meshTransNode = (scene->mRootNode->mNumChildren > 1) ? rootNode->mChildren[1] : rootNode->mChildren[0];

			aiMatrix4x4 meshTrans = meshTransNode->mTransformation;
			if(meshTransNode->mNumChildren >0) {
				meshTrans *= meshTransNode->mChildren[0]->mTransformation;
				if(meshTransNode->mChildren[0]->mNumChildren > 0)
					meshTrans *= meshTransNode->mChildren[0]->mChildren[0]->mTransformation;
			}
			
			meshTrans.Decompose(scaling, rotation, position);
		}

		

		// process transform for vertices, normals, tangents, read them
		for(uint32 j = 0; j < mesh->mNumVertices; vertexI++, j++) {
			aiVector3D pos = rotation.Rotate(currVertices[j] + position).SymMul(scaling);

			/*aiVector3D norm	= rotation.Rotate(currNormals[j]);
			aiVector3D tangent	= rotation.Rotate(currTangents[j]);
			aiVector3D txcoord	= currTxcoords[j];*/
			
			if(meshHaveBakedTrans) {
				swap(pos.y, pos.z);
				// x, z, y  stupid assimp
			}

			if(!meshHaveBakedTrans) {
				pos.y *= -1; // I really don't know why need that
			}
		

			verticesPos[vertexI] = Vec3(pos.x, pos.y, pos.z);

			//if(bakedRotation) {
				//verticesPos[vertexI] = eiFloat3(pos.x, pos.z, pos.y);
			//} else {
				//verticesPos[vertexI] = eiFloat3(pos.x, -pos.y, pos.z);
			//}

			/*verticesPos[vertexI]		= eiFloat3(currVertices[j].x, currVertices[j].y, currVertices[j].z);
			vertices[vertexI].normalL	= XMFLOAT3(norm.x, -norm.z, norm.y);
			vertices[vertexI].tangentL	= XMFLOAT3(tangent.x, -tangent.z, tangent.y);
 			vertices[vertexI].tex0		= XMFLOAT2(txcoord.x, txcoord.y);*/
		}
	}
	
	// drop non unique vertices
	Vec3* uniqueVertices = new Vec3[nVertex];
	int nUniqVertices = 0;
	for(uint32 i = 0; i < nVertex; i++) {
		Vec3& v = verticesPos[i];

		bool uniq = true;
		int uniqIndex = -1;
		// check vertex uniquness
		for(uint32 j = 0; j < nUniqVertices; j++) {
			if(uniqueVertices[j] == v) {
				uniq = false;
				uniqIndex = j;
				break;
			}
		}

		int currIndex = nUniqVertices;
		if(uniq) {
			uniqueVertices[nUniqVertices] = v;
			nUniqVertices++;
		} else {
			currIndex = uniqIndex;
		}

		for(uint32 k = 0; k <nIndex; k++)
			if(indices[k] == i)
				indices[k] = currIndex;
	}

	// Finally Index reordering for optimal post vertex cache
	uint32* reorderedIndices = new uint32[nIndex];
	reorderedIndices = tipsify(indices, nIndex / 3,nVertex, 16);
	delete[] indices;

	zsPrintDebug(L"ManagerResource::LoadGeometry -> " + fileName);
	int vertexStride = sizeof(float) * 8;

	IVertexBuffer *vb = gApi->CreateVertexBuffer(nUniqVertices, vertexStride, eBufferUsage::DEFAULT, uniqueVertices);
	IIndexBuffer *ib = gApi->CreateIndexBuffer(nIndex * sizeof(uint32), eBufferUsage::DEFAULT, reorderedIndices);
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
	return ::operator==(*this, other);
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
	return ::operator==(*this, other);
}

cMaterial* cMaterialRef::get() const {
	return shared_ptr<cMaterial>::get();
}