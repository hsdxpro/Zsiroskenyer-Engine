////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/InstanceGroup.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "InstanceGroup.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "InstanceGroup.h"

cInstanceGroup::cInstanceGroup() {
}

cInstanceGroup::cInstanceGroup(cGeometryRef geom, cMaterialRef mtl) 
: geom(geom), mtl(mtl) {
}

bool cInstanceGroup::operator == (const cInstanceGroup& other) {
	return geom==other.geom && mtl==other.mtl;
}