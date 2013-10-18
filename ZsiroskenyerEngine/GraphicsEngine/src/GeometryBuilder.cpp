#include "GeometryBuilder.h"

// DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"
#include "../../CommonLib/src/tipsify.h"

// Common and Math
#include "../../CommonLib/src/common.h"
#include "../../CommonLib/src/math/vec3.h"

// File IO
#include "../../CommonLib/src/FileWin32.h"

// Exception
#include "../../GraphicsCommon/src/Exception.h"


cGeometryBuilder::tGeometryDesc cGeometryBuilder::LoadGeometryDAE(const zsString& fileName) {
	Assimp::Importer importer;

	if(!cFileWin32::isFileExits(fileName)) {
		throw FileNotFoundException();
	}

	// read up dae scene
	char ansiFileName[256];
	wcstombs(ansiFileName, fileName.c_str(), 256);
	const aiScene* scene = importer.ReadFile(ansiFileName, (aiProcessPreset_TargetRealtime_Quality | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder)^aiProcess_FindInvalidData);
	if(scene == NULL)
		throw FileNotFoundException();

	int nVertices = 0;
	int nIndex = 0;
	int nMeshes = scene->mNumMeshes;
	aiMesh** meshes = scene->mMeshes;

	// count indices, vertices
	for(size_t i = 0; i < nMeshes; i++) {
		aiMesh *mesh = meshes[i];
		nVertices += mesh->mNumVertices;
		nIndex += mesh->mNumFaces * 3;
	}

	struct baseVertex {
		Vec3 pos;
		Vec3 normal;
		bool operator == (const baseVertex& v) {return pos == v.pos && normal == v.normal;}
	};

	// Geometry read up
	baseVertex *vertices= new baseVertex[nVertices];
	unsigned *indices = new unsigned[nIndex];
	int indexI = 0;
	int vertexI = 0;
	int vertexOffset = 0;
	for(size_t i = 0; i < nMeshes;i++) {
		aiMesh* mesh = meshes[i];
		aiVector3D* currVertices	= mesh->mVertices;
		aiVector3D* currNormals		= mesh->mNormals;
		//aiVector3D* currTangents	= mesh->mTangents;
		//aiVector3D* currTxcoords	= mesh->mTextureCoords[0]; // @TODO not general algorithm, wee need to handle more UV channels

		// Indices
		aiFace* faces = mesh->mFaces;
		int nFaces = mesh->mNumFaces;
		for(size_t j = 0; j < nFaces; indexI += 3, j++) {
			aiFace& face = faces[j];
			indices[indexI]		= face.mIndices[0] + vertexI;
			indices[indexI + 1]	= face.mIndices[1] + vertexI;
			indices[indexI + 2]	= face.mIndices[2] + vertexI;
		}

		// Vertices
		for(size_t j = 0; j < mesh->mNumVertices; vertexI++, j++) {
			aiVector3D pos = currVertices[j];
			aiVector3D norm = currNormals[j];
			vertices[vertexI].pos = Vec3(pos.x, pos.y, pos.z);
			vertices[vertexI].normal = Vec3(norm.x, norm.y, norm.z);
		}
	}

	// Finally Index reordering for optimal post vertex cache
	size_t* reorderedIndices = new size_t[nIndex];
	reorderedIndices = tipsify(indices, nIndex / 3, nVertices, 16); // for vertexCache size 16
	delete[] indices;

	// return geometric description about loaded DAE
	cGeometryBuilder::tGeometryDesc geomDesc;
		geomDesc.vertices = vertices;
		geomDesc.indices = reorderedIndices;
		geomDesc.nVertices = nVertices;
		geomDesc.nIndices = nIndex;
		geomDesc.vertexStride = sizeof(float) * 6;
		geomDesc.indexStride= sizeof(unsigned);
	return geomDesc;
}