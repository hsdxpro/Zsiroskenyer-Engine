////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a list of Entities with the same material and geometry.
//	Used for instanced rendering of objects.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ManagerResource.h"
#include <unordered_set>
#include "../../GraphicsApi/src/ITexture2D.h"

class cEntity;

class cInstanceGroup {
	friend struct std::hash<cInstanceGroup>;
public:
	cInstanceGroup();
	cInstanceGroup(cGeometryRef, cMaterialRef);

	bool operator == (const cInstanceGroup& other);

	// Geometry
	cGeometryRef geom;

	// Material
	cMaterialRef mtl;

	// Entities
	std::unordered_set<cEntity*> entities;
};



template <>
struct std::hash<cInstanceGroup> {
	typedef cMaterialRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cInstanceGroup i) const {
		return std::hash<cGeometryRef>()(i.geom)^std::hash<cMaterialRef>()(i.mtl);
	}
};