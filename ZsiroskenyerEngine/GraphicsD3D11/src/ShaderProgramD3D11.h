////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation


#include "../../GraphicsCommon/src/IShaderProgram.h"

class cShaderProgramD3D11 : public IShaderProgram {
public:
	cVertexFormat GetVertexFormat() override;
private:
	cVertexFormat format;
};