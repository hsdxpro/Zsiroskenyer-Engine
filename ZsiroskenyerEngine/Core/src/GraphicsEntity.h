////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "math/math_all.h"
#include "ITransformable3D.h"
#include "Transform3D.h"

class cInstanceGroup;
class ITexture2D;


class cGraphicsEntity : public ITransformable3D, public cTransform3D {
	friend class cSceneManager;
public:
	cGraphicsEntity();

	void SetPos(const Vec3& p) override;
	void SetPos(float x, float y, float z) override;

	void SetRot(const Quat& r) override;
	void SetRot(float x, float y, float z, float w) override;

	void SetScale(const Vec3& s) override;
	void SetScale(float x, float y, float z) override;	

	const Vec3& GetPos() override;
	const Quat& GetRot() override;
	const Vec3& GetScale() override;

	const ITexture2D* GetTextureDiffuse(size_t subMtlIdx)	const;
	const ITexture2D* GetTextureNormal(size_t subMtlIdx)	const;
	const ITexture2D* GetTextureSpecular(size_t subMtlIdx)	const;
	const ITexture2D* GetTextureDisplace(size_t subMtlIdx)	const;

protected:
	cInstanceGroup* instanceGroup;

	bool isVisible;
};