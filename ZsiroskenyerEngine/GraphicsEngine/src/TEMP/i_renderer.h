////////////////////////////////////////////////////////////////////////////////
//	interface.h
//	this contains the abstract class of the renderer
//
////////////////////////////////////////////////////////////////////////////////
#pragma once


#include <Windows.h>
#include <climits>
#include <math\vec3.h>
#include <math\color.h>


union Matrix44;
class IVertexBuffer;
class IIndexBuffer;
class ITexture;


////////////////////////////////////////////////////////////////////////////////
// structs, typedefs and defines
struct MATERIAL {
	Colorf diffuse, ambient, emissive, specular;
	float power;
	ITexture *tDiffuse, *tSpecular;
	MATERIAL() {tDiffuse=nullptr; tSpecular=nullptr;}
};
struct ATTRIB {
	int offset;
	int nFaces;
	int id;
};
enum LIGHT_TYPE {
	LT_AMBIENT=0,
	LT_DIRECTIONAL=1,
	LT_POINT=2,
	LT_SPOT=4
};
struct LIGHT {
	LIGHT_TYPE type;
	bool enable;
	Colorf color;
	Vec3 position;
	Vec3 direction;
	float range;
	float atten0, atten1, atten2;
	float inner, outer;
};

////////////////////////////////////////////////////////////////////////////////
// the renderer interface
class IRenderer {
	public:
		// call this to render stuff 
			//virtual void Render(SceneGeometryDescriptor& scenePart) = 0; // obsolete

		// rendering
		virtual void BeginScene()=0;

		virtual void BeginGeometryPass()=0;
		virtual void Draw(IVertexBuffer* VB, IIndexBuffer* IB, Matrix44* pMatTransform, unsigned nInstances=1)=0;
		virtual void Draw(IVertexBuffer* VB, IIndexBuffer* IB,  MATERIAL* materials, ATTRIB* attribs, unsigned nSubMaterials, Matrix44* pMatTransform, unsigned nInstances=1)=0;
		virtual void EndGeometryPass()=0;

		virtual void EndScene()=0;
		virtual void Present()=0;

		// drawing text
		virtual void TextDraw(const char* text, int left, int top, int right, int bottom, unsigned textColorARGB=0xFFffffff,
								int fontHeight=12, unsigned fontWidth=0, const char* fontName="Lucida Consolas", bool bold=false, bool italic=false) {}
		virtual void TextDraw(const wchar_t* text, int left, int top, int right, int bottom, unsigned textColorARGB=0xFFffffff,
								int fontHeight=12, unsigned fontWidth=0, const char* fontName="Lucida Consolas", bool bold=false, bool italic=false) {}


		// buffers
		virtual int CreateVertexBuffer(IVertexBuffer** vertexBuffer, unsigned lenght, DWORD FVF=0) = 0;
		virtual int CreateIndexBuffer(IIndexBuffer** indexBuffer, unsigned lenght) = 0;
		virtual int CreateTexture(ITexture** texture, unsigned width, unsigned height, unsigned levels) = 0;
		virtual int CreateTextureFromFile(ITexture** texture, const char* fileName, unsigned levels=0) = 0;

		// resize device
		virtual void Resolution(unsigned x, unsigned y) = 0;
		virtual void UseFullScreen(bool bEnableFS, unsigned scrWidth=0, unsigned scrHeight=0) = 0;

		// set camera
		virtual void SetCamera(Vec3 pos, Vec3 target, Vec3 upDirection=Vec3(/*std::numeric_limits<float>::quiet_NaN()*/0,0,0),
							 float FOV=-1.f, float nearViewPlane=-1.f, float farViewPlane=-1.f) = 0;
		// set target
		virtual void SetTargetWindow(HWND hWnd) = 0;

		// manage lights
		virtual LIGHT* AddLight(const std::string& name,  LIGHT& light) = 0;
		virtual LIGHT* GetLight(const std::string& name) = 0;
		virtual void RemoveLight(const std::string& name, bool all=false) = 0;

		// release interface
		virtual void Release() {delete this;}
};