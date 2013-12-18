#include "Gui.h"

#include "GuiImage.h"
#include "../../GraphicsEngineRaster/src/GraphicsEngine.h"

cGui::cGui() {
}

cGui::cGui(IGraphicsScene* s) 
:scene(s) {
}

cGuiImage* cGui::CreateImage(const zsString& mtlPath) {
	return new cGuiImage(scene->CreateEntity(L"objects/quad.dae", mtlPath));
}