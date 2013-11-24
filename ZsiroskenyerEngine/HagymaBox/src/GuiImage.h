// GuiImage.h By Zsíroskenyér Team 2013.11.22 23:25
#pragma once

class ITexture2D;

class cGuiImage {
	friend class cGuiSystem;
public:
	void SetPos( unsigned posX, unsigned posY);

protected:
	cGuiImage(ITexture2D* t);
protected:
	ITexture2D* texture;

	unsigned posX, posY;
};