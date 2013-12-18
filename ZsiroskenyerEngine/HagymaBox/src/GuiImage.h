// GuiImage.h By Zsíroskenyér Team 2013.11.22 23:25
#pragma once

class cGraphicsEntity;

class cGuiImage {
	friend class cGui;
protected:
	cGuiImage(cGraphicsEntity* e);
protected:
	cGraphicsEntity* entity;
};