// Gui.h By Zsíroskenyér Team 2013.11.24 22:42
// Holding GuiControls
#pragma once

#include "../../Core/src/zsString.h"
#include <list>

class IGraphicsScene;
class cGuiImage;

class cGui {
public:
	friend class cGuiSystem;

	cGuiImage* CreateImage(const zsString& mtlPath);

protected:
	cGui();
	cGui(IGraphicsScene* s);
protected:
	// Gui represented with cGraphicsScene
	IGraphicsScene* scene;

	std::list<cGuiImage*> images;
};