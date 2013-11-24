// 2013.11.22 22:22 HagymaBox is an incredible easy to use SandBox for Zsíroskenyér Engine By Zsíroskenyér Team
// The purpose of that tool is to provide full functionality and usable tools for creating
// cool games as easy as possible
#pragma once

class cGuiSystem;
class cGuiRenderer;
class cGui;
class IWindow;
class cCore;

class cHagymaBox {
public:
	cHagymaBox(IWindow* w);
	~cHagymaBox();

	void Update(float tDelta);

protected:
	void InitGui();
	void RenderGui();

	// The main gui for the editor
	cGui* mainGui;

	// Guisystem responsible for the interface
	cGuiSystem* guiSystem;

	// Zsíroskenyér core used in the toolbox
	cCore *engineCore;
	cGuiRenderer *guiRenderer;
};