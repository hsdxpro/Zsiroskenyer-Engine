////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../GraphicsCommon/src/IGraphicsEngine.h"
#include "../../GraphicsCommon/src/IGraphicsApi.h"

#include "ManagerScene.h"
#include "ManagerResource.h"
#include "ManagerShader.h"

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();

	void RenderSceneForward();

	IManagerScene* GetSceneManager();
private:	
	cManagerResource mgrResource;
	cManagerScene mgrScene;
	cManagerShader *mgrShader;
	IGraphicsApi* mgrGApi;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}