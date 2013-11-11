////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/RsourceManager.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	The ResourceManager class is responsible for loading and (automatic)
//	unloading of graphical resources, such as geometries, textures and 
//	materials.
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "..\..\Core\src\GeometryBuilder.h"
#include "Geometry.h"
#include "Material.h"
#include "../../Core/src/shared_ptr.h"

#include "..\..\Core\src\zsString.h"
#include <memory>

#include <boost\bimap.hpp>
#include <boost\bimap\unordered_set_of.hpp>


class IGraphicsApi;

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager
class cResourceManager {
	friend class cGeometryRef;
	friend class cMaterialRef;
	friend class cTextureRef;
public:
	// resource aquisition
	cMaterialRef GetMaterial(const zsString& filePath);

	cGeometryRef GetGeometry(const zsString& filePath);

	// constructor
	cResourceManager(IGraphicsApi* gApi);
	~cResourceManager();
private:
	// automatic resource unloading requested by references
	void UnloadGeometry(const cGeometry* geometry);
	void UnloadMaterial(const cMaterial* material);

	// resource database
	typedef boost::bimap<boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>, boost::bimaps::unordered_set_of<cGeometry*>> GeometryMapT;
	typedef boost::bimap<boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>, boost::bimaps::unordered_set_of<cMaterial*>> MaterialMapT;

	GeometryMapT geometries;
	MaterialMapT materials;

	// graphics api
	IGraphicsApi* const gApi;
};




////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
class cGeometryRef : public zs_shared_ptr<cGeometry> {
	friend struct std::hash<cGeometryRef>;
public:
	cGeometryRef();
	cGeometryRef(cResourceManager* rm, cGeometry* ptr = NULL);
	cGeometryRef(const cGeometryRef& other);
	cGeometryRef(cGeometryRef&& other);
	
	cGeometryRef& operator = (const cGeometryRef& other);

	bool operator == (const cGeometryRef& other);
private:
	cGeometry* get() const;	// kill this function
	cResourceManager* rm;	// reference to the 'owner'
};

// material reference
class cMaterialRef : public zs_shared_ptr<cMaterial> {
	friend struct std::hash<cMaterialRef>;
public:
	cMaterialRef();
	cMaterialRef(cResourceManager* rm, cMaterial* ptr = NULL);
	cMaterialRef(const cMaterialRef& other);
	cMaterialRef(cMaterialRef&& other);

	cMaterialRef& operator = (const cMaterialRef& other);

	bool operator == (const cMaterialRef& other);
private:
	cMaterial* get() const;	// kill this function
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