////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation
#pragma once

#include "../../Core/src/IShaderProgram.h"
#include "../../Core/src/VertexFormat.h"
#include "../../Core/src/common.h"
#include <unordered_map>

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
class cGraphicsApiD3D11;

class cShaderProgramD3D11 : public IShaderProgram {
	friend class cGraphicsApiD3D11;
public:
	// sampler info
	struct tSamplerInfo {
		uint16_t gApiSamplerIdx;
		uint16_t slotIdx;
		
		tSamplerInfo(uint16_t gApiSamplerIdx, uint16_t slotIdx) :gApiSamplerIdx(gApiSamplerIdx), slotIdx(slotIdx){}
	};

	// ctor/dtor/release
	cShaderProgramD3D11(
		void* vsByteCode, size_t vsByteCodeSize,
		ID3D11VertexShader* vs = NULL, ID3D11HullShader* hs = NULL,
		ID3D11DomainShader* ds = NULL, ID3D11GeometryShader* gs = NULL, 
		ID3D11PixelShader* ps = NULL);
	~cShaderProgramD3D11();
	void Release() override;

	// internal bullshit
	void SetTextureSlotsVS(const std::unordered_map<zsString, uint16_t>& texSlotsVS);
	void SetTextureSlotsPS(const std::unordered_map<zsString, uint16_t>& texSlotsPS);

	void SetSamplerStatesVS(const std::vector<tSamplerInfo>& samplerStatesVS);
	void SetSamplerStatesPS(const std::vector<tSamplerInfo>& samplerStatesPS);

	int GetTextureSlotVS(const zsString& varName) const;
	int GetTextureSlotPS(const zsString& varName) const;

	const std::vector<tSamplerInfo>& GetSamplerStatesVS() const;
	const std::vector<tSamplerInfo>& GetSamplerStatesPS() const;

	const ID3D11VertexShader* GetVS() const;
	const ID3D11HullShader* GetHS() const;
	const ID3D11DomainShader* GetDS() const;
	const ID3D11GeometryShader* GetGS() const;
	const ID3D11PixelShader* GetPS() const;

	// byte codes
	size_t GetVSByteCodeSize() const;
	const void* const GetVSByteCode() const;

	// format
	cVertexFormat GetInputVertexFormat() const;

protected:
	// In Sequence of the Dx pipeline :) 
	ID3D11VertexShader*		vs;
	void*					vsByteCode;
	size_t					vsByteCodeSize;

	ID3D11HullShader*		hs;
	ID3D11DomainShader*		ds;
	ID3D11GeometryShader*	gs;
	ID3D11PixelShader*		ps;


	// Texture name, slot idx
	std::unordered_map<zsString, uint16_t> textureSlotsVS;
	std::unordered_map<zsString, uint16_t> textureSlotsPS;

	// Sampler slot look up
	std::vector<tSamplerInfo> samplerStatesVS;
	std::vector<tSamplerInfo> samplerStatesPS;

	// vertex format
	cVertexFormat format;
};