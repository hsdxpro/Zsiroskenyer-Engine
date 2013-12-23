// GuiSystem.h By Zsíroskenyér Team 2013.11.22 23:15
// Responsible for managing (Temporarily rendering) GUI's
#pragma once

#include "../../Core/src/zsString.h"
#include <vector>

class IGraphicsScene;
class cGuiImage;
class cGui;

class cGuiSystem {
public:
	cGuiSystem();
	~cGuiSystem();

	cGui* CreateGui(IGraphicsScene* s, size_t width, size_t height);
protected:
	// Image gui controls
	std::vector<cGuiImage*> images;

	// Guis
	std::vector<cGui*> guis;
};