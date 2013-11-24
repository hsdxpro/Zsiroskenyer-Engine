#include "Gui.h"

#include "GuiImage.h"

cGui::cGui() {
}

void cGui::Add(cGuiImage* img, unsigned posX, unsigned posY) {
	img->SetPos(posX, posY);
	images.push_back(img);
}