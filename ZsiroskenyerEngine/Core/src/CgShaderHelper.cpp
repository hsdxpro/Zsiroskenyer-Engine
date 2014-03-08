#include "CgShaderHelper.h"
#include "Cg/cgGL.h"

#include "StrUtil.h"
#include "FileUtil.h"

// Windows based because of cgc.exe
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>


cCgShaderHelper::cCgShaderHelper(const zsString& shaderPath) {

	cFileUtil::ReplaceIncludeDirectives(shaderPath, cgFileLines, includedFilesPaths);

	// Setup context, for creation (Yeah silly, but use OpenGL cg state and params) because equal with dx...
	CGcontext con = cgCreateContext();
	cgGLRegisterStates(con);
	cgGLSetManageTextureParameters(con, CG_TRUE);

	// Create cg effect from file (load)
	assert(shaderPath.size() <= 256);
	char ansiShaderPath[256];
	cStrUtil::ToAnsi(shaderPath, ansiShaderPath, 256);
	CGeffect effect = cgCreateEffectFromFile(con, ansiShaderPath, NULL);
	if (effect == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return;
	}

	// Tech creation
	CGtechnique tech = cgGetFirstTechnique(effect);
	if (tech == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return;
	}

	// Pass creation
	CGpass pass = cgGetFirstPass(tech);
	if (pass == NULL) {
		lastErrorMsg = cgGetLastListing(con);
		return;
	}

	CGprogram shaderPrograms[NDOMAINS];
		shaderPrograms[VS] = cgGetPassProgram(pass, CGdomain::CG_VERTEX_DOMAIN);
		shaderPrograms[HS] = cgGetPassProgram(pass, CGdomain::CG_TESSELLATION_CONTROL_DOMAIN);
		shaderPrograms[DS] = cgGetPassProgram(pass, CGdomain::CG_TESSELLATION_EVALUATION_DOMAIN);
		shaderPrograms[GS] = cgGetPassProgram(pass, CGdomain::CG_GEOMETRY_DOMAIN);
		shaderPrograms[PS] = cgGetPassProgram(pass, CGdomain::CG_FRAGMENT_DOMAIN);


	// Domain infos
	for (uint8_t i = 0; i < NDOMAINS; i++) {
		// Domain existence
		info.domainsExist[i] = shaderPrograms[i] != NULL;

		// if exist save entry name
		if (info.domainsExist[i]) 
			info.domainsEntry[i] = cgGetProgramString(shaderPrograms[i], CGenum::CG_PROGRAM_ENTRY);
	}
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
	//zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -unroll all count=50 -o " + shaderOut;
	zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -unroll none -inline none -o " + shaderOut;
	//zsString shellParams = cgcExePath + L" " + cgFilePath + L" " + entryAndProfile + L" -unroll none -inline all -o " + shaderOut;

	// Process info
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

std::unordered_map<zsString, uint16_t> cCgShaderHelper::GetHLSLTextureSlots(const zsString& hlslFilePath) {
	// Parse hlsl code for samplers, textures
	std::ifstream hlslFile(hlslFilePath);

	// Result
	std::unordered_map<zsString, uint16_t> result;

	// Tmp holder
	std::unordered_map<zsString, uint16_t> textureSlotsParsed;

	uint16_t texIdx = 0;
	bool reachTextures = false;
	bool reachSampling = false;

	auto lines = cFileUtil::GetLines(hlslFile);
	for (auto it = lines.begin(); it != lines.end(); it++) {
		const zsString& row = *it;

		// Collect <texture names, slot numbers>
		if (!reachSampling && cStrUtil::Begins(row, L"Texture")) {

			// TODO JESUS CHRISTS, WHY SEARCH FROM FRONT FOR '[' CHAR, "Texture" Already found, and we can get the index of it
			int bracketIdx = cStrUtil::Find(row, '[');
			if ( bracketIdx >= 0) {
				wchar_t tmp[3];
				uint16_t nElements;
				cStrUtil::LastNumber(row, nElements, tmp, 3);
				
				textureSlotsParsed[cStrUtil::Between(row, L' ', L'[')] = texIdx;
				texIdx += nElements;
			} else {
				textureSlotsParsed[cStrUtil::Between(row, L' ', L';')] = texIdx++;
			}

			reachTextures = true;
		}

		// match textures, samplers
		int chPos = cStrUtil::Find(row, L".Sample");
		if (reachTextures && chPos >= 0) {
			reachSampling = true;

			zsString textureName;
			zsString samplerName;

			// "].Sample"
			// Found arrays of samplers...
			if (row[chPos - 1] == ']') {
				// Example : _pout._color = _TMP23[0].Sample(_shadowMaps[1], _In._tex01);
				int leftBracketPos = cStrUtil::FindLeft(row, chPos, '[');

				textureName = cStrUtil::SubStrLeft(row, leftBracketPos - 1, '_');
				samplerName = cStrUtil::SubStrRight(row, chPos + 9, '[', -1);
			}
			else {
				// Example : _pout._color = _TMP23.Sample(_diffuseTex, _In._tex01);
				textureName = cStrUtil::SubStrLeft(row, chPos - 1, '_');
				samplerName = cStrUtil::SubStrRight(row, chPos + 9, ',', -1);
			}
			result[samplerName] = textureSlotsParsed[textureName];
		}
	}

	hlslFile.close();
	return result;
}

const std::list<zsString>& cCgShaderHelper::GetIncludedFilesPaths() const {
	return includedFilesPaths;
}

std::unordered_map<zsString, tSamplerDesc> cCgShaderHelper::GetSamplerStates() {
	std::unordered_map<zsString, tSamplerDesc> result;

	// Lines that contains "sampler" and "=", contains sampler states under that
	const zsString words[2] = { L"sampler", L"=" };
	auto samplerLineIndices = cStrUtil::GetLinesContainingAllStr(cgFileLines, words, 2);

	// For each sampler line that uses SamplerStates 
	//for (auto it = samplerLineIndices.begin(); it != samplerLineIndices.end(); it++) {
	for (auto lineIdx : samplerLineIndices) {
		// Sampler Name
		
		const zsString& _samplerName = *std::next(cgFileLines.begin(), lineIdx);
		zsString samplerName = _samplerName;
		cStrUtil::TrimBorder(samplerName, ' ');
		cStrUtil::Between(samplerName, ' ', ' ');


		const auto samplerStateLines = cStrUtil::GetLines(cgFileLines, lineIdx + 1, L";");

		// TODO Noob mode : assume one state per line
		// ex.
		// MipFilter = POINT,
		// MinFilter = POINT,
		// MagFilter = POINT,

		tSamplerDesc samplerDesc;

		// For each of the above lines
		for (auto state : samplerStateLines) {
			const zsString& row = *std::next(cgFileLines.begin(), state);
			if (row.size() == 0)
				continue;

			// ex. "MipFilter = POINT,", split, trim to "MipFilter", "POINT", then lower those
			std::list<zsString> parts = cStrUtil::SplitAt(row, '=');
			const wchar_t borders[5] = { ' ', ',', '\t', '{', '}'};
			cStrUtil::TrimBorder(parts, borders, 5);
			cStrUtil::ToUpper(parts);

			auto it = parts.begin();
			const zsString& left = *it++;
			const zsString& right = *it;

			if (left == L"MIPFILTER") {
				if (right == L"POINT") {
					samplerDesc.filterMip = eFilter::POINT;
				}
				else if (right == L"LINEAR") {
					samplerDesc.filterMip = eFilter::LINEAR;
				}
				else if (right == L"ANISOTROPIC") {
					samplerDesc.filterMip = eFilter::ANISOTROPIC;
				}

			}
			else if (left == L"MINFILTER") {
				if (right == L"POINT") {
					samplerDesc.filterMin = eFilter::POINT;
				}
				else if (right == L"LINEAR") {
					samplerDesc.filterMin = eFilter::LINEAR;
				}
				else if (right == L"ANISOTROPIC") {
					samplerDesc.filterMin = eFilter::ANISOTROPIC;
				}

			}
			else if (left == L"MAGFILTER") {
				if (right == L"POINT") {
					samplerDesc.filterMag = eFilter::POINT;
				}
				else if (right == L"LINEAR") {
					samplerDesc.filterMag = eFilter::LINEAR;
				}
				else if (right == L"ANISOTROPIC") {
					samplerDesc.filterMag = eFilter::ANISOTROPIC;
				}
			}
			else if (left == L"ADDRESSU") {
				if (right == L"CLAMP") {
					samplerDesc.addressU = eAddress::CLAMP;
				}
				else if (right == L"WRAP") {
					samplerDesc.addressU = eAddress::WRAP;
				}
				else if (right == L"MIRROR") {
					samplerDesc.addressU = eAddress::MIRROR;
				}
			}
			else if (left == L"ADDRESSV") {
				if (right == L"CLAMP") {
					samplerDesc.addressV = eAddress::CLAMP;
				}
				else if (right == L"WRAP") {
					samplerDesc.addressV = eAddress::WRAP;
				}
				else if (right == L"MIRROR") {
					samplerDesc.addressV = eAddress::MIRROR;
				}
			}
		}
		result[samplerName] = samplerDesc;
	}
	return result;
}


const wchar_t* cCgShaderHelper::GetLastErrorMsg() {
	return (lastErrorMsg.size() == 0 ) ? NULL : lastErrorMsg.c_str();
}

cVertexFormat cCgShaderHelper::GetVSInputFormat()
{
	// Parse input Layout... from VERTEX_SHADER
	// - 1. search for vertexShader Entry name ex:"VS_MAIN(", get return value, for example struct name VS_OUT, or valid params like float4
	// - 2. search for VS_OUT, get lines under that, while line != "};"
	// - 3. extract VERTEX DECLARATION from those lines

	// TODOOOOOOOOOOOOOOOOOOOOOOO Found info.domainsEntry[VS] && Find'(' but next character is not')'
	zsString vsEntryLine = cStrUtil::GetWordAfter(cgFileLines, info.domainsEntry[VS] + L"(");
	std::list<zsString> vsInStructLines = cStrUtil::GetLinesBetween(cgFileLines, vsEntryLine, L"};");

	// (inputLayout)  Vertex shader input format
	std::vector<cVertexFormat::Attribute> attribs;
	cVertexFormat::Attribute tmpAttrib;
	uint16_t attribIdx = 0;
	auto it = vsInStructLines.begin();
	while (it != vsInStructLines.end()) {
		// not empty line... Parse Vertex Declaration
		if (it->size() != 0) {
			char semanticNames[10][32]; // Max 10 semantic, each 32 word length
			char semanticIndex[3]; // 999 max

			cStrUtil::GetWordBetween(*it, ':', ';', semanticNames[attribIdx]);
			cStrUtil::GetNumberFromEnd(semanticNames[attribIdx], semanticIndex);
			cStrUtil::CutNumberFromEnd(semanticNames[attribIdx]);

			if (strcmp(semanticNames[attribIdx], "POSITION") == 0)
				tmpAttrib.semantic = cVertexFormat::POSITION;
			else if (strcmp(semanticNames[attribIdx], "NORMAL") == 0)
				tmpAttrib.semantic = cVertexFormat::NORMAL;
			else if (strcmp(semanticNames[attribIdx], "COLOR") == 0)
				tmpAttrib.semantic = cVertexFormat::COLOR;
			else if (strcmp(semanticNames[attribIdx], "TEXCOORD") == 0)
				tmpAttrib.semantic = cVertexFormat::TEXCOORD;
			else
			{
				lastErrorMsg = zsString("Zsiroskenyer Engine does not support that vertex shader input format semantic name :( -> ") + zsString(semanticNames[attribIdx]);
				assert(0);
			}


			if (it->find(L"float4") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 4;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (it->find(L"float3") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 3;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (it->find(L"float2") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 2;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else if (it->find(L"float") != std::wstring::npos) {
				tmpAttrib.bitsPerComponent = cVertexFormat::_32_BIT;
				tmpAttrib.nComponents = 1;
				tmpAttrib.type = cVertexFormat::FLOAT;
			}
			else
			{
				lastErrorMsg = L"Cg file parsing: Failed to determine vertex shader input attribute format type (float1, 2, 3, 4) supported lol";
				assert(0);
			}

			attribs.push_back(tmpAttrib);
		}

		attribIdx++;
		it++;
	}

	// Vertex Shader input format
	cVertexFormat vsInputFormat;
	vsInputFormat.Create(attribs);

	return vsInputFormat;
}

const cCgShaderHelper::tCgInfo& cCgShaderHelper::GetDomainInfo() {
	return info;
}