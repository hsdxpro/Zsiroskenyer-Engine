////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "InstanceGroup.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "InstanceGroup.h"

cInstanceGroup::cInstanceGroup() {

}

cInstanceGroup::cInstanceGroup(cGeometryRef geom, cMaterialRef mtl) 
: geom(geom), mtl(mtl) {
}

bool cInstanceGroup::operator == (const cInstanceGroup& other) {
	return geom==other.geom && mtl==other.mtl;
}

void cInstanceGroup::SetTextureDiffuse(ITexture2D* t) {
	mtl->SetTextureDiffuse(t);
}

void cInstanceGroup::SetTextureNormal(ITexture2D* t) {
	mtl->SetTextureNormal(t);
}

void cInstanceGroup::SetTextureSpecular(ITexture2D* t) {
	mtl->SetTextureSpecular(t);
}

void cInstanceGroup::SetTextureDisplacement(ITexture2D* t) {
	mtl->SetTextureDisplacement(t);
}

ITexture2D* cInstanceGroup::GetTextureDiffuse() {
	return mtl->GetTextureDiffuse();
}

ITexture2D* cInstanceGroup::GetTextureNormal() {
	return mtl->GetTextureNormal();
}

ITexture2D* cInstanceGroup::GetTextureSpecular() {
	return mtl->GetTextureSpecular();
}

ITexture2D* cInstanceGroup::GetTextureDisplacement() {
	return mtl->GetTextureDisplacement();
}