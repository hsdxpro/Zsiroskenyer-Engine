// 2013.11.22 22:22 HagymaBox is an incredible easy to use SandBox for Zs�roskeny�r Engine By Zs�roskeny�r Team
// The purpose of that tool is to provide full functionality and usable tools for creating
// cool games as easy as possible
#pragma once

#include "GUISystem.h"

class cHagymaBox {
public:
	cHagymaBox();
	~cHagymaBox();

	void Update(float tDelta);

protected:
	void InitGui();

	cGuiSystem* guiSystem;
};