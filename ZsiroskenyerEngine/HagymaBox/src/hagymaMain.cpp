#pragma once

#include "../../Core/src/IWindow.h"
#include "../../Core/src/Timer.h"
#include "HagymaBox.h"

// I think we need an incredibly flexible GUI library.
// This library contain multifarious gui elements, to build the best user interface
// to iteract with. The lib uses Zsíroskenyér Engine's graphics api library to achieve
// platform independence.

// You can intergrate into your app, just create the gui system with Dx, OpenGL, or
// with some tracer engine. And it will do the job for you.
// Or you can implement some interfaces and that's all :)

// The gui system that i create is just test, so sources are stay in HagymaBox project

int main() {
	// Create window
	IWindow::tDesc winDesc;
		winDesc.brush = IWindow::eBrush::RENDER_;
		winDesc.captionName = L"HagymaBox 0.1";
		winDesc.clientHeight = 600;
		winDesc.clientWidth = 800;
		winDesc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* window = IWindow::Create(winDesc);

	
	cHagymaBox editor(window);

	cTimer t;
	IWindow::eMessage msg;
	while (window->IsOpened()) {
		while (window->HandleMessage(&msg));

		editor.Update(t.getDeltaSeconds());
	}
}