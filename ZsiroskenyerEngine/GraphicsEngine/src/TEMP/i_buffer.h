////////////////////////////////////////////////////////////////////////////////
// This provides interfaces for Index and Vertex buffers
// independent, implementation in renderer.dll
//
////////////////////////////////////////////////////////////////////////////////


#pragma once

////////////////////////////////////////////////////////////////////////////////
// these are the FVF stuff

// used by clients to specify FVF formats, the graphics engine processes it on its own
// codes are the same as in D3D9 for the ease of use

/*
#define FVF_NONE			 0xFFFF

#define FVF_RESERVED0        0x001
#define FVF_POSITION_MASK    0x400E
#define FVF_XYZ              0x002
#define FVF_XYZRHW           0x004
#define FVF_XYZB1            0x006
#define FVF_XYZB2            0x008
#define FVF_XYZB3            0x00a
#define FVF_XYZB4            0x00c
#define FVF_XYZB5            0x00e
#define FVF_XYZW             0x4002

#define FVF_NORMAL           0x010
#define FVF_PSIZE            0x020
#define FVF_DIFFUSE          0x040
#define FVF_SPECULAR         0x080

#define FVF_TEXCOUNT_MASK    0xf00
#define FVF_TEXCOUNT_SHIFT   8
#define FVF_TEX0             0x000
#define FVF_TEX1             0x100
#define FVF_TEX2             0x200
#define FVF_TEX3             0x300
#define FVF_TEX4             0x400
#define FVF_TEX5             0x500
#define FVF_TEX6             0x600
#define FVF_TEX7             0x700
#define FVF_TEX8             0x800

#define FVF_LASTBETA_UBYTE4   0x1000
#define FVF_LASTBETA_COLOR	  0x8000

#define FVF_RESERVED2         0x6000  // 2 reserved bits
*/

enum VERTEX_FORMAT : unsigned {
	VF_POS		= 0x001,
	VF_POS_PRE	= 0x011,
	VF_NORMAL	= 0x002,
	VF_COLOR	= 0x004,
	VF_TEX		= 0x008
};




// !NOTE! /////////
// could use Lock/Open and Unlock/Close instead of Write
// additional functionality that might be useful: GetRenderer, GetLenght

////////////////////////////////////////////////////////////////////////////////
// Index buffer
class IIndexBuffer {
	public:
		//virtual void Write(void* indexData, unsigned size, unsigned offset=0) = 0;
		virtual int Lock(unsigned offset, unsigned size, void** buffer, unsigned flags) = 0;
		virtual int Unlock() = 0;
		virtual void Release() = 0;

		virtual unsigned Size() = 0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex buffer
class IVertexBuffer {
	public:
		//virtual void Write(void* vertexData, unsigned size, unsigned offset=0) = 0;
		virtual int Lock(unsigned offset, unsigned size, void** buffer, unsigned flags) = 0;
		virtual int Unlock() = 0;
		virtual void Release() = 0;

		virtual unsigned Size() = 0;
		virtual void SetFormat(unsigned fmt) = 0;
};








