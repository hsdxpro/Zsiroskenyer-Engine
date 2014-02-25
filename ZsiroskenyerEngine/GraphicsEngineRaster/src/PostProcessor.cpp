////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer tEAM
////////////////////////////////////////////////////////////////////////////////
//	General post-processor for the renderer
//	Supported effects:
//		- depth of field
//		- motion blur
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEngine.h"

cGraphicsEngine::cPostProcessor::cPostProcessor(cGraphicsEngine& parent)
:parent(parent), gApi(parent.gApi)
{
}

void cGraphicsEngine::cPostProcessor::Update(float deltaT)
{
	/*
	//-------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- MOTION BLUR --- --- --- --- --- --- --- --- //
	//-------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &helperBuffer, NULL);

	gApi->SetShaderProgram(shaderMotionBlur);

	struct s
	{
		Matrix44 invViewProj;
		Matrix44 prevViewProj;
		float	 deltaT; float _pad[3];
	} mbConstants;

	mbConstants.invViewProj = invViewProjMat;
	mbConstants.prevViewProj = prevView * projMat;
	mbConstants.deltaT = deltaT; // FUCCCCCCCKKKKKKK TODO

	gApi->SetPSConstantBuffer(&mbConstants, sizeof(mbConstants), 0);

	gApi->SetTexture(L"textureInput", sourceBuffer);
	gApi->SetTexture(L"depthTexture", depthBuffer);

	// asd new
	prevView = viewMat;

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);

	//----------------------------------------------------------------------------//
	// --- --- --- --- --- --- --- DEPTH OF FIELD --- --- --- --- --- --- --- --- //
	//----------------------------------------------------------------------------//
	gApi->SetRenderTargets(1, &compositionBuffer, NULL);
	gApi->SetShaderProgram(shaderDof);

	struct tDofConstants
	{
	Matrix44 invViewProj;
	Vec3 camPos; float _pad;
	} dofConstants;

	dofConstants.invViewProj = invViewProjMat;
	dofConstants.camPos = cam->GetPos();

	gApi->SetPSConstantBuffer(&dofConstants, sizeof(dofConstants), 0);

	gApi->SetTexture(L"inputTexture",DOFInput);
	gApi->SetTexture(L"depthTexture", depthBufferCopy);

	// Draw triangle, hardware will quadify them automatically :)
	gApi->Draw(3);
	*/
}