////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Material.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Material.h"

cMaterial::cMaterial()
:textureDiffuse(0), textureNormal(0), textureSpecular(0), textureDisplacement(0) {

}

void cMaterial::SetTextureDiffuse(ITexture2D* t) {
	textureDiffuse = t;
}

void cMaterial::SetTextureNormal(ITexture2D* t) {
	textureNormal = t;
}

void cMaterial::SetTextureSpecular(ITexture2D* t) {
	textureSpecular = t;
}

void cMaterial::SetTextureDisplacement(ITexture2D* t) {
	textureDisplacement = t;
}

ITexture2D* cMaterial::GetTextureDiffuse() {
	return textureDiffuse;
}

ITexture2D* cMaterial::GetTextureNormal() {
	return textureNormal;
}

ITexture2D*cMaterial::GetTextureSpecular() {
	return textureSpecular;
}

ITexture2D* cMaterial::GetTextureDisplacement() {
	return textureDisplacement;
}