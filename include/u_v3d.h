#pragma once

#include "common.h"
#include <string>
#include <vector>

// References
// http://www.paulbourke.net/dataformats/unreal/
// Additional help from 3ds2unr

struct FUnreal3DHeader {
	friend std::ostream& operator<<(std::ostream& stream, const FUnreal3DHeader& hdr) {
		stream << hdr.numPolygons;
		stream << hdr.numVertices;
		stream << hdr.bogusRot;
		stream << hdr.bogusFrame;
		stream << hdr.bogusNormX;
		stream << hdr.bogusNormY;
		stream << hdr.bogusNormZ;
		stream << hdr.fixScale;
		stream << hdr.unused;
		stream << hdr.unknown;
	}

	friend std::istream& operator>>(std::istream& stream, FUnreal3DHeader& hdr) {
		stream >> hdr.numPolygons;
		stream >> hdr.numVertices;
		stream >> hdr.bogusRot;
		stream >> hdr.bogusFrame;
		stream >> hdr.bogusNormX;
		stream >> hdr.bogusNormY;
		stream >> hdr.bogusNormZ;
		stream >> hdr.fixScale;

		for (int i = 0; i < ARRAY_SIZE(unused); i++)
			stream >> hdr.unused[i];

		for (int i = 0; i < ARRAY_SIZE(unknown); i++)
			stream >> hdr.unknown;
	}

	uint16_t numPolygons;
	uint16_t numVertices;
	uint16_t bogusRot;
	uint16_t bogusFrame;
	uint32_t bogusNormX;
	uint32_t bogusNormY;
	uint32_t bogusNormZ;
	uint32_t fixScale;
	uint32_t unused[3];
	uint8_t  unknown[12];
};

struct FUnreal3DTri {
	friend std::ostream& operator<<(std::ostream& stream, const FUnreal3DTri& tri) {
		stream << tri.vertex;
		stream << tri.type;
		stream << tri.color;
		stream << tri.uv;
		stream << tri.texNum;
		stream << tri.flags;
	}

	friend std::istream& operator>>(std::istream& stream, FUnreal3DTri& tri) {
		stream >> tri.vertex[0];
		stream >> tri.vertex[1];
		stream >> tri.vertex[2];
		stream >> tri.type;
		stream >> tri.color;
		stream >> tri.uv[0][0];
		stream >> tri.uv[0][1];
		stream >> tri.uv[1][0];
		stream >> tri.uv[1][1];
		stream >> tri.uv[2][0];
		stream >> tri.uv[2][1];
		stream >> tri.texNum;
		stream >> tri.flags;
	}

	uint16_t vertex[3] = { 0 };
	char	 type = 0;
	char	 color = 255;
	uint8_t  uv[3][2] = { 0 };
	char     texNum = 0;
	char	 flags = 0;
};

struct FUnreal3DSeq {
	FUnreal3DSeq();
	FUnreal3DSeq(std::string& _name, int _length);

	std::string name;
	int length;
};

class FUnrealLodMesh {
public:
	FUnrealLodMesh();
	virtual ~FUnrealLodMesh();

	void AddVertex(FVec3f& vertex);
	void AddPolygon(FUnreal3DTri& tri);
	void AddTexture(std::string& name, int n = -1);
	void AddSequence(std::string& name, int len);

	void Write(std::string& modelname);

	FUnreal3DHeader header;

private:
	std::vector<uint32_t> vertices;
	std::vector<FUnreal3DTri> polygons;
	std::vector<FUnreal3DSeq> sequences;
	std::vector<std::string> textures;
};

class FUnrealLodMeshConverter {
public:
	virtual void Read(std::string& path) = 0;
	virtual FUnrealLodMesh Convert() = 0;
};