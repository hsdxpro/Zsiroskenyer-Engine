// Gui.h By Zsíroskenyér Team 2013.11.24 22:42
// Holding GuiControls
#pragma once

#include <list>

class cGuiImage;

class cGui {
public:
	friend class cGuiSystem;

	void Add(cGuiImage* img, unsigned posX, unsigned posY);
protected:
	cGui();
protected:
	std::list<cGuiImage*> images;
};