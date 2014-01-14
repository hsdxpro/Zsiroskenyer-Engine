////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/ShaderManager.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	This class loads, stores, retrieves shaders requested by the graphics engine.
////////////////////////////////////////////////////////////////////////////////
//
// THIS FILE IS NO LONGER IN USE, BUT IS KEPT FOR A SHORT TIME
//
////////////////////////////////////////////////////////////////////////////////


#pragma message("NOTE: shadermanager.h: IShaderManager deleted!")

/*
#include "..\..\Core\src\IShaderManager.h"

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include "../../Core/src/zsString.h"

class IShaderProgram;
class IGraphicsApi;

class cShaderManager : public IShaderManager {
public:
	IShaderProgram* LoadShader(const zsString& shaderPath) override;
															
	void UnloadShader(const zsString& shaderName) override;
	void UnloadShader(IShaderProgram* shader) override;

	IShaderProgram* ReloadShader(const zsString& shaderPath)  override;
	IShaderProgram* ReloadShader(const zsString& shaderDir, IShaderProgram* shader)  override;

	IShaderProgram* GetShaderByName(const zsString& shaderName) override;

	void Reset() override; // unloads all shaders

	cShaderManager(IGraphicsApi* gApi);
	~cShaderManager();
private:
	typedef boost::bimap<boost::bimaps::unordered_set_of<IShaderProgram*>, boost::bimaps::unordered_set_of<zsString, std::hash<zsString>>> ShaderMapT;

	ShaderMapT loadedShaders;
	IGraphicsApi* gApi;
};

*/