#include "GuiButton.h"
#include "Gui.h"
#include "../../Core/src/math/Vec3.h"
#include "../../Core/src/GraphicsEntity.h"

cGuiButton::cGuiButton(cGui* parent, cGraphicsEntity* e, size_t width, size_t height)
:parent(parent), entity(e), width(width), height(height) {
}

void cGuiButton::SetText(const zsString& str) {
	const Vec3& pos = entity->GetPos();
	parent->AddText(eFontType::DEFAULT, (size_t)pos.x, (size_t)pos.y, str);
}