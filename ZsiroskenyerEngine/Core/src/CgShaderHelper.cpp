#include "CgShaderHelper.h"
#include "Cg/cgGL.h"

#include "StrUtil.h"
#include "FileUtil.h"

// Windows based because of cgc.exe
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


cCgShaderHelper::tCgInfo cCgShaderHelper::LoadCgShader(const zsString& shaderPath) {
	loadedFilePath = shaderPath;

	// Setup context, for creation
	con = cgCreateContext();
	cgGLRegisterStates(con);
	cgGLSetManageTextureParameters(con, CG_TRUE);

	// Effect creation
	char ansiShaderPath[256];
	cStrUtil::ToAnsi(shaderPath, ansiShaderPath, 256);
	effect = cgCreateEffectFromFile(con, ansiShaderPath, NULL);
	if (effect == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return cCgShaderHelper::tCgInfo();
	}

	// Tech creation
	tech = cgGetFirstTechnique(effect);
	if (tech == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return cCgShaderHelper::tCgInfo();
	}

	// Pass creation
	pass = cgGetFirstPass(tech);
	if (pass == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return cCgShaderHelper::tCgInfo();
	}

// Read programs
	// VertexShader
	vs = cgGetPassProgram(pass, CGdomain::CG_VERTEX_DOMAIN);

	// GeometryShader
	gs = cgGetPassProgram(pass, CGdomain::CG_GEOMETRY_DOMAIN);

	// PixelShader
	ps = cgGetPassProgram(pass, CGdomain::CG_FRAGMENT_DOMAIN);

	hs = cgGetPassProgram(pass, CGdomain::CG_TESSELLATION_CONTROL_DOMAIN);

	ds = cgGetPassProgram(pass, CGdomain::CG_TESSELLATION_EVALUATION_DOMAIN);

	cCgShaderHelper::tCgInfo info; // result
		info.vsExists = (vs != NULL) ? true : false;
		info.hsExists = (hs != NULL) ? true : false;
		info.dsExists = (ds != NULL) ? true : false;
		info.gsExists = (gs != NULL) ? true : false;
		info.psExists = (ps != NULL) ? true : false;
		info.vsEntryName = (vs != NULL) ? cgGetProgramString(vs, CGenum::CG_PROGRAM_ENTRY) : "";
		info.hsEntryName = (hs != NULL) ? cgGetProgramString(hs, CGenum::CG_PROGRAM_ENTRY) : "";
		info.dsEntryName = (ds != NULL) ? cgGetProgramString(ds, CGenum::CG_PROGRAM_ENTRY) : "";
		info.gsEntryName = (gs != NULL) ? cgGetProgramString(gs, CGenum::CG_PROGRAM_ENTRY) : "";
		info.psEntryName = (ps != NULL) ? cgGetProgramString(ps, CGenum::CG_PROGRAM_ENTRY) : "";
	return info;
}


bool cCgShaderHelper::CompileCg(const zsString& cgFilePath, const zsString& shaderOut, cCgShaderHelper::eProfileCG compileProfile) {
	// Paths
	zsString cgcExePath = L"bin\\cgc.exe";
	zsString entryAndProfile;
	switch (compileProfile)
	{
	case eProfileCG::VS_5_0:
		entryAndProfile = L"-profile vs_5_0 -entry VS_MAIN";
		break;
	case eProfileCG::HS_5_0:
		entryAndProfile = L"-profile hs_5_0 -entry HS_MAIN";
		break;
	case eProfileCG::DS_5_0:
		entryAndProfile = L"-profile ds_5_0 -entry DS_MAIN";
		break;
	case eProfileCG::GS_5_0:
		entryAndProfile = L"-profile gs_5_0 -entry GS_MAIN";
		break;
	case eProfileCG::PS_5_0:
		entryAndProfile = L"-profile ps_5_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_4_0:
		entryAndProfile = L"-profile vs_4_0 -entry VS_MAIN";
		break;
	case eProfileCG::HS_4_0:
		entryAndProfile = L"-profile hs_4_0 -entry HS_MAIN";
		break;
	case eProfileCG::DS_4_0:
		entryAndProfile = L"-profile ds_4_0 -entry DS_MAIN";
		break;
	case eProfileCG::GS_4_0:
		entryAndProfile = L"-profile gs_4_0 -entry GS_MAIN";
		break;
	case eProfileCG::PS_4_0:
		entryAndProfile = L"-profile ps_4_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_3_0:
		entryAndProfile = L"-profile vs_3_0 -entry VS_MAIN";
		break;
	case eProfileCG::PS_3_0:
		entryAndProfile = L"-profile ps_3_0 -entry PS_MAIN";
		break;
	case eProfileCG::VS_2_0:
		entryAndProfile = L"-profile vs_2_0 -entry VS_MAIN";
		break;
	case eProfileCG::PS_2_0:
		entryAndProfile = L"-profile ps_2_0 -entry PS_MAIN";
		break;
	}
	//cgc.exe proba.fx -profile vs_5_0 -entry MAIN -o proba.vs
	zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -o " + shaderOut;

	// Process infos
	STARTUPINFO StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	PROCESS_INFORMATION ProcessInfo;

	// LPWCSTR to LPWSTR
	wchar_t params[512];
	wcscpy(params, shellParams.c_str());

	// Start cgc.exe and Generate .hlsl from .cg
	BOOL appStarted = CreateProcessW(cgcExePath.c_str(), params, NULL, NULL, false, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
	if (!appStarted) {
		lastErrorMsg = zsString(L"cannot find cg compiler @") + cgcExePath;
		return false;
	}

	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	return true;
}

std::map<zsString, size_t> cCgShaderHelper::GetHLSLTextureSlots(const zsString& hlslFilePath) {
	std::map<zsString, size_t> result;

	// Parse hlsl code for samplers, textures
	std::ifstream hlslFile(hlslFilePath);

	std::map<zsString, size_t> textureSlotsParsed;

	size_t texIdx = 0;
	bool reachTextures = false;
	bool reachSampling = false;

	auto lines = cFileUtil::GetLines(hlslFile);
	for (auto it = lines.begin(); it != lines.end(); it++) {
		const zsString& row = *it;

		// Collect <texture names, slot numbers>
		if (!reachSampling && cStrUtil::Begins(row, L"Texture")) {
			textureSlotsParsed[cStrUtil::Between(row, L' ', L';')] = texIdx++;
			reachTextures = true;
		}

		// match textures, samplers
		if (reachTextures && cStrUtil::Contains(row, L".Sample")) {
			reachSampling = true;
			// Example : _pout._color = _TMP23.Sample(_diffuseTex, _In._tex01);
			size_t chPos = cStrUtil::Find(row, L".Sample");
			zsString textureName = cStrUtil::SubStrLeft(row, chPos - 1, '_');
			zsString samplerName = cStrUtil::SubStrRight(row, chPos + 9, ',', -1);

			result[samplerName] = textureSlotsParsed[textureName];
		}
	}

	hlslFile.close();
	return result;
}

const wchar_t* cCgShaderHelper::GetLastErrorMsg() {
	return lastErrorMsg.c_str();
}