#include "Renderer.h"

#include "IGraphicsApi.h"
#include "IVertexBuffer.h"


cRenderer::cRenderer(IGraphicsApi* gApi, IShaderManager* shaderManager)
:gApi(gApi), shaderManager(shaderManager) {
}

void cRenderer::RenderLines(const Matrix44& viewProj, const Vec3* lines, size_t nLines, const Vec3& color /*= Vec3(1.0f, 1.0f, 1.0f)*/) {
#pragma message("RENDERER_ERROR: IShaderManager wiped completely!")
	// no shaders
	return;
	
	// Create, set VertexBuffer for lines
	cVertexFormat::Attribute a;
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 3;
		a.semantic = cVertexFormat::POSITION;
		a.type = cVertexFormat::FLOAT;
		cVertexFormat f({ a });

	IVertexBuffer* linesBuffer;
	gApi->CreateVertexBuffer(&linesBuffer, eUsage::IMMUTABLE, f, nLines * 2 * sizeof(Vec3), (void*)lines);
	gApi->SetVertexBuffer(linesBuffer);

	// Set camera constants
	gApi->SetVSConstantBuffer(&viewProj, sizeof(Matrix44), 0);
	gApi->SetPSConstantBuffer(&color, sizeof(Vec3), 0);

	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Set Shader
#pragma message("RENDERER_ERROR: IShaderManager wiped completely!")
	IShaderProgram* sh = nullptr;
	gApi->SetShaderProgram(sh);

	// Set Line primitives for pipeline
	gApi->SetPrimitiveTopology(ePrimitiveTopology::LINE_LIST);

	// Draw lines
	gApi->Draw( nLines * 2);

	// Free up buffers
	SAFE_RELEASE(linesBuffer);

	// // Set TRIANGLE primitives for pipeline
	gApi->SetPrimitiveTopology(ePrimitiveTopology::TRIANGLE_LIST);
}
