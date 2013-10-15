////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ManagerShader.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This class loads, stores, retrieves shaders requested by the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include "../../CommonLib/src/zsString.h"

class IShaderProgram;
class IGraphicsApi;


class cManagerShader {
public:
	IShaderProgram* LoadShader(const zsString& shaderDir, const zsString& shaderName); // load a shader once from source
															// multiple loads will return the already-loaded shader

	void UnloadShader(const zsString& shaderName); // permanently unload specified shader
	void UnloadShader(IShaderProgram* shader);

	IShaderProgram* ReloadShader(const zsString& shaderDir, const zsString& shaderName);
	IShaderProgram* ReloadShader(const zsString& shaderDir, IShaderProgram* shader);

	IShaderProgram* GetShaderByName(const zsString& shaderName); // returns the shader by its name

	void Reset(); // unloads all shaders

	cManagerShader(IGraphicsApi* graphicsApi);
	~cManagerShader();
private:
	typedef boost::bimap<boost::bimaps::unordered_set_of<IShaderProgram*>, boost::bimaps::unordered_set_of<zsString>> ShaderMapT;

	ShaderMapT loadedShaders;
	IGraphicsApi* graphicsApi;
};