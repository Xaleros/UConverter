#include <iostream>
#include "UnrealLodMesh.h"
#include "idTechMd2.h"

struct FAssetType {
	char name[8];
	EAssetType type;
	char* desc;
};

FAssetType typeTable[] = {
	{"mdl", AST_MODEL, "idTech 1 (Quake) Model"},
	{"md2", AST_MODEL, "idTech 2 (Quake 2) Model"},
	{"md3", AST_MODEL, "idTech3 Model"},
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
}

static int Error(const char* msg) {
	std::cout << msg << std::endl;
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
	std::string outputPath;
	std::string assetPath;
	std::string assetExt;
	EAssetType assetType = AST_NONE;

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		if (arg[0] == '-') {
			i++;
			switch (arg[1]) {
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
		return Error("Required argument [-i <input path>] missing");
	}

	if (outputPath.size() == 0) {
		return Error("Required argument [-o <output path>] missing");
	}

	// Check file extension
	if (assetType == AST_NONE) {
		size_t extDot = assetPath.find_last_of('.');
		if (extDot == std::string::npos) {
			return Error("Unknown file extension - please specify [-t <asset type>]");
		}

		assetExt = assetPath.substr(extDot+1);
		assetType = GetAssetType(assetExt.c_str());
	}

	// TODO: Check file headers to figure out asset type

	int rc = 0;

	NormalizeDirectoryString(assetPath);
	NormalizeDirectoryString(outputPath);

	switch (assetType) {
	case AST_MODEL:
		rc = ConvertModel(assetPath, assetExt, outputPath);
		break;
	default:
		return Error("Unknown asset type");
	}

	if (rc) {
		return Error("Asset conversion fail");
	}
}