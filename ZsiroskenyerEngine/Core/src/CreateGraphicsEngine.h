// bazmeg vmi file headert írjál már, kedves Future Én

#pragma once

#include "IGraphicsEngine.h"

class IGraphicsEngine;
class IWindow;

IGraphicsEngine* CreateGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config);