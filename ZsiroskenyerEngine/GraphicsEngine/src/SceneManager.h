////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/SceneManager.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that manages the 3D scene's entities.
//	This includes, but is not limited to creation/deletion of entities,
//	positioning entities, talking to the resource manager, grouping entities
//	by instancing and/or spatial position.
////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <unordered_set>
#include <string>


class cInstanceGroup;
class Entity;
class cResourceManager;


class cSceneManager {
public:
	cSceneManager(cResourceManager& rm);
	~cSceneManager();

	Entity& AddEntity(const std::string& geometry, const std::string& material);
	void RemoveEntity(const Entity& entity);

private:
	std::unordered_set<cInstanceGroup*> instanceGroups;
	cResourceManager& resourceManager;
};