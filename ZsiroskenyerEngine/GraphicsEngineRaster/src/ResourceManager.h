////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/RsourceManager.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	The ResourceManager class is responsible for loading and (automatic)
//	unloading of graphical resources, such as geometries, textures and 
//	materials.
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include "../../Core/src/GeometryBuilder.h"
#include "../../Core/src/shared_ptr.h"
#include "../../Core/src/zsString.h"
#include "Geometry.h"
#include "Material.h"
#include "ResourceReference.h"

#include <memory>
#include <unordered_set>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>


class IGraphicsApi;
class ITexture2D;

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager
class cResourceManager {
	friend class cGeometryRef;
	friend class cMaterialRef;
	friend class cTextureRef;
public:
	// resource aquisition
	cGeometryRef GetGeometry(const zsString& filePath);
	cMaterialRef GetMaterial(const zsString& filePath);
	cTextureRef GetTexture(const zsString& filePath);

	// constructor
	cResourceManager(IGraphicsApi* gApi);
	~cResourceManager();

	// misc
	IGraphicsApi* GetGraphicsApi();

	// check if manager is still valid
	static bool IsValid(cResourceManager* mgr);
private:
	// automatic resource unloading requested by references
	void UnloadGeometry(const cGeometry* geometry);
	void UnloadMaterial(const cMaterial* material);
	void UnloadTexture(const ITexture2D* texture);

	// resource database
	typedef boost::bimap<boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>, boost::bimaps::unordered_set_of<cGeometry*>> GeometryMapT;
	typedef boost::bimap<boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>, boost::bimaps::unordered_set_of<cMaterial*>> MaterialMapT;
	typedef boost::bimap<boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>, boost::bimaps::unordered_set_of<ITexture2D*>> TextureMapT;

	GeometryMapT geometries;
	MaterialMapT materials;
	TextureMapT textures;

	// graphics api
	IGraphicsApi* const gApi;

	// magic for the case when cSomethingRef is destroyed after it's cResourceManager
	static std::unordered_set<cResourceManager*> validManagers;
};


