////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.cpp
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Shader related stuff - not yet known

#include "ShaderProgramD3D11.h"
#include <d3d11.h>
#include "GraphicsApiD3D11.h"

#undef max

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
	if (vs) vs->Release();
	if (hs) hs->Release();
	if (ds) ds->Release();
	if (gs) gs->Release();
	if (ps) ps->Release();
}

void cShaderProgramD3D11::Release() {
	delete this;
}

void cShaderProgramD3D11::SetTextureSlotsVS(std::map<zsString, size_t> texSlotsVS) {
	textureSlotsVS = texSlotsVS;
}

void cShaderProgramD3D11::SetTextureSlotsPS(std::map<zsString, size_t> texSlotsPS) {
	textureSlotsPS = texSlotsPS;
}

void cShaderProgramD3D11::SetSamplerStatesVS(std::map<size_t, size_t> samplerStatesVS) {
	this->samplerStatesVS = samplerStatesVS;
}

void cShaderProgramD3D11::SetSamplerStatesPS(std::map<size_t, size_t> samplerStatesPS) {
	this->samplerStatesPS = samplerStatesPS;
}

int cShaderProgramD3D11::GetTextureSlotVS(const zsString& varName) const {
	auto i = textureSlotsVS.find(varName);
	return (i != textureSlotsVS.end()) ? i->second : -1;
}

int cShaderProgramD3D11::GetTextureSlotPS(const zsString& varName) const {
	auto i = textureSlotsPS.find(varName);
	return (i != textureSlotsPS.end()) ? i->second : -1;
}

const std::map<size_t, size_t>& cShaderProgramD3D11::GetSamplerStatesVS() const {
	return samplerStatesVS;
}

const std::map<size_t, size_t>& cShaderProgramD3D11::GetSamplerStatesPS() const {
	return samplerStatesPS;
}

const ID3D11VertexShader* cShaderProgramD3D11::GetVertexShader() const {
	return vs;
}

const ID3D11HullShader* cShaderProgramD3D11::GetHullShader() const {
	return hs;
}

const ID3D11DomainShader* cShaderProgramD3D11::GetDomainShader() const {
	return ds;
}

const ID3D11GeometryShader* cShaderProgramD3D11::GetGeometryShader() const {
	return gs;
}

const ID3D11PixelShader* cShaderProgramD3D11::GetPixelShader() const {
	return ps;
}

const ID3D11InputLayout* cShaderProgramD3D11::GetInputLayout() const {
	return inputLayout;
}

size_t cShaderProgramD3D11::GetVertexFormatSize() const {
	return vertexFormatSize;
}