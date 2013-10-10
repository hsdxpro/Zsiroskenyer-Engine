////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////


#include "GraphicsEngine.h"

// construction of the graphics engine
cGraphicsEngine::cGraphicsEngine()
	:
	resourceManager(),
	sceneManager(resourceManager)
{
}



// interface
ISceneManager* cGraphicsEngine::GetSceneManager() {
	return (ISceneManager*)&sceneManager;
}