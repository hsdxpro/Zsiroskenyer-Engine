// IGraphicsApi.h By Zsíroskenyér Team 2013.10.10 1:37
// interface for different 3D graphics libraries like DirectX11 or OpenGL ..

#include "IWindow.h"

class IGraphicsApi {
public:
	virtual void Present() = 0;
	virtual void SetWindow(IWindow *renderWindow) = 0;
};