// 2013.11.22 22:22 HagymaBox is an incredible easy to use SandBox for Zsíroskenyér Engine By Zsíroskenyér Team
// The purpose of that tool is to provide full functionality and usable tools for creating
// cool games as easy as possible
#pragma once

class cCore;
class cGuiSystem;
class cGui;
class IGraphicsScene;
class IWindow;

#include "../../Core/src/Camera.h"

class cHagymaBox {
public:
	cHagymaBox(IWindow* w);
	~cHagymaBox();

	void Update(float deltaT);

	cCore* GetEngineCore();
	cCamera& GetCamera();

protected:
	void InitGui();

	// Zsíroskenyér core used in the toolbox
	cCore *engineCore;

	// Main 3D scene for editing
	IGraphicsScene* scene;

	// Guisystem responsible for the interface
	cGuiSystem* guiSystem;

	// The main gui for the editor
	cGui* mainGui;
};