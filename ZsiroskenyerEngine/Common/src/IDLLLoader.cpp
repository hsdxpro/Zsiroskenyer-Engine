#include "IDLLLoader.h"

#include "../../CommonOS/src/DLLLoader.h"

IDLLLoader* IDLLLoader::Create() {
	return new cDLLLoader();
}