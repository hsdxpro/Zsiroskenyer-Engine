////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Bullshit dumbfuck boilerplate wrapper for cGeometryBuilder to expose dat
//	interface.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "_GeometryBuilder.h"
#include "ResourceReference.h"
#include "../../Core/src/IGeometryBuilder.h"

class cResourceManager;


class cUserGeometryRef : public cGeometryRef, public IGeometryRef {
public:
	cUserGeometryRef() : cGeometryRef() {}
	cUserGeometryRef(const cGeometryRef& ref) : cGeometryRef(ref) {}
	void Release() override {
		delete this;
	}
};


class cUserGeometry : public IGeometryBuilder {
public:
	// ctor
	cUserGeometry(cResourceManager& rm);

	// vertex data
	void SetVertexFormat(cVertexFormat format) override;
	void InsertVertices(const void* verts, size_t bytes, size_t offset = 0) override;
	void EraseVertices(size_t offset, size_t bytes) override;
	void* GetVertices() override;
	size_t GetNumVertices() override;
	cVertexFormat GetVertexFormat() const override;

	// index data
	void InsertIndices(uint32_t* indices, size_t count, size_t position = 0) override;
	void EraseIndices(size_t position, size_t count) override;
	uint32_t* GetIndices() override;
	size_t GetNumIndices() override;
	// material groups
	void AddMatGroup(size_t id, size_t indexOffset, size_t indexCount) override;
	void EraseMatGroup(size_t idx) override;
	tMatGroup* GetMatGroups() override;
	size_t GetNumMatGroups() override;

	// file handling
	void LoadFile(const wchar_t* path) override;

	// clone
	void Clone(IGeometryBuilder* other) override;

	// submit or reset
	IGeometryRef* Submit(const wchar_t* name) override;
	void Reset() override;

	// politely ask for an error message
	const char* YUNoWorkBitch() override;

	// release
	void Release() override;
private:
	cResourceManager& rm;
	_cGeometryBuilder builder;
	std::string errorMessage;
};
