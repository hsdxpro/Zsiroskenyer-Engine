////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Stores the description of a multi-material.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../GraphicsApi/src/ITexture2D.h"

class cMaterial {
public:
	cMaterial();

	void SetTextureDiffuse(ITexture2D* t);
	void SetTextureNormal(ITexture2D* t);
	void SetTextureSpecular(ITexture2D* t);
	void SetTextureDisplacement(ITexture2D* t);

	ITexture2D* GetTextureDiffuse();
	ITexture2D* GetTextureNormal();
	ITexture2D* GetTextureSpecular();
	ITexture2D* GetTextureDisplacement();

private:
	ITexture2D* textureDiffuse;
	ITexture2D* textureNormal;
	ITexture2D* textureSpecular;
	ITexture2D* textureDisplacement;
};