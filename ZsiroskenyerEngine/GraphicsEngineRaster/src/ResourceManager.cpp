////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "ResourceManager.h"

// Geometry building
#include "../../Core/src/GeometryBuilder.h"

// Graphics api
#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/StrUtil.h"
#include "../../Core/src/FileUtil.h"


////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////
//	Load/unload geometries
cGeometryRef cResourceManager::GetGeometry(const zsString& filePath) {
	cGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(filePath);
	if (it == geometries.left.end()) {
		cGeometryBuilder::tGeometryDesc d = cGeometryBuilder::LoadGeometry(filePath);

		IVertexBuffer *VB;
		gApi->CreateVertexBuffer(&VB, d.nVertices * d.vertexStride, eUsage::IMMUTABLE, d.vertices);
		IIndexBuffer *IB;
		gApi->CreateIndexBuffer(&IB, d.nIndices * d.indexStride, eUsage::IMMUTABLE, d.indices);
		

		std::vector<cGeometry::tMatGroup> matGroup;

		for (cGeometryBuilder::tGeometryDesc::tMatGroup& e : d.matGroups)
			matGroup.push_back({ e.id, e.indexOffset, e.indexCount });

		geom = new cGeometry(VB, IB, &matGroup);

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

// Custom, externally generated geometries
cGeometryRef cResourceManager::AddGeometry(cGeometry* geometry, const zsString& name) {
	// check if name is taken
	auto it = geometries.left.find(name);
	if (it == geometries.left.end()) { // it's not taken
		// insert into database
		geometries.insert(GeometryMapT::value_type(name, geometry));
		return cGeometryRef(this, geometry);
	}
	else { // it's taken
		throw std::invalid_argument("name already in use");
		return cGeometryRef(this, nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
//	Load/unload materials
cMaterialRef cResourceManager::GetMaterial(const zsString& filePath) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(filePath);
	if (it == materials.left.end()) {
		// Open material file
		std::ifstream file(filePath.c_str(), std::ios::in);

		auto lines = cFileUtil::GetLines(file);

		// Number of subMaterials
		size_t nSubMaterials = lines.size() / 9;

		// create material with nSubMaterials subMaterials
		mtl = new cMaterial(nSubMaterials);
		
		// Texture path's relative to materials ;)
		zsString mtlBasePath = cStrUtil::GetDirectory(filePath);

		auto stringIt = lines.begin();

		for(size_t i = 0; i < nSubMaterials; i++) {
			// subMaterial ID, not used yet
			const zsString& idLine = *stringIt++;

			std::vector<float> floats;
			// Diffuse
			cStrUtil::GetFloats(*stringIt++, floats);
			(*mtl)[i].diffuse = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Specular
			cStrUtil::GetFloats(*stringIt++, floats);
			(*mtl)[i].specular = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Emissive
			cStrUtil::GetFloats(*stringIt++, floats);
			(*mtl)[i].emissive = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Glossiness
			cStrUtil::GetFloats(*stringIt++, floats);
			(*mtl)[i].glossiness = floats[0];
			
			// Texture Diffuse
			(*mtl)[i].textureDiffuse = GetTexture(mtlBasePath + ((*stringIt++).c_str() + 9));

			// Texture Normal
			(*mtl)[i].textureNormal = GetTexture(mtlBasePath + ((*stringIt++).c_str() + 8));

			// Texture Specular
			(*mtl)[i].textureSpecular = GetTexture(mtlBasePath + ((*stringIt++).c_str() + 10));

			// Texture Displace
			(*mtl)[i].textureDisplace = GetTexture(mtlBasePath + ((*stringIt++).c_str() + 10));
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

////////////////////////////////////////////////////////////////////////////////
//	Load/Unload textures
cTextureRef cResourceManager::GetTexture(const zsString& filePath) {
	ITexture2D* texture;

	// lookup if already exists
	auto it = textures.left.find(filePath);
	if (it == textures.left.end()) {
		// create texture
		auto result = gApi->CreateTexture(&texture, filePath.c_str());
		if (result != eGapiResult::OK) {
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


////////////////////////////////////////////////////////////////////////////////
// register and unregister managers to see if they are valid
bool cResourceManager::IsValid(cResourceManager* rm) {
	auto it = validManagers.find(rm);
	return it != validManagers.end();
}
std::unordered_set<cResourceManager*> cResourceManager::validManagers;


////////////////////////////////////////////////////////////////////////////////
// Misc
IGraphicsApi* cResourceManager::GetGraphicsApi() {
	return gApi;
}