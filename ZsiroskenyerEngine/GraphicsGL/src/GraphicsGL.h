// GraphicsGL.h By Zsíroskenyér team
#pragma once

#include "..\..\GraphicsEngine\src\IGraphicsApi.h"
#include "..\..\GraphicsEngine\src\IWindow.h"
#include "..\..\CommonLib\src\common.h"

class cGraphicsGL : public IGraphicsApi {
public:
	// Creating Buffers
	IVertexBuffer* CreateVertexBuffer(size_t size, eBufferUsage usage, void* data = NULL) override;
	IIndexBuffer* CreateIndexBuffer(size_t size, eBufferUsage usage, void* data = NULL);

	// Write to buffers
	bool WriteBuffer(IIndexBuffer* buffer , void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;
	bool WriteBuffer(IVertexBuffer* buffer, void* source, size_t size = ZS_NUMLIMITMAX(size_t), size_t offset = 0) override;

	// Read from buffers
	bool ReadBuffer(IIndexBuffer* buffer , void* dest, size_t size, size_t offset = 0) override;
	bool ReadBuffer(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	

	// SwapChain modifications
	void BBClear(bool clearOnlyDepth = false) override;
	void BBPresent() override;
	void SetWindow(IWindow *renderWindow) override;
protected:
};


// DLL accessor
extern "C"
	__declspec(dllexport)
	IGraphicsApi* CreateGraphicsGL() {
		return new cGraphicsGL();
}