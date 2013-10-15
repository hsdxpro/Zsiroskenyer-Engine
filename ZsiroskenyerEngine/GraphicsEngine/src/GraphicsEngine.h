////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////


#pragma once


#include "../../GraphicsCommon/src/IGraphicsEngine.h"


class IGraphicsApi;
class cManagerShader;
class cManagerScene;
class cManagerResource;

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();

	void RenderSceneForward();

	IManagerScene* GetSceneManager();
private:
	IGraphicsApi* gApi;
	cManagerShader* managerShader;
	cManagerResource* managerResource;
	cManagerScene* managerScene;	
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}