#pragma once

#include <fstream>
#include "Common.h"
#include "UnrealLodMesh.h"

#define Q_MDL_IDENT 1330660425

struct FQuakeMdlHeader {
	friend std::ifstream& operator>>(std::ifstream& stream, FQuakeMdlHeader& hdr) {
		stream.read((char*)&hdr, sizeof(hdr));
		return stream;
	}

	int ident = 0;
	int version = 0;

	FVec3f scale;
	FVec3f translate;
	float boundingRadius;
	FVec3f eyePosition;

	int numSkins;
	int skinWidth;
	int skinHeight;
	
	int numVerts;
	int numTris;
	int numFrames;

	int syncType;
	int flags;
	float size;
};

struct FQuakeMdlSkin {
	int group;
	std::vector<uint8_t> data;
};

struct FQuakeMdlGroupSkin {
	int group;
	int nb;
	std::vector<float> time;
	std::vector<uint8_t> data;
};

struct FQuakeMdlTexCoord {
	int onSeam;
	int s;
	int t;
};

struct FQuakeMdlTriangle {
	friend std::ifstream& operator>>(std::ifstream& stream, FQuakeMdlTriangle& tri) {
		stream.read((char*)&tri, sizeof(tri));
		return stream;
	}

	int facesFront;
	int vertex[3];
};

struct FQuakeMdlVertex {
	friend std::ifstream& operator>>(std::ifstream& stream, FQuakeMdlVertex& vertex) {
		stream.read((char*)&vertex, sizeof(vertex));
		return stream;
	}

	uint8_t v[3];
	uint8_t normalIndex;
};

struct FQuakeMdlSimpleFrame {
	FQuakeMdlVertex bboxMin;
	FQuakeMdlVertex bboxMax;
	char name[16];
	std::vector<FQuakeMdlVertex> verts;
};

struct FQuakeMdlFrame {
	int type;
	FQuakeMdlSimpleFrame frame;
};

struct FQuakeMdlGroupFrame {
	int type;
	FQuakeMdlVertex min;
	FQuakeMdlVertex max;
	std::vector<float> time;
	std::vector<FQuakeMdlSimpleFrame> frames;
};
