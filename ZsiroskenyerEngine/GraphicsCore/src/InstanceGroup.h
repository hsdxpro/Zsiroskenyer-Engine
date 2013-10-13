////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a list of Entities with the same material and geometry.
//	Used for instanced rendering of objects.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ManagerResource.h"
#include <unordered_set>


class cEntity;



class cInstanceGroup {
	friend struct std::hash<cInstanceGroup>;
public:
	cInstanceGroup() {}
	cInstanceGroup(cGeometryRef, cMaterialRef);

	cGeometryRef geom;
	cMaterialRef mtl;
	std::unordered_set<cEntity*> entities;
	bool operator==(const cInstanceGroup& other) {
		return geom==other.geom && mtl==other.mtl;
	}
};



template <>
struct std::hash<cInstanceGroup> {
	typedef cMaterialRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cInstanceGroup i) const {
		return std::hash<cGeometryRef>()(i.geom)^std::hash<cMaterialRef>()(i.mtl);
	}
};