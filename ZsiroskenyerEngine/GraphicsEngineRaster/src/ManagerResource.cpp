////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "ManagerResource.h"

// Geometry building
#include "GeometryBuilder.h"

// Graphics api
#include "../../GraphicsApi/src/IGraphicsApi.h"
#include "../../Common/src/IFile.h"

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
			cGeometryBuilder b;
			cGeometryBuilder::tGeometryDesc& d = b.LoadGeometryDAE(fileName);

			IVertexBuffer *VB = gApi->CreateBufferVertex(d.nVertices, d.vertexStride, eBufferUsage::IMMUTABLE, d.vertices);
			IIndexBuffer *IB = gApi->CreateBufferIndex(d.nIndices * d.indexStride, eBufferUsage::IMMUTABLE, d.indices);
			geom = new cGeometry(VB, IB);
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

// load/unload materials
cMaterialRef cManagerResource::LoadMaterial(const zsString& fileName) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(fileName);
	if (it == materials.left.end()) {
		mtl = new cMaterial;
		// try to load material

		IFile* file = IFile::Create(fileName);

		zsString buffer;
		while(!file->IsEOF()) {
			buffer = file->GetLine();
		}
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