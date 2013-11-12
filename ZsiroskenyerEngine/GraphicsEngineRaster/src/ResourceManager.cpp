////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "ResourceManager.h"

// Geometry building
#include "..\..\Core\src\GeometryBuilder.h"

// Graphics api
#include "..\..\Core\src\IGraphicsApi.h"
#include "..\..\Core\src\IFile.h"

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager


//	Constructors
cResourceManager::cResourceManager(IGraphicsApi* gApi) : gApi(gApi) {
	validManagers.insert(this);
}
cResourceManager::~cResourceManager() {
	// release all geometries
	for (auto& it : geometries) {
		delete it.right;
	}
	// release all materials
	for (auto& it : materials) {
		delete it.right;
	}
	// release all textures
	for (auto& it : textures) {
		it.right->Release();
	}
	
	validManagers.erase(this);
}



//	Load/unload geometries
cGeometryRef cResourceManager::GetGeometry(const zsString& filePath) {
	cGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(filePath);
	if (it == geometries.left.end()) {
		cGeometryBuilder b;
		cGeometryBuilder::tGeometryDesc d = b.LoadGeometry(filePath);

		IVertexBuffer *VB = gApi->CreateVertexBuffer(d.nVertices, d.vertexStride, eBufferUsage::IMMUTABLE, d.vertices);
		IIndexBuffer *IB = gApi->CreateIndexBuffer(d.nIndices * d.indexStride, eBufferUsage::IMMUTABLE, d.indices);
		geom = new cGeometry(VB, IB);
		
		// insert into database
		geometries.insert(GeometryMapT::value_type(filePath, geom));
	}
	else {
		geom = it->second;
	}

	return cGeometryRef(this, geom);
}
void cResourceManager::UnloadGeometry(const cGeometry* geometry) {
	auto it = geometries.right.find(const_cast<cGeometry*>(geometry));
	if (it != geometries.right.end()) {
		delete it->first;
		geometries.right.erase(it);
	}
}

//	Load/unload materials
cMaterialRef cResourceManager::GetMaterial(const zsString& filePath) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(filePath);
	if (it == materials.left.end()) {
		// Open material file
		IFile* file = IFile::Create(filePath);

		// Number of subMaterials
		size_t nSubMaterials = file->GetNLines() / 8;

		// create material with nSubMaterials subMaterials
		mtl = new cMaterial(nSubMaterials);
		
		for(size_t i = 0; i < nSubMaterials; i++) {
			// subMaterial ID, not used yet
			const zsString& idLine = file->GetLine();

			std::vector<float> floats;
			// Diffuse
			file->GetLine().GetFloats(floats);
			(*mtl)[i].diffuse = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Specular
			file->GetLine().GetFloats(floats);
			(*mtl)[i].specular = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Emissive
			file->GetLine().GetFloats(floats);
			(*mtl)[i].emissive = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Glossiness
			file->GetLine().GetFloats(floats);
			(*mtl)[i].glossiness = floats[0];
			
			// Texture Diffuse
			(*mtl)[i].textureDiffuse = GetTexture(file->GetLine().c_str() + 9);

			// Texture Normal
			(*mtl)[i].textureNormal = GetTexture(file->GetLine().c_str() + 8);

			// Texture Specular
			(*mtl)[i].textureSpecular = GetTexture(file->GetLine().c_str() + 10);

			// Texture Displace
			(*mtl)[i].textureDisplace = GetTexture(file->GetLine().c_str() + 10);
			
		}

		// insert into database
		materials.insert(MaterialMapT::value_type(filePath, mtl));
	}
	else {
		mtl = it->second;
	}
	
	return cMaterialRef(this, mtl);
}

void cResourceManager::UnloadMaterial(const cMaterial* material) {
	auto it = materials.right.find(const_cast<cMaterial*>(material));
	if (it != materials.right.end()) {
		delete it->first;
		materials.right.erase(it);
	}
}

//	Load/Unload textures
cTextureRef cResourceManager::GetTexture(const zsString& filePath) {
	ITexture2D* texture;

	// lookup if already exists
	auto it = textures.left.find(filePath);
	if (it == textures.left.end()) {
		// create texture
		texture = gApi->CreateTexture(filePath.c_str());
		if (texture == nullptr) {
			return cTextureRef(this, nullptr);
		}

		// insert into database
		textures.insert(TextureMapT::value_type(filePath, texture));
	}
	else {
		texture = it->second;
	}

	return cTextureRef(this, texture);
}

void cResourceManager::UnloadTexture(const ITexture2D* texture) {
	auto it = textures.right.find(const_cast<ITexture2D*>(texture));
	if (it != textures.right.end()) {
		it->first->Release();
		textures.right.erase(it);
	}
}



// register and unregister managers to see if they are valid
bool cResourceManager::IsValid(cResourceManager* rm) {
	auto it = validManagers.find(rm);
	return it != validManagers.end();
}
std::unordered_set<cResourceManager*> cResourceManager::validManagers;