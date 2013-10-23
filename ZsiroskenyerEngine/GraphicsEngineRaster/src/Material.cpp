////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Material.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Material.h"
#include <stdexcept>

#include "../../Common/src/common.h"

// constructor
cMaterial::cMaterial() 
:subMaterials(NULL), size_(0) {
}

cMaterial::cMaterial(size_t nSubMaterials)
:size_(nSubMaterials) {
	subMaterials = new tSubMaterial[nSubMaterials];
}

cMaterial::~cMaterial() {
	for (size_t i=0; i < size_; i++) {
		SAFE_RELEASE(subMaterials[i].textureDiffuse);
		SAFE_RELEASE(subMaterials[i].textureNormal);
		SAFE_RELEASE(subMaterials[i].textureSpecular);
		SAFE_RELEASE(subMaterials[i].textureDisplace);
	}
	SAFE_DELETE_ARRAY(subMaterials);
}

cMaterial::tSubMaterial::tSubMaterial() 
:	textureDiffuse(NULL),
	textureDisplace(NULL),
	textureNormal(NULL),
	textureSpecular(NULL),
	diffuse(0,0,0,1),
	emissive(0,0,0,1),
	specular(0,0,0,1)
{
}

// bullshit
void cMaterial::SetSize(size_t nSubMaterials) {
	this->~cMaterial(); // ugly but i don't want to copy-paste
	new (this) cMaterial(nSubMaterials); // ugly too...
}


cMaterial::tSubMaterial& cMaterial::operator[](size_t idx) {
	if (idx>=size_)
		throw std::out_of_range("no such material id");
	return subMaterials[idx];
}
const cMaterial::tSubMaterial& cMaterial::operator[](size_t idx) const {
	if (idx>=size_)
		throw std::out_of_range("no such material id");
	return subMaterials[idx];
}

size_t cMaterial::GetSize() const {
	return size_;
}



