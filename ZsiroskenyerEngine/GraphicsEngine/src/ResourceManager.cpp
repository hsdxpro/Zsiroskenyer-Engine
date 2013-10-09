////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////


#include "ResourceManager.h"



using namespace std;




////////////////////////////////////////////////////////////////////////////////
//	ResourceManager

// load/unload geometries
cGeometryRef cResourceManager::LoadGeometry(string fileName) {
	return cGeometryRef(this);
}
void cResourceManager::UnloadGeometry(const cGeometry* geometry) {
}

// load/unload materials
cMaterialRef cResourceManager::LoadMaterial(string fileName) {
	return cMaterialRef(this);
}
void cResourceManager::UnloadMaterial(const cMaterial* material) {
}




////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
cGeometryRef::cGeometryRef(cResourceManager* rm, cGeometry* ptr)
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
	return ::operator==(*this, other);
}

cMaterial* cMaterialRef::get() const {
	return shared_ptr<cMaterial>::get();
}