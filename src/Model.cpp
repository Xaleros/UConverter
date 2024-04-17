#include "common.h"
#include "idTechMd2.h"
#include "UnrealLodMesh.h"

struct FModelType {
	char name[8];
	EModelType type;
	bool bVertexFormat;
};

FModelType modelTable[] = {
	{"mdl", MDL_MDL, true},
	{"md2", MDL_MD2, true},
	{"md3", MDL_MD3, true},
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

int ConvertModel(const std::string& assetPath, const std::string& assetExt, const std::string& outputPath) {

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
		// TODO: skeletal meshes
		std::cout << "Skeletal meshes not yet supported" << std::endl;
		return -1;
	}
}