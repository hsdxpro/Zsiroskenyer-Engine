#include "IWindow.h"
#include "../../CommonOS/src/Window.h"

IWindow* IWindow::Create(const IWindow::tDesc& desc) {
	return new cWindow(desc);
}