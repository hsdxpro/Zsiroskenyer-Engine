#include "Gui.h"

#include "GuiImage.h"
#include "../../GraphicsEngineRaster/src/GraphicsEngine.h"
#include "../../Core/src/GraphicsEntity.h"
cGui::cGui() {
}

cGui::cGui(IGraphicsScene* s) 
:scene(s) {
	s->GetCamera() = cCamera(cCamera::tProjOrtho(0, 1, 0, 1), 0.01f, 5000.0f);
}

cGuiImage* cGui::CreateImage(const zsString& mtlPath) {
	cGraphicsEntity* e = scene->CreateEntity(L"objects/quad.obj", mtlPath);
	e->SetPos(1, 1, 1);
	return new cGuiImage(e);
}

IGraphicsScene* cGui::GetScene() {
	return scene;
}