#pragma once

#include <fstream>
#include "Common.h"
#include "UnrealLodMesh.h"

// References
// http://tfc.duke.free.fr/coding/md2-specs-en.html

#define MD2_IDENT 844121161

struct FMD2Header {
	STREAM_READ_OP_DECL(FMD2Header, hdr) {
		STREAM_READ(hdr.ident);
		STREAM_READ(hdr.version);
		STREAM_READ(hdr.skinWidth);
		STREAM_READ(hdr.skinHeight);
		STREAM_READ(hdr.frameSize);
		STREAM_READ(hdr.numSkins);
		STREAM_READ(hdr.numVertices);
		STREAM_READ(hdr.numSt);
		STREAM_READ(hdr.numTris);
		STREAM_READ(hdr.numGlCmds);
		STREAM_READ(hdr.numFrames);
		STREAM_READ(hdr.offsetSkins);
		STREAM_READ(hdr.offsetSt);
		STREAM_READ(hdr.offsetTris);
		STREAM_READ(hdr.offsetFrames);
		STREAM_READ(hdr.offsetGlCmds);
		STREAM_READ(hdr.offsetEnd);
		STREAM_OP_END();
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
	STREAM_READ_OP_DECL(FMD2Skin, skin) {
		STREAM_READ_BUF(skin.name);
		STREAM_OP_END();
	}

	char name[64] = { 0 };
};

struct FMD2TexCoord {
	STREAM_READ_OP_DECL(FMD2TexCoord, texcoord) {
		STREAM_READ(texcoord.s);
		STREAM_READ(texcoord.t);
		STREAM_OP_END();
	}

	uint16_t s = 0;
	uint16_t t = 0;
};

struct FMD2Triangle {
	STREAM_READ_OP_DECL(FMD2Triangle, tri) {
		STREAM_READ(tri.v);
		STREAM_READ(tri.st);
		STREAM_OP_END();
	}

	uint16_t v[3] = { 0 };
	uint16_t st[3] = { 0 };
};

struct FMD2Vertex {
	STREAM_READ_OP_DECL(FMD2Vertex, vertex) {
		STREAM_READ_BUF(vertex.v);
		STREAM_READ(vertex.normalIndex);
		STREAM_OP_END();
	}

	uint8_t v[3] = { 0 };
	uint8_t normalIndex = 0;
};

struct FMD2Frame {
	STREAM_READ_OP_DECL(FMD2Frame, frame) {
		stream >> frame.scale;
		stream >> frame.translate;
		STREAM_READ_BUF(frame.name);
		for (FMD2Vertex& v : frame.verts) {
			stream >> v;
		}
		STREAM_OP_END();
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
