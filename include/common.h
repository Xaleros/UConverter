#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#ifdef _WIN32
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif

enum EAssetType {
	AST_NONE,
	AST_MODEL,
};

enum EModelType {
	MDL_NONE,
	MDL_MDL,
	MDL_MDL_GOLDSRC,
	MDL_MD2,
	MDL_MD3,
};

class FVec3f {
public:
	friend std::ofstream& operator<<(std::ofstream& stream, FVec3f& vec) {
		stream.write(reinterpret_cast<const char*>(&vec), sizeof(vec));
		return stream;
	}

	friend std::ifstream& operator>>(std::ifstream& stream, FVec3f& vec) {
		stream.read(reinterpret_cast<char*>(&vec), sizeof(vec));
		return stream;
	}

	float& operator[](int i) {
		return v[i];
	}

	float v[3];
};

int ConvertModel(const std::string& assetPath, const std::string& assetExt, const std::string& outputPath);