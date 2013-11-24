// ResourceManager.h By Zsíroskenyér Team 2013.11.24
// holding textures for the GuiSystem, and some geometries
// TODO, may be optimal to store only one quad, and post transform appropriately
#pragma once

#include "../../Core/src/zsString.h"

#include <boost/unordered_map.hpp>

class ITexture2D;
class cGeometry;
class IGraphicsApi;

class cResourceManager {
public:
	cResourceManager(IGraphicsApi* gApi);
	~cResourceManager();

	ITexture2D* GetTexture(const zsString& filePath);
protected:
	typedef boost::unordered_map<zsString, ITexture2D*> TextureMapT;

	TextureMapT textures;

	IGraphicsApi* gApi;
};

