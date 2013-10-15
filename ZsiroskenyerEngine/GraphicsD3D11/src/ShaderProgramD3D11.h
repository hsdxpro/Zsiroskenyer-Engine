////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation
#pragma once

#include "../../GraphicsCommon/src/IShaderProgram.h"
#include "Dx11_SDK/Include/D3D11.h"
#include "Dx11_SDK/Include/D3DX11.h"
#include "../../CommonLib/src/common.h"

class cShaderProgramD3D11 : public IShaderProgram {
public:
	cShaderProgramD3D11(const cVertexFormat& format,  ID3D11InputLayout* inputLayout = NULL, ID3D11VertexShader* vs = NULL, ID3D11PixelShader* ps = NULL);
	cVertexFormat GetVertexFormat() const override;
	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetInputLayout() const;

private:
	ID3D11VertexShader* vs;
	ID3D11PixelShader* ps;

	ID3D11InputLayout* inputLayout;

	// The Vertex format of the input layout
	cVertexFormat format;
};