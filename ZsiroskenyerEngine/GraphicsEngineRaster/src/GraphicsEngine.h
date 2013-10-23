////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This is the core class of the graphics engine, that holds together all the
//	stuff.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../GraphicsEngine/src/IGraphicsEngine.h"


class IGraphicsApi;
class cManagerShader;
class cSceneManager;
class cResourceManager;

class cGraphicsEngine : public IGraphicsEngine {
public:
	cGraphicsEngine();
	void Release() override;

	void RenderWorldForward() override;

	ISceneManager* GetManagerScene();
	cResourceManager* GetManagerResource();
	IGraphicsApi* GetGraphicsApi();
private:	
	IGraphicsApi* gApi;
	cManagerShader* managerShader;
	cResourceManager* managerResource;
	cSceneManager* managerScene;
};


extern "C"
__declspec(dllexport) IGraphicsEngine* CreateGraphicsEngine() {
	return new cGraphicsEngine();
}