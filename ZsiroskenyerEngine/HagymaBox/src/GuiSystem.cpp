#include "GuiSystem.h"

#include "Gui.h"
#include "GuiImage.h"

#include "../../Core/src/common.h"
#include "../../Core/src/ITexture2D.h"

cGuiSystem::~cGuiSystem() {
	for (auto i = images.begin(); i != images.end(); i++)
		SAFE_DELETE(*i);

	for (auto i = guis.begin(); i != guis.end(); i++)
		SAFE_DELETE(*i);

	for (auto i = textures.begin(); i != textures.end(); i++)
		SAFE_RELEASE((*i));
}

cGui* cGuiSystem::CreateGui() {
	cGui* g = new cGui();
	guis.push_back(g);
	return g;
}

cGuiImage* cGuiSystem::CreateImage(ITexture2D* t) {
	textures.push_back(t);
	cGuiImage* img = new cGuiImage(t);
	images.push_back(img);
	return img;
}