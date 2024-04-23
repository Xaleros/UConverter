#include "common.h"
#include "idTechMd2.h"
#include "UnrealLodMesh.h"
#include "UnrealSkeletalMesh.h"

struct FModelType {
	char name[8];
	EModelType type;
	bool bVertexFormat;
};

FModelType modelTable[] = {
	{".3d", MDL_U3D, true},
	{"mdl", MDL_MDL, true},
	{"md2", MDL_MD2, true},
	{"md3", MDL_MD3, true},
	{"psk", MDL_PSK, false},
};

static FModelType* GetModelType(const char* type) {
	for (int i = 0; i < ARRAY_SIZE(modelTable); i++) {
		FModelType* t = &modelTable[i];
		if (strnicmp(type, t->name, strlen(t->name)) == 0) {
			return t;
		}
	}

	return nullptr;
}

int ConvertModel(const std::string& assetPath, const std::string& assetExt, const std::string& outputPath, bool testMode) {
	int slash = outputPath.find_last_of(DIRECTORY_SEPARATOR);
	std::string& outputName = outputPath.substr(slash+1);
	std::string& outputDir = outputPath.substr(0, slash);

	FModelType* type = GetModelType(assetExt.c_str());
	if (!type) {
		std::cout << "Unknown model format " << assetExt << std::endl;
		return -1;
	}

	if (type->bVertexFormat) {
		FUnrealLodMeshConverter* converter = nullptr;

		switch (type->type) {
		case MDL_MD2:
			converter = new FMD2Model();
			break;
		default:
			std::cout << "Unregistered model format " << assetExt << std::endl;
			return -1;
		}

		converter->Read(assetPath);
		FUnrealLodMesh mesh = converter->Convert();
		return mesh.Write(outputDir, outputName);
	}
	else {
		if (testMode) {
			if (type->type != MDL_PSK) {
				std::cout << "Test mode not available for format " << assetExt << std::endl;
				return -1;
			}

			FUnrealSkeletalMesh mesh;
			if (mesh.Test(assetPath) < 0) {
				std::cout << "Skeletal Test failed" << std::endl;
				return -1;
			}
		}
		return 0;
	}
}