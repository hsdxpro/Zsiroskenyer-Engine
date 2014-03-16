////////////////////////////////////////////////////////////////////////////////
//	References to resources

#pragma once

#include "../../Core/src/shared_ptr.h"
#include "../../Core/src/zsString.h"

class cGeometry;
class cMaterial;
class ITexture2D;

class cResourceManager;


// geometry reference
class cGeometryRef : public zs_shared_ptr<cGeometry> {
	friend struct std::hash<cGeometryRef>;
public:
	cGeometryRef();
	cGeometryRef(cResourceManager* rm, cGeometry* ptr = nullptr);
	cGeometryRef(const cGeometryRef& other);
	cGeometryRef(cGeometryRef&& other);

	cGeometryRef& operator = (const cGeometryRef& other);

	bool operator == (const cGeometryRef& other);
	cGeometry* get() const;
private:
	cResourceManager* rm;	// reference to the 'owner'
};

// material reference
class cMaterialRef : public zs_shared_ptr<cMaterial> {
	friend struct std::hash<cMaterialRef>;
public:
	cMaterialRef();
	cMaterialRef(cResourceManager* rm, cMaterial* ptr = nullptr);
	cMaterialRef(const cMaterialRef& other);
	cMaterialRef(cMaterialRef&& other);

	cMaterialRef& operator = (const cMaterialRef& other);

	bool operator == (const cMaterialRef& other);
	cMaterial* get() const;
private:
	cResourceManager* rm;	// reference to the 'owner'
};

// texture reference
class cTextureRef : public zs_shared_ptr<ITexture2D> {
	friend struct std::hash<cTextureRef>;
public:
	cTextureRef();
	cTextureRef(cResourceManager* rm, ITexture2D* ptr = nullptr);
	cTextureRef(const cTextureRef& other);
	cTextureRef(cTextureRef&& other);

	cTextureRef& operator = (const cTextureRef& other);

	bool operator == (const cTextureRef& other);
	ITexture2D* get() const;
private:
	cResourceManager* rm;	// reference to the 'owner'
};


// hashers
template <>
struct std::hash<cGeometryRef> {
	std::size_t operator()(const cGeometryRef& g) {
		return std::hash<cGeometry*>()(g.get());
	}
};

template <>
struct std::hash<cMaterialRef> {
	std::size_t operator()(const cMaterialRef& m) {
		return std::hash<cMaterial*>()(m.get());
	}
};

template <>
struct std::hash<cTextureRef> {
	std::size_t operator()(const cTextureRef& t) {
		return std::hash<ITexture2D*>()(t.get());
	}
};
