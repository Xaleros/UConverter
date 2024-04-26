#include <iostream>
#include "UnrealLodMesh.h"
#include "idTechMd2.h"

struct FAssetType {
	char name[8];
	EAssetType type;
	char* desc;
};

FAssetType typeTable[] = {
	{"3d",  AST_MODEL, "Unreal LodMesh"},
	{"mdl", AST_MODEL, "idTech 1 (Quake) Model"},
	{"md2", AST_MODEL, "idTech 2 (Quake 2) Model"},
	{"md3", AST_MODEL, "idTech3 Model"},
	{"psk", AST_MODEL, "Unreal Skeletal Mesh"},
	{"psa", AST_MODEL, "Unreal Skeletal Mesh"}
};

#ifndef _WIN32
int strnicmp(const char* s1, const char* s2, size_t n) {
	for (int i = 0; i < n; i++) {
		char c1 = tolower(s1[i]);
		char c2 = tolower(s2[i]);
		if (c1 != c2) {
			return static_cast<int>(c1) - static_cast<int>(c2);
		}
	}

	return 0;
}
#endif

std::string GetFileExt(const std::string& path) {
	std::string ext("");
	size_t extDot = path.find_last_of('.');
	if (extDot != std::string::npos) {
		ext = path.substr(extDot + 1);
	}
	return ext;
}

static EAssetType GetAssetType(const char* type) {
	for (FAssetType t : typeTable) {
		if (strnicmp(type, t.name, strlen(t.name)) == 0) {
			return t.type;
		}
	}

	return AST_NONE;
}

static void PrintAssetTypeHelp() {
	std::cout << "Model Types:" << std::endl;
	for (FAssetType t : typeTable) {
		std::cout << "\t" << t.name << " - " << t.desc;
	}
}

static void PrintHelp() {
	std::cout << "usage" << std::endl;
	std::cout << "UConverter [-i <input path>] [-o <output path>] <args>" << std::endl << std::endl;
	std::cout << "optional arguments:" << std::endl;
	std::cout << "\t-t <asset type>" << std::endl;
	std::cout << "\t-T <path>" << std::endl;
}

static int Error(const char* msg, int help = 0) {
	std::cout << msg << std::endl << std::endl;
	PrintHelp();
	return -1;
}

void NormalizeDirectoryString(std::string& s) {
#ifdef _WIN32
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == '/') {
			s[i] = '\\';
		}
	}
#endif
}

int main(int argc, char** argv) {
	bool testMode = false;
	std::string outputPath;
	std::string assetPath;
	std::string assetExt;
	EAssetType assetType = AST_NONE;

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		if (arg[0] == '-') {
			i++;
			switch (arg[1]) {
			case 'T':
				testMode = true;
				assetPath.assign(argv[i]);
				break;
			case 't':
				assetExt = argv[i];
				assetType = GetAssetType(argv[i]);
				break;
			case 'i':
				assetPath.assign(argv[i]);
				break;
			case 'o':
				outputPath.assign(argv[i]);
				break;
			}
		}
	}

	if (assetPath.size() == 0) {
		return Error("Required argument [-i <input path>] missing", 1);
	}

	NormalizeDirectoryString(assetPath);

	if (outputPath.size() == 0 && !testMode) {
		return Error("Required argument [-o <output path>] missing", 1);
	}
	else {
		NormalizeDirectoryString(outputPath);
	}

	// Check file extension
	if (assetType == AST_NONE) {
		assetExt = GetFileExt(assetPath);
		if (assetExt.size() == 0) {
			return Error("No file extension - please specify [-t <asset type>]", 1);
		}
		assetType = GetAssetType(assetExt.c_str());
	}

	// TODO: Check file headers to figure out asset type

	int rc = 0;


	switch (assetType) {
	case AST_MODEL:
		rc = ConvertModel(assetPath, assetExt, outputPath, testMode);
		break;
	default:
		return Error("Unknown asset type");
	}

	if (rc) {
		return Error("Asset conversion fail");
	}
}