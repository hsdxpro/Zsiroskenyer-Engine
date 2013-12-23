#include "Gui.h"

#include "GuiImage.h"
#include "GuiButton.h"
#include "GuiText.h"

#include "../../GraphicsEngineRaster/src/GraphicsEngine.h"
#include "../../Core/src/GraphicsEntity.h"
cGui::cGui() {
}

cGui::cGui(IGraphicsScene* s, size_t width, size_t height)
:scene(s), width(width), height(height) {
	s->GetCamera() = cCamera(cCamera::tProjOrtho(0, (float)width, (float)height, 0), 0.001f, 5000.0f);
}

cGuiImage* cGui::CreateImage(const zsString& mtlPath, size_t posX, size_t posY, size_t width, size_t height) {
	return new cGuiImage(this, CreateQuad(mtlPath, posX, posY, width, height), width, height);
}

cGuiButton* cGui::CreateButton(const zsString& mtlPath, size_t posX, size_t posY, size_t width, size_t height) {
	return new cGuiButton(this, CreateQuad(mtlPath, posX, posY, width, height), width, height);
}

cGuiText* cGui::CreateText(eFontType fT, size_t posX, size_t posY, const zsString& str) {
	// TODO
	return NULL;


	cGraphicsEntity* textEntity;
	return new cGuiText(this, textEntity);
}

IGraphicsScene* cGui::GetScene() {
	return scene;
}

///-----------------------------------------------------INTERNAL FUNCTIONS------------------------------------------------------//
cGraphicsEntity* cGui::CreateQuad(const zsString& mtlPath, size_t posX, size_t posY, size_t width, size_t height) {
	cGraphicsEntity* e = scene->CreateEntity(L"objects/quad.obj", mtlPath);
		e->SetPos(-(float)(this->width - width) / 2.0f + posX, 0, -(float)(this->height - height) / 2.0f + posY);
		e->SetScale((float)width, 1.0f, (float)height);
	return e;
}
