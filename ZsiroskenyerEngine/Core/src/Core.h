// Core.h By Zsíroskenyér Team 2013.10.25 23:16
// That class is above all Engine modules
// Like Graphics, Physics, Sound etc..
// Core can create things that need more modul to work together
#pragma once

class cCore {
public:
	static cCore* GetInstance();
protected:
	static cCore* instance;
};