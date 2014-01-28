// GuiImage.h By Zsíroskenyér Team 2013.12.23 19:50
#pragma once

#include "../../Core/src/zsString.h"

class cGraphicsEntity;

class cGuiButton {
public:
	void SetText(const zsString& str);

//-------- FULLY INTERNAL ----------//
	friend class cGui;
protected:
	cGuiButton(cGui* parent, cGraphicsEntity* e, size_t width, size_t height);

protected:
	cGraphicsEntity* entity;
	size_t width, height;
	cGui* parent;
};