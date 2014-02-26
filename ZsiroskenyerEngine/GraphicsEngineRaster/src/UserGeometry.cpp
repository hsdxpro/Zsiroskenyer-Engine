////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Boilerplate, boilerplate everywhere.
////////////////////////////////////////////////////////////////////////////////

#include "UserGeometry.h"
#include "ResourceManager.h"


////////////////////////////////////////////////////////////////////////////////
// ctor
cUserGeometry::cUserGeometry(cResourceManager& rm)
:
rm(rm),
builder(rm.GetGraphicsApi())
{
}


////////////////////////////////////////////////////////////////////////////////
// attribs

// vertex data
void cUserGeometry::SetVertexFormat(cVertexFormat format) {
	builder.SetVertexFormat(format);
}
void cUserGeometry::InsertVertices(const void* verts, size_t bytes, size_t offset) {
	builder.InsertVertices(verts, bytes, offset);
}
void cUserGeometry::EraseVertices(size_t offset, size_t bytes) {
	builder.EraseVertices(offset, bytes);
}
void* cUserGeometry::GetVertices() {
	return builder.GetVertices();
}
size_t cUserGeometry::GetNumVertices() {
	return builder.GetNumVertices();
}
cVertexFormat cUserGeometry::GetVertexFormat() const {
	return builder.GetVertexFormat();
}
// index data
void cUserGeometry::InsertIndices(uint32_t* indices, size_t count, size_t position) {
	builder.InsertIndices(indices, count, position);
}
void cUserGeometry::EraseIndices(size_t position, size_t count) {
	builder.EraseIndices(position, count);
}
uint32_t* cUserGeometry::GetIndices() {
	return builder.GetIndices();
}
size_t cUserGeometry::GetNumIndices() {
	return builder.GetNumIndices();
}
// material groups
void cUserGeometry::AddMatGroup(size_t id, size_t indexOffset, size_t indexCount) {
	builder.AddMatGroup(id, indexOffset, indexCount);
}
void cUserGeometry::EraseMatGroup(size_t idx) {
	builder.EraseMatGroup(idx);
}
auto cUserGeometry::GetMatGroups() -> tMatGroup* {
	static_assert(sizeof(tMatGroup) == sizeof(_cGeometryBuilder::tMatGroup), "tMatGroup sizes mismatch!");
	return (tMatGroup*)builder.GetMatGroups();
}
size_t cUserGeometry::GetNumMatGroups() {
	return builder.GetNumMatGroups();
}


////////////////////////////////////////////////////////////////////////////////
// other

// file handling
void cUserGeometry::LoadFile(const wchar_t* path) {
	builder.LoadFile(path);
}

// clone
void cUserGeometry::Clone(IGeometryBuilder* other) {
	builder = ((cUserGeometry*)other)->builder;
}

// submit
IGeometryRef* cUserGeometry::Submit(const wchar_t* name) {
	try {
		cGeometry* geom = builder.Create();
		cGeometryRef gref = rm.AddGeometry(geom, name);
		auto ugref = new cUserGeometryRef(gref);
		return ugref;
	}
	catch (std::exception& e) {
		errorMessage = e.what();
		return nullptr;
	}
}

// error messages
const char* cUserGeometry::YUNoWorkBitch() {
	return errorMessage.c_str();
}

// reset
void cUserGeometry::Reset() {
	builder.Reset();
}

// release
void cUserGeometry::Release() {
	delete this;
}