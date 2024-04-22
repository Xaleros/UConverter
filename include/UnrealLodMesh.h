#pragma once

#include "common.h"
#include <fstream>
#include <string>
#include <vector>

// References
// http://www.paulbourke.net/dataformats/unreal/
// Additional help from 3ds2unr

struct FUnreal3DHeader {
	STREAM_WRITE_OP_DECL(FUnreal3DHeader, hdr) {
		STREAM_WRITE(&hdr.numPolygons);
		STREAM_WRITE(&hdr.numVertices);
		STREAM_WRITE(&hdr.bogusRot);
		STREAM_WRITE(&hdr.bogusFrame);
		STREAM_WRITE(&hdr.bogusNormX);
		STREAM_WRITE(&hdr.bogusNormY);
		STREAM_WRITE(&hdr.bogusNormZ);
		STREAM_WRITE(&hdr.fixScale);
		STREAM_WRITE(hdr.unused);
		STREAM_WRITE(hdr.unknown);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnreal3DHeader, hdr) {
		STREAM_READ(&hdr.numPolygons);
		STREAM_READ(&hdr.numVertices);
		STREAM_READ(&hdr.bogusRot);
		STREAM_READ(&hdr.bogusFrame);
		STREAM_READ(&hdr.bogusNormX);
		STREAM_READ(&hdr.bogusNormY);
		STREAM_READ(&hdr.bogusNormZ);
		STREAM_READ(&hdr.fixScale);
		STREAM_READ(hdr.unused);
		STREAM_READ(hdr.unknown);
		STREAM_OP_END();
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
	STREAM_WRITE_OP_DECL(FUnreal3DTri, tri) {
		STREAM_WRITE(tri.vertex);
		STREAM_WRITE(&tri.type);
		STREAM_WRITE(&tri.color);
		STREAM_WRITE(tri.uv);
		STREAM_WRITE(&tri.texNum);
		STREAM_WRITE(&tri.flags);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnreal3DTri, tri) {
		STREAM_READ(tri.vertex);
		STREAM_READ(&tri.type);
		STREAM_READ(&tri.color);
		STREAM_READ(tri.uv);
		STREAM_READ(&tri.texNum);
		STREAM_READ(&tri.flags);
		STREAM_OP_END();
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