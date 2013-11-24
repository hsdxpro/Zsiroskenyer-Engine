// GuiSystem.h By Zsíroskenyér Team 2013.11.22 23:15
// Responsible for managing (Temporarily rendering) GUI's
#pragma once

#include <boost/unordered_map.hpp>

#include "../../Core/src/zsString.h"
#include <list>

class cGuiImage;
class ITexture2D;
class cGui;

class cGuiSystem {
public:
	~cGuiSystem();

	cGui* CreateGui();
	cGuiImage* CreateImage(ITexture2D* t);
protected:
	std::list<cGuiImage*> images;
	std::list<cGui*> guis;

	// Lol resource managing is there xD Yeah Richard Yeah !
	std::list<ITexture2D*> textures;
};