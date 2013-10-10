#include "..\..\RendererD3D11\src\GraphicsD3D11.h"
#include "..\..\GraphicsEngine\src\WindowWin32.h"

int main() {
	IGraphicsApi * dx11 = CreateGraphicsD3D11();
	IWindow *myWindow = new cWindowWin32();
	return 0;
}