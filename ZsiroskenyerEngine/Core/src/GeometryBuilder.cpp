#include "GeometryBuilder.h"

// DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"
#include "tipsify.h"

// Common and Math
#include "common.h"
#include "StrUtil.h"
#include "math/vec3.h"
#include "math/vec2.h"

// Exception
#include "Exception.h"
#include "FileUtil.h"

cGeometryBuilder::cGeometryBuilder() {
}

cGeometryBuilder::tGeometryDesc cGeometryBuilder::LoadGeometry(const zsString& filePath) {
	Assimp::Importer importer;

	if(!cFileUtil::isFileExits(filePath)) {
		ILog::GetInstance()->MsgBox(L"Can't open file: " + filePath);
		throw FileNotFoundException();
	}

	// read up dae scene
	char ansiFilePath[256];
	cStrUtil::ToAnsi(filePath, ansiFilePath, 256);
	const aiScene* scene = importer.ReadFile(ansiFilePath, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate /*| aiProcess_ImproveCacheLocality | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes*/ | aiProcess_FlipWindingOrder);
	if (scene == NULL) {
			ILog::GetInstance()->MsgBox(L"Can't found 3D model: " + filePath);
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
		bool operator == (const baseVertex& v) {return pos == v.pos && normal == v.normal && tangent == v.tangent && tex == v.tex;}
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
					vertices[vertIdx + vertexI].pos = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasNormals()) {
					supTmpVec = &mesh->mNormals[vertIdx];
					vertices[vertIdx + vertexI].normal = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasTangentsAndBitangents()) {
					supTmpVec = &mesh->mTangents[vertIdx];
					vertices[vertIdx + vertexI].tangent = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				// @TODO not general algorithm, wee need to handle more UV channels
				supTmpVec = &mesh->mTextureCoords[0][vertIdx];
				vertices[vertIdx + vertexI].tex = Vec2(supTmpVec->x, -supTmpVec->y); // Fucking hate Assimp
			}
		}

		vertexI += mesh->mNumVertices;
	}

	// Finally Index reordering for optimal post vertex cache
	//size_t* reorderedIndices = new size_t[nIndex];
	//reorderedIndices = tipsify(indices, nIndex / 3, nVertices, 16); // For vertexCache size 16 . I think it's ideal
	//SAFE_DELETE_ARRAY(indices);

	// Return geometric description about loaded DAE
	tGeometryDesc geomDesc;
		geomDesc.vertices = vertices;
		geomDesc.nVertices = nVertices;
		geomDesc.vertexStride = sizeof(baseVertex);

		//geomDesc.indices = reorderedIndices;
		geomDesc.indices = indices;
		geomDesc.nIndices = nIndex;
		geomDesc.indexStride = sizeof(unsigned);
	return geomDesc;
}