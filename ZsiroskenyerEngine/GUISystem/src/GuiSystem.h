#include "Widget.h"

class cGraphicsScene;

class cGuiSystem {
public:
	cGuiSystem(cGraphicsScene* renderTarget);

	void AddWidget(cWidget* widget);
private:

};