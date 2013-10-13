////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "../../GraphicsCommon/src/IGraphicsEngine.h"

#include "ManagerScene.h"
#include "ManagerResource.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();

	void RenderSceneForward();

	IManagerScene* GetSceneManager();
private:	
	cManagerResource managerResource;
	cManagerScene managerScene;
	IGraphicsApi* gApi;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}