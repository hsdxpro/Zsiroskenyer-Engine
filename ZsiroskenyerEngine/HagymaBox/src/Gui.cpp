#include "Gui.h"

#include "GuiImage.h"
#include "../../GraphicsEngineRaster/src/GraphicsEngine.h"

cGui::cGui() {
}

cGui::cGui(IGraphicsScene* s) 
:scene(s) {
	s->GetCamera() = cCamera(/*0.5*3.141592653589*/1.15f, 1.0f, 0.01f, 5000.0f);
}

cGuiImage* cGui::CreateImage(const zsString& mtlPath) {
	return new cGuiImage(scene->CreateEntity(L"objects/quad.dae", mtlPath));
}

cCamera& cGui::GetCamera() {
	return scene->GetCamera();
}