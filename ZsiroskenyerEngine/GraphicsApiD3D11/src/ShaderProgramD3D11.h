////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation
#pragma once

#include "../../Core/src/IShaderProgram.h"	

// TODO DO SOMETHING, I DON'T WANT TO INCLUDE THE WHOLE COMMON JUST FOR THE "NULL"
#include "../../Core/src/common.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class cShaderProgramD3D11 : public IShaderProgram {
public:
	cShaderProgramD3D11(size_t vertexFormatSize, ID3D11InputLayout* inputLayout = NULL, ID3D11VertexShader* vs = NULL, ID3D11PixelShader* ps = NULL);

	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetInputLayout() const;

	size_t GetVertexFormatSize() const override;

private:
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;

	ID3D11InputLayout* inputLayout;
	size_t vertexFormatSize;
};