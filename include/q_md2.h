#pragma once

#include <iostream>
#include "common.h"
#include "u_v3d.h"

// References
// http://tfc.duke.free.fr/coding/md2-specs-en.html

#define MD2_IDENT 844121161

struct FMD2Header {
	friend std::istream& operator>>(std::istream& stream, FMD2Header& hdr) {
		stream >> hdr.ident;
		stream >> hdr.version;
		stream >> hdr.skinWidth;
		stream >> hdr.skinHeight;
		stream >> hdr.frameSize;
		stream >> hdr.numSkins;
		stream >> hdr.numVertices;
		stream >> hdr.numSt;
		stream >> hdr.numTris;
		stream >> hdr.numGlCmds;
		stream >> hdr.numFrames;
		stream >> hdr.offsetSkins;
		stream >> hdr.offsetSt;
		stream >> hdr.offsetTris;
		stream >> hdr.offsetFrames;
		stream >> hdr.offsetGlCmds;
		stream >> hdr.offsetEnd;
	}

	int ident = 0;
	int version = 0;

	int skinWidth = 0;
	int skinHeight = 0;

	int frameSize = 0;

	int numSkins = 0;
	int numVertices = 0;
	int numSt = 0;
	int numTris = 0;
	int numGlCmds = 0;
	int numFrames = 0;

	int offsetSkins = 0;
	int offsetSt = 0;
	int offsetTris = 0;
	int offsetFrames = 0;
	int offsetGlCmds = 0;
	int offsetEnd = 0;
};

struct FMD2Skin {
	friend std::istream& operator>>(std::istream& stream, FMD2Skin& skin) {
		stream >> skin.name;
	}

	char name[64] = { 0 };
};

struct FMD2TexCoord {
	friend std::istream& operator>>(std::istream& stream, FMD2TexCoord& texcoord) {
		stream >> texcoord.s;
		stream >> texcoord.t;
	}

	uint16_t s = 0;
	uint16_t t = 0;
};

struct FMD2Triangle {
	friend std::istream& operator>>(std::istream& stream, FMD2Triangle& tri) {
		stream >> tri.v[0];
		stream >> tri.v[1];
		stream >> tri.v[2];
		stream >> tri.st[0];
		stream >> tri.st[1];
		stream >> tri.st[2];
	}

	uint16_t v[3] = { 0 };
	uint16_t st[3] = { 0 };
};

struct FMD2Vertex {
	friend std::istream& operator>>(std::istream& stream, FMD2Vertex& vertex) {
		stream >> vertex.v[0];
		stream >> vertex.v[1];
		stream >> vertex.v[2];
		stream >> vertex.normalIndex;
	}

	uint8_t v[3] = { 0 };
	uint8_t normalIndex = 0;
};

struct FMD2Frame {
	friend std::istream& operator>>(std::istream& stream, FMD2Frame& frame) {
		stream >> frame.scale;
		stream >> frame.translate;
		stream >> frame.name;
		for (FMD2Vertex& v : frame.verts) {
			stream >> v;
		}
	}

	FVec3f scale;
	FVec3f translate;
	char name[16];
	std::vector<FMD2Vertex> verts;
};

class FMD2Model : public FUnrealLodMeshConverter {
public:
	FMD2Model() {}
	virtual ~FMD2Model() {}

	virtual void Read(std::string& path);
	virtual FUnrealLodMesh Convert();

private:
	std::string GetMD2SequenceGroup(FMD2Frame& frame);

	FMD2Header header;
	std::vector<FMD2Skin> skins;
	std::vector<FMD2TexCoord> texcoords;
	std::vector<FMD2Triangle> triangles;
	std::vector<FMD2Frame> frames;
	std::vector<int> glcmds;
};
