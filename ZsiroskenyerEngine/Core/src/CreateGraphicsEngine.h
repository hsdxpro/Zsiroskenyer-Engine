// bazmeg vmi file headert �rj�l m�r, kedves Future �n

#pragma once

#include "IGraphicsEngine.h"

class IGraphicsEngine;
class IWindow;

IGraphicsEngine* CreateGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);