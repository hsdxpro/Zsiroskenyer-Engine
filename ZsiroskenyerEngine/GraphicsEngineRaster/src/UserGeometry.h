////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Bullshit dumbfuck boilerplate wrapper for cGeometryBuilder to expose dat
//	interface.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "_GeometryBuilder.h"
#include "../../Core/src/ICustomGeometry.h"

class cResourceManager;

class cUserGeometry : public ICustomGeometry {
public:
	// ctor
	cUserGeometry(cResourceManager& rm);

	// vertex data
	void SetVertexStride(size_t size) override;
	void InsertVertices(const void* verts, size_t bytes, size_t offset = 0) override;
	void EraseVertices(size_t offset, size_t bytes) override;
	void* GetVertices() override;
	size_t GetNumVertices() override;
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
	void Clone(ICustomGeometry* other) override;

	// submit or reset
	IGeometryRef* Submit(const wchar_t* name) override;
	void Reset()  override;

	// release
	void Release() override;
private:
	cResourceManager& rm;
	_cGeometryBuilder builder;
};
