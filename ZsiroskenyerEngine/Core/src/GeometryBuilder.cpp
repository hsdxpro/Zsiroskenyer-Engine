#include "GeometryBuilder.h"

// DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"
#include "tipsify.h"

// Common and Math
#include "common.h"
#include "math/vec3.h"
#include "math/vec2.h"

// File IO
#include "IFile.h"

// Exception
#include "Exception.h"

cGeometryBuilder::cGeometryBuilder() {
}

cGeometryBuilder::tGeometryDesc cGeometryBuilder::LoadGeometry(const zsString& filePath) {
	Assimp::Importer importer;

	if(!IFile::isFileExits(filePath)) {
		ILog::GetInstance()->MsgBox(L"Can't open file: " + filePath);
		throw FileNotFoundException();
	}

	// read up dae scene
	char ansiFilePath[256];
	zsString::UniToAnsi(filePath, ansiFilePath, 256);
	const aiScene* scene = importer.ReadFile(ansiFilePath, (aiProcessPreset_TargetRealtime_Quality )^ aiProcess_FindInvalidData );
	if(scene == NULL) {
		ILog::GetInstance()->MsgBox(L"Can found 3D model: " + filePath);
		throw FileNotFoundException();
	}

	size_t nVertices = 0;
	size_t nIndex = 0;
	size_t nMeshes = scene->mNumMeshes;
	aiMesh** meshes = scene->mMeshes;

	// count indices, vertices
	for(size_t i = 0; i < nMeshes; i++) {
		aiMesh *mesh = meshes[i];
		nVertices += mesh->mNumVertices;
		nIndex += mesh->mNumFaces * 3;
	}

	// DEFINE VERTEX STRUCTURE HERE.... @TODO REMOVE IT OR I KILL MYSELF
	struct baseVertex {
		Vec3 pos;
		Vec3 normal;
		Vec3 tangent;
		Vec2 tex;
		bool operator == (const baseVertex& v) {return pos == v.pos && normal == v.normal;}
	};

	// Geometry read up
	baseVertex *vertices= new baseVertex[nVertices];
	unsigned *indices = new unsigned[nIndex];

	// Super TMP Vec3 for usage
	aiVector3D* supTmpVec;

	size_t indexI = 0;
	size_t vertexI = 0;
	size_t vertexOffset = 0;
	for(size_t i = 0; i < nMeshes;i++) {
		aiMesh* mesh = meshes[i];

		// Indices
		aiFace* faces = mesh->mFaces;
		size_t nFaces = mesh->mNumFaces;
		for(size_t j = 0; j < nFaces; indexI += 3, j++) {
			aiFace& face = faces[j];
			indices[indexI]		= face.mIndices[0] + vertexI;
			indices[indexI + 1]	= face.mIndices[1] + vertexI;
			indices[indexI + 2]	= face.mIndices[2] + vertexI;
		}

		// Vertices
		for(size_t j = 0; j < mesh->mNumVertices; vertexI++, j++) {
			
			supTmpVec = &mesh->mNormals[j];
			if (mesh->HasPositions()) {
				supTmpVec = &mesh->mVertices[j];
				vertices[vertexI].pos = Vec3(supTmpVec->x, supTmpVec->z, supTmpVec->y);
			}

			if (mesh->HasNormals()) {
				supTmpVec = &mesh->mNormals[j];
				vertices[vertexI].normal = Vec3(supTmpVec->x, supTmpVec->z, supTmpVec->y);
			}

			if (mesh->HasTangentsAndBitangents()) {
				supTmpVec = &mesh->mTangents[j];
				vertices[vertexI].tangent = Vec3(supTmpVec->x, supTmpVec->z, supTmpVec->y);
			}
			
			// @TODO not general algorithm, wee need to handle more UV channels
			if (mesh->HasTextureCoords(0)) {
				supTmpVec = &mesh->mTextureCoords[0][j];
				vertices[vertexI].tex = Vec2(supTmpVec->x, supTmpVec->y);
			}			
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
		geomDesc.vertexStride = sizeof(baseVertex);
		geomDesc.indexStride = sizeof(unsigned);
	return geomDesc;
}