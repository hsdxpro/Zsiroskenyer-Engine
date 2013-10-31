// ResourceManager.h By Zsíroskenyér Team 2013.10.27 13:32
#pragma once

#include "GeometryBuilder.h"
#include "zsString.h"

class cGeometryRef;
class cMaterialRef;

class cGeometry;
class cMaterial;

class IResourceManager {
	// Resource accessing
	virtual cGeometryRef GetGeometry(const zsString& filePath) = 0;

	virtual cMaterialRef GetMaterial(const zsString& filePath) = 0;
private:
	// free up resources
	virtual void UnloadGeometry(const cGeometry* geometry) = 0;
	virtual void UnloadMaterial(const cMaterial* material) = 0;
};

