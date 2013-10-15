////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.cpp
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Shader related stuff - not yet known

#include "ShaderProgramD3D11.h"

cShaderProgramD3D11::cShaderProgramD3D11(const cVertexFormat& format,  ID3D11InputLayout* inputLayout /*= NULL*/, ID3D11VertexShader* vs /*= NULL*/, ID3D11PixelShader* ps /*= NULL*/)
:vs(vs), ps(ps), inputLayout(inputLayout), format(format) {
}

cVertexFormat cShaderProgramD3D11::GetVertexFormat() {
	return format;
}