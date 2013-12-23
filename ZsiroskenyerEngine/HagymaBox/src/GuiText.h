// GuiText.h By Zsíroskenyér Team 2013.12.23 2010
#pragma once

#include "../../Core/src/zsString.h"

class cGraphicsEntity;

class cGuiText {
public:
	//-------- FULLY INTERNAL ----------//
	friend class cGui;
protected:
	cGuiText(cGui* parent, cGraphicsEntity* e);
protected:
	cGraphicsEntity* entity;
	cGui* parent;
	zsString text;
};