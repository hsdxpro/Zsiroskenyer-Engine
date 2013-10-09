////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Contains a list of Entities with the same material and geometry.
//	Used for instanced rendering of objects.
////////////////////////////////////////////////////////////////////////////////


#include "ResourceManager.h"
#include <unordered_set>


class Entity;



class cInstanceGroup {
	friend struct std::hash<cInstanceGroup>;
public:
	cInstanceGroup();
	cInstanceGroup(cGeometryRef, cMaterialRef);

	cGeometryRef geom;
	cMaterialRef mtl;
	std::unordered_set<Entity*> entityList;
	bool operator==(const cInstanceGroup& other) {
		return geom==other.geom && mtl==other.mtl;
	}
};



template <>
class std::hash<cInstanceGroup> {
	typedef cMaterialRef argument_type;
	typedef std::size_t return_type;
	std::size_t operator()(const cInstanceGroup i) {
		return std::hash<cGeometryRef>()(i.geom)^std::hash<cMaterialRef>()(i.mtl);
	}
};