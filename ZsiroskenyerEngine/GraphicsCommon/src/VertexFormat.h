////////////////////////////////////////////////////////////////////////////////
//	GraphicsCommon/src/VertexFormat.h
//	2013.oct.14, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class declaration is worth a thousand words...
////////////////////////////////////////////////////////////////////////////////


enum class eVertexAttributes : unsigned {
	POSITION=1,
	NORMAL=2,
	TEXCOORD=4,
	TANGENT=8,
	BONE=16,
};


class cVertexFormat {
public:
	void SetAttribute(eVertexAttributes attrib, bool value);
	void AddAttribute(eVertexAttributes attrib);
	void ClearAttribute(eVertexAttributes attrib);

	unsigned Get();

	cVertexFormat();
private:
	unsigned attribs;
};