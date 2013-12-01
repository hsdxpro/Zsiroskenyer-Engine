#include "GuiRenderer.h"

cGuiRenderer::cGuiRenderer(IGraphicsApi* gApi, IShaderManager* shaderManager)
:gApi(gApi), shaderManager(shaderManager) {
}

cGuiRenderer::~cGuiRenderer() {
}

void cGuiRenderer::RenderGui(const cGui* gui) {
}