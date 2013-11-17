#include "Renderer.h"

#include "IGraphicsApi.h"
#include "IShaderManager.h"
#include "IVertexBuffer.h"
#include "IConstantBuffer.h"

cRenderer::cRenderer(IGraphicsApi* gApi, IShaderManager* shaderManager)
:gApi(gApi), shaderManager(shaderManager) {
}

void cRenderer::RenderLines(const Matrix44& viewProj, const Vec3* lines, size_t nLines, const Vec3& color /*= Vec3(1.0f, 1.0f, 1.0f)*/) {
	// Create, set VertexBuffer for lines
	IVertexBuffer* linesBuffer = gApi->CreateVertexBuffer(nLines * 2 * sizeof(Vec3), eUsage::IMMUTABLE, (void*)lines);
	gApi->SetVertexBuffer(linesBuffer, sizeof(Vec3));

	// Set camera constants
	IConstantBuffer* viewProjBuffer = gApi->CreateConstantBuffer(sizeof(Matrix44), eUsage::DEFAULT, (void*)&viewProj);
	gApi->SetVSConstantBuffer(viewProjBuffer, 0);

	IConstantBuffer* colorBuffer = gApi->CreateConstantBuffer(sizeof(Vec3), eUsage::DEFAULT, (void*)&color);
	gApi->SetPSConstantBuffer(colorBuffer, 0);

	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Set Shader
	IShaderProgram* sh = shaderManager->GetShaderByName(L"LINE_RENDERER.cg");
	gApi->SetShaderProgram(sh);

	// Set Line primitives for pipeline
	gApi->SetPrimitiveTopology(ePrimitiveTopology::LINE_LIST);

	// Draw lines
	gApi->Draw(nLines * 2);

	// Free up buffers
	linesBuffer->Release();
	colorBuffer->Release();
	viewProjBuffer->Release();

	// // Set TRIANGLE primitives for pipeline
	gApi->SetPrimitiveTopology(ePrimitiveTopology::TRIANGLE_LIST);
}
