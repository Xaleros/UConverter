#pragma once

#include <cstdint>
#include <iostream>
#include <fstream>

//////////////////////////////////////////////////////////////////////////

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#ifdef _WIN32
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif

#define STREAM_READ_OP_DECL(t, n) \
	friend std::ifstream& operator>>(std::ifstream& stream, t& n)

#define STREAM_WRITE_OP_DECL(t, n) \
	friend std::ofstream& operator<<(std::ofstream& stream, const t& n)

#define STREAM_READ(m) \
	stream.read(reinterpret_cast<char*>(m), sizeof(m))

#define STREAM_WRITE(m) \
	stream.write(reinterpret_cast<const char*>(m), sizeof(m))

#define STREAM_OP_END() \
	return stream

//////////////////////////////////////////////////////////////////////////

enum EAssetType {
	AST_NONE,
	AST_MODEL,
};

enum EModelType {
	MDL_NONE,
	MDL_U3D,
	MDL_MDL,
	MDL_MDL_GOLDSRC,
	MDL_MD2,
	MDL_MD3,
	MDL_PSK
};

//////////////////////////////////////////////////////////////////////////

struct FVec3f {
	STREAM_WRITE_OP_DECL(FVec3f, vec) {
		STREAM_WRITE(vec.v);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FVec3f, vec) {
		STREAM_READ(vec.v);
		STREAM_OP_END();
	}

	float& operator[](int i) {
		return v[i];
	}

	friend bool operator==(const FVec3f& a, const FVec3f& b) {
		return a.v[0] == b.v[0] && a.v[1] == b.v[1] && a.v[2] == b.v[2];
	}

	float v[3];
};

//////////////////////////////////////////////////////////////////////////

struct FQuatf {
	STREAM_WRITE_OP_DECL(FQuatf, quat) {
		STREAM_WRITE(quat.q);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FQuatf, quat) {
		STREAM_READ(quat.q);
		STREAM_OP_END();
	}

	float& operator[](int i) {
		return q[i];
	}

	friend bool operator==(const FQuatf& a, const FQuatf& b) {
		return a.q[0] == b.q[0] && a.q[1] == b.q[1] && a.q[2] == b.q[2] && a.q[3] == b.q[3];
	}

	float q[4];
};

//////////////////////////////////////////////////////////////////////////

std::string GetFileExt(const std::string& path);

//////////////////////////////////////////////////////////////////////////

int ConvertModel(const std::string& assetPath, const std::string& assetExt, const std::string& outputPath, bool testMode);
