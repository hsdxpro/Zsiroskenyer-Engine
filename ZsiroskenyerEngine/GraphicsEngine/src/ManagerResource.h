////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/RsourceManager.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	The ResourceManager class is responsible for loading and (automatic)
//	unloading of graphical resources, such as geometries, textures and 
//	materials.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <memory>

#include "..\..\CommonLib\src\zsString.h"
#include "Geometry.h"
#include "Material.h"

#include <boost\bimap.hpp>
#include <boost\bimap\unordered_set_of.hpp>



////////////////////////////////////////////////////////////////////////////////
//	ResourceManager
class cManagerResource {
	friend class cGeometryRef;
	friend class cMaterialRef;
	friend class cTextureRef;
public:
	// constructor
	cManagerResource();
	~cManagerResource();

	// resource aquisition
	cGeometryRef LoadGeometry(const zsString& fileName);
	cMaterialRef LoadMaterial(const zsString& fileName);

private:
	// automatic resource unloading requested by references
	void UnloadGeometry(const cGeometry* geometry);
	void UnloadMaterial(const cMaterial* material);

	// Loading .DAE geometry with Assimp 3d library
	cGeometry *LoadGeometryDAE(const zsString& fileName);

	// resource database
	typedef boost::bimap<boost::bimaps::unordered_set_of<const zsString>, boost::bimaps::unordered_set_of<cGeometry*>> GeometryMapT;
	typedef boost::bimap<boost::bimaps::unordered_set_of<const zsString>, boost::bimaps::unordered_set_of<cMaterial*>> MaterialMapT;

	GeometryMapT geometries;
	MaterialMapT materials;
};




////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
class cGeometryRef : public std::shared_ptr<cGeometry> {
	friend struct std::hash<cGeometryRef>;
public:
	cGeometryRef();
	cGeometryRef(cManagerResource* rm, cGeometry* ptr=nullptr);
	cGeometryRef(const cGeometryRef& other);

	cGeometryRef& operator=(const cGeometryRef& other);

	bool operator==(const cGeometryRef& other);
private:
	cGeometry* get() const;	// kill this function
	cManagerResource* rm;	// reference to the 'owner'
};

// material reference
class cMaterialRef : public std::shared_ptr<cMaterial> {
	friend struct std::hash<cMaterialRef>;
public:
	cMaterialRef();
	cMaterialRef(cManagerResource* rm, cMaterial* ptr=nullptr);
	cMaterialRef(const cMaterialRef& other);

	cMaterialRef& operator=(const cMaterialRef& other);

	bool operator==(const cMaterialRef& other);
private:
	cMaterial* get() const;	// kill this function
	cManagerResource* rm;	// reference to the 'owner'
};


// hashers
template <>
struct std::hash<cGeometryRef> {
	typedef cGeometryRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cGeometryRef& g) {
		return std::hash<cGeometry*>()(g.get());
	}
};

template <>
struct std::hash<cMaterialRef> {
	typedef cMaterialRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cMaterialRef& m) {
		return std::hash<cMaterial*>()(m.get());
	}
};