// GuiSystem.h By Zsíroskenyér Team 2013.11.22 23:15
// Responsible for managing (Temporarily rendering) GUI's
#pragma once

#include "ResourceManager.h"
#include "../../Core/src/zsString.h"
#include <list>

class ITexture2D;
class IGraphicsApi;
class cGuiImage;
class cGui;

class cGuiSystem {
public:
	cGuiSystem(IGraphicsApi* gApi);
	~cGuiSystem();

	cGui* CreateGui();
	cGuiImage* CreateImage(const zsString& filePath);
protected:
	// Image gui controls
	std::list<cGuiImage*> images;

	// Guis
	std::list<cGui*> guis;

	// Resource manager
	cResourceManager resMgr;
};