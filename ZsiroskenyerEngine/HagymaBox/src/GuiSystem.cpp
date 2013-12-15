#include "GuiSystem.h"

#include "Gui.h"
#include "GuiImage.h"

#include "../../Core/src/common.h"
#include "../../Core/src/ITexture2D.h"

cGuiSystem::cGuiSystem() 
{}

cGuiSystem::~cGuiSystem() {
	for (auto i = images.begin(); i != images.end(); i++)
		SAFE_DELETE(*i);

	for (auto i = guis.begin(); i != guis.end(); i++)
		SAFE_DELETE(*i);
}

cGui* cGuiSystem::CreateGui() {
	cGui* g = new cGui();
	guis.push_back(g);
	return g;
}

cGuiImage* cGuiSystem::CreateImage(const zsString& filePath) {
	//cGuiImage* img = new cGuiImage(resMgr.GetTexture(filePath));
	//images.push_back(img);
	return NULL;
}