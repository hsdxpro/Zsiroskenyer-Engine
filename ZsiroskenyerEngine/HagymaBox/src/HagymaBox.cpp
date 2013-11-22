#include "HagymaBox.h"

#include "../../Core/src/common.h"

cHagymaBox::cHagymaBox() {
	// Create gui system
	guiSystem = new cGuiSystem();

	// Init gui
	InitGui();
}

cHagymaBox::~cHagymaBox() {
	SAFE_DELETE(guiSystem);
}

void cHagymaBox::InitGui() {
	//cGuiImage* img = guiSystem->Createmage("textures/cliff.jpg");
}

void cHagymaBox::Update(float tDelta) {
	// Updating the gui system..

	// Editor Logic here :....
}
