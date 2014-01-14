////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.cpp
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Shader related stuff - not yet known

#include "ShaderProgramD3D11.h"
#include <d3d11.h>
#include "GraphicsApiD3D11.h"

#undef max;

cShaderProgramD3D11::cShaderProgramD3D11(cGraphicsApiD3D11* parent,
	size_t vertexFormatSize,
	ID3D11InputLayout* inputLayout /*= NULL*/,
	ID3D11VertexShader* vs /*= NULL*/,
	ID3D11HullShader* hs /*= NULL*/, ID3D11DomainShader* ds /*= NULL*/,
	ID3D11GeometryShader* gs /*= NULL*/,
	ID3D11PixelShader* ps /*= NULL*/)
	: parent(parent), vertexFormatSize(vertexFormatSize), vs(vs), hs(hs), ds(ds), gs(gs), ps(ps), inputLayout(inputLayout) 
{
}

cShaderProgramD3D11::~cShaderProgramD3D11() {
	return;
}

void cShaderProgramD3D11::Release() {
	parent->UnloadShaderProgram(this);
}

void cShaderProgramD3D11::SetSlotLookups(std::map<zsString, size_t> textureSlots) {
	this->textureSlots = textureSlots;
}

size_t cShaderProgramD3D11::GetTextureSlot(const zsString& varName) {
	auto i = textureSlots.find(varName);
	//ASSERT_MSG(i != textureSlots.end(), L"Texture name: " + varName + L" doesn't exists in that shader");
	return (i != textureSlots.end() ? i->second : std::numeric_limits<size_t>::max());
}

ID3D11VertexShader* cShaderProgramD3D11::GetVertexShader() const {
	return vs;
}

ID3D11HullShader* cShaderProgramD3D11::GetHullShader() const {
	return hs;
}

ID3D11DomainShader* cShaderProgramD3D11::GetDomainShader() const {
	return ds;
}

ID3D11GeometryShader* cShaderProgramD3D11::GetGeometryShader() const {
	return gs;
}

ID3D11PixelShader* cShaderProgramD3D11::GetPixelShader() const {
	return ps;
}

ID3D11InputLayout* cShaderProgramD3D11::GetInputLayout() const {
	return inputLayout;
}

size_t cShaderProgramD3D11::GetVertexFormatSize() const{
	return vertexFormatSize;
}