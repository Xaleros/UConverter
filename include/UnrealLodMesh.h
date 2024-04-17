#pragma once

#include "common.h"
#include <fstream>
#include <string>
#include <vector>

// References
// http://www.paulbourke.net/dataformats/unreal/
// Additional help from 3ds2unr

struct FUnreal3DHeader {
	friend std::ofstream& operator<<(std::ofstream& stream, const FUnreal3DHeader& hdr) {
		stream.write((char*)&hdr.numPolygons, sizeof(hdr.numPolygons));
		stream.write((char*)&hdr.numVertices, sizeof(hdr.numVertices));
		stream.write((char*)&hdr.bogusRot, sizeof(hdr.bogusRot));
		stream.write((char*)&hdr.bogusFrame, sizeof(hdr.bogusFrame));
		stream.write((char*)&hdr.bogusNormX, sizeof(hdr.bogusNormX));
		stream.write((char*)&hdr.bogusNormY, sizeof(hdr.bogusNormY));
		stream.write((char*)&hdr.bogusNormZ, sizeof(hdr.bogusNormZ));
		stream.write((char*)&hdr.fixScale, sizeof(hdr.fixScale));
		stream.write((char*)&hdr.unused, sizeof(hdr.unused));
		stream.write((char*)&hdr.unknown, sizeof(hdr.unknown));
		return stream;
	}

	friend std::ifstream& operator>>(std::ifstream& stream, FUnreal3DHeader& hdr) {
		stream.read((char*)&hdr.numPolygons, sizeof(hdr.numPolygons));
		stream.read((char*)&hdr.numVertices, sizeof(hdr.numVertices));
		stream.read((char*)&hdr.bogusRot, sizeof(hdr.bogusRot));
		stream.read((char*)&hdr.bogusFrame, sizeof(hdr.bogusFrame));
		stream.read((char*)&hdr.bogusNormX, sizeof(hdr.bogusNormX));
		stream.read((char*)&hdr.bogusNormY, sizeof(hdr.bogusNormY));
		stream.read((char*)&hdr.bogusNormZ, sizeof(hdr.bogusNormZ));
		stream.read((char*)&hdr.fixScale, sizeof(hdr.fixScale));
		stream.read((char*)&hdr.unused, sizeof(hdr.unused));
		stream.read((char*)&hdr.unknown, sizeof(hdr.unknown));
		return stream;
	}

	uint16_t numPolygons;
	uint16_t numVertices;
	uint16_t bogusRot = 0;
	uint16_t bogusFrame = 0;
	uint32_t bogusNormX = 0;
	uint32_t bogusNormY = 0;
	uint32_t bogusNormZ = 0;
	uint32_t fixScale = 0;
	uint32_t unused[3] = { 0 };
	uint8_t  unknown[12] = { 0 };
};

struct FUnreal3DTri {
	friend std::ofstream& operator<<(std::ofstream& stream, const FUnreal3DTri& tri) {
		stream.write((char*)&tri.vertex, sizeof(tri.vertex));
		stream.write((char*)&tri.type, sizeof(tri.type));
		stream.write((char*)&tri.color, sizeof(tri.color));
		stream.write((char*)&tri.uv, sizeof(tri.uv));
		stream.write((char*)&tri.texNum, sizeof(tri.texNum));
		stream.write((char*)&tri.flags, sizeof(tri.flags));
		return stream;
	}

	friend std::ifstream& operator>>(std::ifstream& stream, FUnreal3DTri& tri) {
		stream.read((char*)&tri.vertex, sizeof(tri.vertex));
		stream.read((char*)&tri.type, sizeof(tri.type));
		stream.read((char*)&tri.color, sizeof(tri.color));
		stream.read((char*)&tri.uv, sizeof(tri.uv));
		stream.read((char*)&tri.texNum, sizeof(tri.texNum));
		stream.read((char*)&tri.flags, sizeof(tri.flags));
		return stream;
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

	int Write(std::string& outputPath, std::string& modelName);

	FUnreal3DHeader header;

private:
	std::vector<uint32_t> vertices;
	std::vector<FUnreal3DTri> polygons;
	std::vector<FUnreal3DSeq> sequences;
	std::vector<std::string> textures;
};

class FUnrealLodMeshConverter {
public:
	virtual int Read(const std::string& path) = 0;
	virtual FUnrealLodMesh Convert() = 0;
};