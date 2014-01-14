////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation
#pragma once

#include "../../Core/src/IShaderProgram.h"	
#include "../../Core/src/common.h"
#include <map>

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
class cGraphicsApiD3D11;

class cShaderProgramD3D11 : public IShaderProgram {
	friend class cGraphicsApiD3D11;
public:
	cShaderProgramD3D11(cGraphicsApiD3D11* parent,
		size_t vertexFormatSize, 
		ID3D11InputLayout* inputLayout = NULL,
		ID3D11VertexShader* vs = NULL, ID3D11HullShader* hs = NULL,
		ID3D11DomainShader* ds = NULL, ID3D11GeometryShader* gs = NULL, 
		ID3D11PixelShader* ps = NULL);
	~cShaderProgramD3D11();
	void Release() override;

	void SetSlotLookups(std::map<zsString, size_t> textureSlots);
	size_t GetTextureSlot(const zsString& varName);

	ID3D11VertexShader* GetVertexShader() const;
	ID3D11HullShader* GetHullShader() const;
	ID3D11DomainShader* GetDomainShader() const;
	ID3D11GeometryShader* GetGeometryShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetInputLayout() const;

	size_t GetVertexFormatSize() const override;
	
private:
	// parent
	cGraphicsApiD3D11* parent;

	// input layout
	ID3D11InputLayout* inputLayout;
	size_t vertexFormatSize;

	// In Sequence of the Dx pipeline :) 
	ID3D11VertexShader*		vs;
	ID3D11HullShader*		hs;
	ID3D11DomainShader*		ds;
	ID3D11GeometryShader*	gs;
	ID3D11PixelShader*		ps;

	// Texture slot look up
	std::map<zsString, size_t> textureSlots;
};