// GuiImage.h By Zsíroskenyér Team 2013.11.22 23:25
#pragma once

class cGraphicsEntity;

class cGuiImage {
	//-------- FULLY INTERNAL ----------//
	friend class cGui;
protected:
	cGuiImage(cGui* parent, cGraphicsEntity* e, size_t width, size_t height);

protected:
	cGraphicsEntity* entity;
	size_t width, height;
	cGui* parent;
};