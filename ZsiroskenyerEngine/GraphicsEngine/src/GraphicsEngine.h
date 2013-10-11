////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "IGraphicsEngine.h"

#include "SceneManager.h"
#include "ResourceManager.h"


class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();

	ISceneManager* GetSceneManager();
private:	
	cManagerResource resourceManager;
	cManagerScene sceneManager;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}