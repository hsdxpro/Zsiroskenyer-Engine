////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "ResourceManager.h"

// Geometry building
#include "GeometryBuilder.h"

// Graphics api
#include "../../Core/src/IGraphicsApi.h"
#include "../../Core/src/IFile.h"

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager

// load/unload geometries
cGraphicsGeometryRef cResourceManager::LoadGeometry(const zsString& fileName) {
	cGraphicsGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(fileName);
	if (it==geometries.left.end()) {

		// Create geometry based on file extension
		zsString fileExtension = fileName.substr(fileName.length() - 3, 3);
		if(fileExtension == L"dae") {
			cGeometryBuilder b;
			cGeometryBuilder::tGeometryDesc& d = b.LoadGeometryDAE(fileName);

			IVertexBuffer *VB = gApi->CreateBufferVertex(d.nVertices, d.vertexStride, eBufferUsage::IMMUTABLE, d.vertices);
			IIndexBuffer *IB = gApi->CreateBufferIndex(d.nIndices * d.indexStride, eBufferUsage::IMMUTABLE, d.indices);
			geom = new cGraphicsGeometry(VB, IB);
		}
		
		// insert into database
		geometries.insert(GeometryMapT::value_type(fileName, geom));
	}
	else {
		geom = it->second;
	}

	return cGraphicsGeometryRef(this, geom);
}
void cResourceManager::UnloadGeometry(const cGraphicsGeometry* geometry) {
	auto it = geometries.right.find(const_cast<cGraphicsGeometry*>(geometry));
	delete it->first;
	geometries.right.erase(it);
}

// load/unload materials
cMaterialRef cResourceManager::LoadMaterial(const zsString& fileName) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(fileName);
	if (it == materials.left.end()) {
		// Open material file
		IFile* file = IFile::Create(fileName);

		// Number of subMaterials
		size_t nSubMaterials = file->GetNLines() / 8;

		// create material with nSubMaterials subMaterials
		mtl = new cMaterial(nSubMaterials);
		
		for(size_t i = 0; i < nSubMaterials; i++) {
			// subMaterial ID, not used yet
			const zsString& idLine = file->GetLine();

			// Diffuse
			auto floats = file->GetLine().GetFloats(7);
			(*mtl)[i].diffuse = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Specular
			floats = file->GetLine().GetFloats(8);
			(*mtl)[i].specular = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Emissive
			floats = file->GetLine().GetFloats(8);
			(*mtl)[i].emissive = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Glossiness
			floats = file->GetLine().GetFloats(10);
			(*mtl)[i].glossiness = floats[0];

			// Texture Diffuse
			(*mtl)[i].textureDiffuse = gApi->CreateTexture(file->GetLine().c_str() + 9);

			// Texture Normal
			(*mtl)[i].textureNormal = gApi->CreateTexture(file->GetLine().c_str() + 8);

			// Texture Specular
			(*mtl)[i].textureSpecular = gApi->CreateTexture(file->GetLine().c_str() + 10);

			// Texture Displace
			(*mtl)[i].textureDisplace = gApi->CreateTexture(file->GetLine().c_str() + 10);
		}

		// insert into database
		materials.insert(MaterialMapT::value_type(fileName, mtl));
	}
	else {
		mtl = it->second;
	}
	
	return cMaterialRef(this, mtl);
}

void cResourceManager::UnloadMaterial(const cMaterial* material) {
	auto it = materials.right.find(const_cast<cMaterial*>(material));
	delete it->first;
	materials.right.erase(it);
}


// constructors
cResourceManager::cResourceManager(IGraphicsApi* gApi) : gApi(gApi) {
}
cResourceManager::~cResourceManager() {
}


////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
cGraphicsGeometryRef::cGraphicsGeometryRef(cResourceManager* rm, cGraphicsGeometry* ptr)
	:
	shared_ptr(ptr, [this](cGraphicsGeometry* g){this->rm->UnloadGeometry(g);} ), 
	rm(rm)
{
}
cGraphicsGeometryRef::cGraphicsGeometryRef(const cGraphicsGeometryRef& other) 
	:
	shared_ptr(other),
	rm(other.rm)
{
}
cGraphicsGeometryRef::cGraphicsGeometryRef()
	:
	shared_ptr(nullptr),
	rm(nullptr)
{
}

cGraphicsGeometryRef& cGraphicsGeometryRef::operator=(const cGraphicsGeometryRef& other) {
	shared_ptr<cGraphicsGeometry>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cGraphicsGeometryRef::operator==(const cGraphicsGeometryRef& other) {
	return std::operator==(*this, other);
}

cGraphicsGeometry* cGraphicsGeometryRef::get() const {
	return shared_ptr<cGraphicsGeometry>::get();
}



// material reference
cMaterialRef::cMaterialRef(cResourceManager* rm, cMaterial* ptr)
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