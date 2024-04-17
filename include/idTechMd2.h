#pragma once

#include <fstream>
#include "Common.h"
#include "UnrealLodMesh.h"

// References
// http://tfc.duke.free.fr/coding/md2-specs-en.html

#define MD2_IDENT 844121161

struct FMD2Header {
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2Header& hdr) {
		stream.read((char*)&hdr.ident, sizeof(hdr.ident));
		stream.read((char*)&hdr.version, sizeof(hdr.version));
		stream.read((char*)&hdr.skinWidth, sizeof(hdr.skinWidth));
		stream.read((char*)&hdr.skinHeight, sizeof(hdr.skinHeight));
		stream.read((char*)&hdr.frameSize, sizeof(hdr.frameSize));
		stream.read((char*)&hdr.numSkins, sizeof(hdr.numSkins));
		stream.read((char*)&hdr.numVertices, sizeof(hdr.numVertices));
		stream.read((char*)&hdr.numSt, sizeof(hdr.numSt));
		stream.read((char*)&hdr.numTris, sizeof(hdr.numTris));
		stream.read((char*)&hdr.numGlCmds, sizeof(hdr.numGlCmds));
		stream.read((char*)&hdr.numFrames, sizeof(hdr.numFrames));
		stream.read((char*)&hdr.offsetSkins, sizeof(hdr.offsetSkins));
		stream.read((char*)&hdr.offsetSt, sizeof(hdr.offsetSt));
		stream.read((char*)&hdr.offsetTris, sizeof(hdr.offsetTris));
		stream.read((char*)&hdr.offsetFrames, sizeof(hdr.offsetFrames));
		stream.read((char*)&hdr.offsetGlCmds, sizeof(hdr.offsetGlCmds));
		stream.read((char*)&hdr.offsetEnd, sizeof(hdr.offsetEnd));
		return stream;
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
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2Skin& skin) {
		stream.read(skin.name, sizeof(skin.name));
		return stream;
	}

	char name[64] = { 0 };
};

struct FMD2TexCoord {
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2TexCoord& texcoord) {
		stream.read((char*)&texcoord.s, sizeof(texcoord.s));
		stream.read((char*)&texcoord.t, sizeof(texcoord.t));
		return stream;
	}

	uint16_t s = 0;
	uint16_t t = 0;
};

struct FMD2Triangle {
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2Triangle& tri) {
		stream.read((char*)&tri.v, sizeof(tri.v));
		stream.read((char*)&tri.st, sizeof(tri.st));
		return stream;
	}

	uint16_t v[3] = { 0 };
	uint16_t st[3] = { 0 };
};

struct FMD2Vertex {
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2Vertex& vertex) {
		stream.read((char*)&vertex.v, sizeof(vertex.v));
		stream.read((char*)&vertex.normalIndex, sizeof(vertex.v));
		return stream;
	}

	uint8_t v[3] = { 0 };
	uint8_t normalIndex = 0;
};

struct FMD2Frame {
	friend std::ifstream& operator>>(std::ifstream& stream, FMD2Frame& frame) {
		int pos = stream.tellg();
		stream.read((char*)&frame.scale, sizeof(frame.scale));
		stream.read((char*)&frame.translate, sizeof(frame.translate));
		stream.read((char*)&frame.name, sizeof(frame.name));
		for (FMD2Vertex& v : frame.verts) {
			stream.read((char*)&v, sizeof(v));
		}
		return stream;
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

	virtual int Read(const std::string& path);
	virtual FUnrealLodMesh Convert();

private:
	FMD2Header header;
	std::vector<FMD2Skin> skins;
	std::vector<FMD2TexCoord> texcoords;
	std::vector<FMD2Triangle> triangles;
	std::vector<FMD2Frame> frames;
	std::vector<int> glcmds;
};
