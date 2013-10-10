////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that manages the 3D scene's entities.
//	This includes, but is not limited to creation/deletion of entities,
//	positioning entities, talking to the resource manager, grouping entities
//	by instancing and/or spatial position.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "..\..\CommonLib\src\zsString.h"
#include "InstanceGroup.h"

#include <unordered_set>


class cEntity;
class cResourceManager;

// helpers for storing cInstanceGroups by pointer
struct cInstGroupPtrHasher {
	size_t operator()(cInstanceGroup* i) {
		return i!=nullptr?std::hash<cInstanceGroup>()(*i):0;
	}
};
struct cInstGroupPtrCompare {
	bool operator()(cInstanceGroup* a, cInstanceGroup* b) {
		if (a==nullptr || b==nullptr)
			return false;
		return *a==*b;
	}
};

// scene manager
class cSceneManager {
public:
	cSceneManager(cResourceManager& rm);
	~cSceneManager();

	cEntity& AddEntity(const zsString& geometry, const zsString& material);
	void RemoveEntity(const cEntity& entity);

private:
	std::unordered_set<cInstanceGroup*,	cInstGroupPtrHasher,cInstGroupPtrCompare> instanceGroups;
	cResourceManager& resourceManager;
};