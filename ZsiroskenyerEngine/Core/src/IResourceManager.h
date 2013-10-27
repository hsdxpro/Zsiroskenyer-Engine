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
	virtual cGeometryRef *LoadGeometry(const zsString& fileName) = 0;
	virtual cGeometryRef *LoadGeometry(const zsString& fileName, const cGeometryBuilder::tGeometryDesc& geomDesc) = 0;

	virtual cMaterialRef *LoadMaterial(const zsString& fileName) = 0;

	// Resource existence
	virtual bool IsGeometryExists(const zsString& fileName) = 0;
	virtual bool IsMaterialExists(const zsString& fileName) = 0;

private:
	// free up resources
	virtual void UnloadGeometry(const cGeometry* geometry) = 0;
	virtual void UnloadMaterial(const cMaterial* material) = 0;
};

