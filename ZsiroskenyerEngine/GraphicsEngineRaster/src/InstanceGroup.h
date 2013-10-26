////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a list of Entities with the same material and geometry.
//	Used for instanced rendering of objects.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ResourceManager.h"
#include <unordered_set>
#include "../../Core/src/ITexture2D.h"

class cGraphicsEntity;

class cInstanceGroup {
	friend struct std::hash<cInstanceGroup>;
public:
	cInstanceGroup();
	cInstanceGroup(cGraphicsGeometryRef, cMaterialRef);

	bool operator == (const cInstanceGroup& other);

	// Geometry
	cGraphicsGeometryRef geom;

	// Material
	cMaterialRef mtl;

	// Entities
	std::unordered_set<cGraphicsEntity*> entities;
};



template <>
struct std::hash<cInstanceGroup> {
	typedef cMaterialRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cInstanceGroup i) const {
		return std::hash<cGraphicsGeometryRef>()(i.geom)^std::hash<cMaterialRef>()(i.mtl);
	}
};