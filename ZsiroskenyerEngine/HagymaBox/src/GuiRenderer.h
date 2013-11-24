// GuiRenderer.h By Zsíroskenyér Team 2013.11.24
// For rendering Gui elements etc..
#pragma once

class IGraphicsApi;
class IShaderManager;
class IShaderProgram;
class cGui;

class cGuiRenderer {
public:
	cGuiRenderer(IGraphicsApi* gApi, IShaderManager* shaderManager);
	~cGuiRenderer();

	void RenderGui(const cGui* gui);

protected:
	IGraphicsApi* gApi;
	IShaderManager* shaderManager;

	IShaderProgram* guiShader;
};