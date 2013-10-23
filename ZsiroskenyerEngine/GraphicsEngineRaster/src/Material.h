////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Stores the description of a multi-material.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../GraphicsApi/src/ITexture2D.h"
#include "../../Common/src/math/Vec4.h"

class cMaterial {
public:
	cMaterial();
	cMaterial(size_t nSubMaterials);
	~cMaterial();
	
	void SetSize(size_t nSubMaterials);

	struct tSubMaterial {
		ITexture2D* textureDiffuse;
		ITexture2D* textureNormal;
		ITexture2D* textureSpecular;
		ITexture2D* textureDisplace;
		Vec4 diffuse;
		Vec4 specular;
		Vec4 emissive;
		tSubMaterial();
	};

	tSubMaterial& operator[](size_t idx);
	const tSubMaterial& operator[](size_t idx) const;
	size_t GetSize() const;

private:
	tSubMaterial* subMaterials;
	size_t size_;
};