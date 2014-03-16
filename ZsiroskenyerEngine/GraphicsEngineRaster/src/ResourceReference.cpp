////////////////////////////////////////////////////////////////////////////////
//	References to resources

#include "ResourceReference.h"
#include "ResourceManager.h"
#include "../../Core/src/ITexture2D.h"
#include "Material.h"
#include "Geometry.h"


// geometry reference
cGeometryRef::cGeometryRef(cResourceManager* rm, cGeometry* ptr)
:
zs_shared_ptr(ptr, [this](cGeometry* g){if (cResourceManager::IsValid(this->rm)) this->rm->UnloadGeometry(g); }),
rm(rm)
{
}
cGeometryRef::cGeometryRef(const cGeometryRef& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cGeometryRef::cGeometryRef(cGeometryRef&& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cGeometryRef::cGeometryRef()
: zs_shared_ptr(nullptr), rm(nullptr) {
}

cGeometryRef& cGeometryRef::operator = (const cGeometryRef& other) {
	zs_shared_ptr<cGeometry>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cGeometryRef::operator == (const cGeometryRef& other) {
	return ::operator==(*this, other);
}

cGeometry* cGeometryRef::get() const {
	return zs_shared_ptr<cGeometry>::get();
}


// material reference
cMaterialRef::cMaterialRef(cResourceManager* rm, cMaterial* ptr)
:
zs_shared_ptr(ptr, [this](cMaterial* m){if (cResourceManager::IsValid(this->rm)) this->rm->UnloadMaterial(m); }),
rm(rm)
{
}

cMaterialRef::cMaterialRef(const cMaterialRef& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cMaterialRef::cMaterialRef(cMaterialRef&& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cMaterialRef::cMaterialRef()
: zs_shared_ptr(nullptr), rm(nullptr) {
}

cMaterialRef& cMaterialRef::operator = (const cMaterialRef& other) {
	zs_shared_ptr<cMaterial>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cMaterialRef::operator == (const cMaterialRef& other) {
	return ::operator==(*this, other);
}

cMaterial* cMaterialRef::get() const {
	return zs_shared_ptr<cMaterial>::get();
}


// texture reference
cTextureRef::cTextureRef(cResourceManager* rm, ITexture2D* ptr)
:
zs_shared_ptr(ptr, [this](ITexture2D* t){if (cResourceManager::IsValid(this->rm)) this->rm->UnloadTexture(t); }),
rm(rm)
{
}

cTextureRef::cTextureRef(const cTextureRef& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cTextureRef::cTextureRef(cTextureRef&& other)
: zs_shared_ptr(other), rm(other.rm) {
}
cTextureRef::cTextureRef()
: zs_shared_ptr(nullptr), rm(nullptr) {
}

cTextureRef& cTextureRef::operator = (const cTextureRef& other) {
	zs_shared_ptr<ITexture2D>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cTextureRef::operator == (const cTextureRef& other) {
	return ::operator==(*this, other);
}

ITexture2D* cTextureRef::get() const {
	return zs_shared_ptr<ITexture2D>::get();
}