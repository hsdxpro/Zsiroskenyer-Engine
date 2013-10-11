////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "../../GraphicsEngine/src/IGraphicsEngine.h"

#include "ManagerScene.h"
#include "ManagerResource.h"


class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();

	IManagerScene* GetSceneManager();
private:	
	cManagerResource managerResource;
	cManagerScene managerScene;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}