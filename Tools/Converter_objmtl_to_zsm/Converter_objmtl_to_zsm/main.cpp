#include <iostream>
#include <list>
#include <windows.h>
#include <cassert>
#include <fstream>
#include <string>

void main() {
	
	WIN32_FIND_DATA findData;

	// Collect .obj filePaths in that folder
	std::list<std::string> objFilePaths;

	char currDir[512];
	GetCurrentDirectoryA(512, currDir);
	std::string workingDir = currDir; workingDir += '\\';
	std::string searchDir = workingDir;
	searchDir += "*";

	HANDLE h = FindFirstFile(searchDir.c_str(), &findData);
	assert(h != INVALID_HANDLE_VALUE);

	do 
	{
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::string fileName = findData.cFileName;
			if (fileName.find(".obj") != std::string::npos) {
				objFilePaths.push_back(workingDir + fileName);
				std::wcout << "Found: "<<findData.cFileName << std::endl;
			}
		}
	} while (FindNextFile(h, &findData));

	// Search for .obj files
	// RESULT need to be
	//id 0
	//diffuse 0.9 0.9 0.9 1
	//specular 1 1 1 1
	//emissive 0 0 0 0
	//glossiness 0.5
	//tdiffuse .. / textures / crate.jpg
	//tnormal
	//tspecular
	//tdisplace
	for (auto obj_filePath : objFilePaths) {

		// blabla\file
		std::string basePath = obj_filePath.substr(0, obj_filePath.find(L'.'));
		
		// blabla\file.mtl
		std::string mtlPath = basePath + ".mtl";

		// Open .mtl
		std::ifstream is(mtlPath);

		std::string alphaFile;
		std::string diffuseFile;
		std::string normalFile;
		std::string specularFile;

		// .mtl exists for that .obj
		if (is.is_open()) {
			std::string line;
			while (std::getline(is, line)) {

				// Currently handling diffuse normal and specular
				if (line.find("map_Ka") != std::string::npos) {
					// Implement please...
				} else if (line.find("map_Kd") != std::string::npos) {
					// Handling
					auto dirChIdx = line.find_last_of('\\');
					if (dirChIdx != std::string::npos) {
						diffuseFile = line.substr(dirChIdx + 1, line.size() - dirChIdx - 1);
					} else { // root dir
						auto spaceIdx = line.find_last_of(' ');
						if (spaceIdx != std::string::npos)
							diffuseFile = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
					}
					
				} else if (line.find("map_Ks") != std::string::npos) {
					// Handling
					auto dirChIdx = line.find_last_of('\\');
					if (dirChIdx != std::string::npos) {
						specularFile = line.substr(dirChIdx + 1, line.size() - dirChIdx - 1);
					} else { // root dir
						auto spaceIdx = line.find_last_of(' ');
						if (spaceIdx != std::string::npos)
							specularFile = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
					}
				} else if (line.find("map_bump") != std::string::npos) {
					// Handling
					auto dirChIdx = line.find_last_of('\\');
					if (dirChIdx != std::string::npos) {
						normalFile = line.substr(dirChIdx + 1, line.size() - dirChIdx - 1);
					} else { // root dir
						auto spaceIdx = line.find_last_of(' ');
						if (spaceIdx != std::string::npos) 
							normalFile = line.substr(spaceIdx + 1, line.size() - spaceIdx - 1);
					}
				}
			}
		}

		std::string zsmPath = basePath + ".zsm";

		std::ofstream os(zsmPath, std::ios::trunc);

		os << "id 0" << std::endl;
		os << "diffuse 0.9 0.9 0.9 1" << std::endl;
		os << "specular 1 1 1 1" << std::endl;
		os << "emissive 0 0 0 0" << std::endl;
		os << "glossiness 0.5" << std::endl;
		os << "tdiffuse " << diffuseFile << std::endl;
		os << "tnormal " << normalFile << std::endl;
		os << "tspecular " << specularFile << std::endl;
		os << "tdisplace " << specularFile <<std::endl;
#pragma warning("tDisplace now get specular map path....")

		os.close();
	}

	std::cin.get();
}