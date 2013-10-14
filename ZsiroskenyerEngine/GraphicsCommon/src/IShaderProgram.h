////////////////////////////////////////////////////////////////////////////////
//	GraphicsCommon/src/IShaderProgram.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Unbelievably complex Shader Program interface...
//	Actually, an IShaderProgram contains the fully compiled and linked shader,
//	thus including vertex shader, pixel shader, and other stuff.
//	Vertex declaration is also stored in this shader. Use shaders only with
//	correct vertex input format, otherwise behaviour is __undefined__.
////////////////////////////////////////////////////////////////////////////////


#include "VertexFormat.h"

class IShaderProgram {
public:
	virtual cVertexFormat GetVertexFormat();
};