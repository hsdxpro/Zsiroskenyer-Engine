#include "ResourceManager.h"


cResourceManager::cResourceManager(IGraphicsApi* gApi) 
:gApi(gApi) {
}

cResourceManager::~cResourceManager() {

}

ITexture2D* cResourceManager::GetTexture(const zsString& filePath) {
	return NULL;
}
