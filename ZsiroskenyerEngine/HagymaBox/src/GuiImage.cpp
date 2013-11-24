#include "GuiImage.h"

cGuiImage::cGuiImage(ITexture2D* t)
:texture(t), posX(0), posY(0) {
}

void cGuiImage::SetPos(unsigned x, unsigned y) {
	posX = x; posY = y;
}