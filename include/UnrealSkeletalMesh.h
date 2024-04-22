#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "Common.h"

// References
// https://wiki.beyondunreal.com/PSK_%26_PSA_file_formats																					\

#define UNREAL_SKEL_CHUNK_FLAGS 1999801

struct FUnrealSkelChunk {
	STREAM_WRITE_OP_DECL(FUnrealSkelChunk, chunk) {
		STREAM_WRITE(chunk.id);
		STREAM_WRITE(&chunk.flags);
		STREAM_WRITE(&chunk.dataSize);
		STREAM_WRITE(&chunk.dataCount);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealSkelChunk, chunk) {
		STREAM_READ(chunk.id);
		STREAM_READ(&chunk.flags);
		STREAM_READ(&chunk.dataSize);
		STREAM_READ(&chunk.dataCount);
		STREAM_OP_END();
	}

	bool IsValid(const char* expectedId, int expectedSize) {
		size_t len = strlen(expectedId);
		if (len > sizeof(id))
			return false;

		return (strncmp(id, expectedId, len) == 0 
			&& flags == UNREAL_SKEL_CHUNK_FLAGS 
			&& dataSize == expectedSize);
	}

	char id[20] = { 0 };
	int flags = 0;
	int dataSize = 0;
	int dataCount = 0;
};

// PSK data types
struct FUnrealVertex {
	STREAM_WRITE_OP_DECL(FUnrealVertex, vert) {
		STREAM_WRITE(&vert.pointIndex);
		STREAM_WRITE(&vert.reserved0);
		STREAM_WRITE(&vert.u);
		STREAM_WRITE(&vert.v);
		STREAM_WRITE(&vert.matIndex);
		STREAM_WRITE(vert.reserved1);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealVertex, vert) {
		STREAM_READ(&vert.pointIndex);
		STREAM_READ(&vert.reserved0);
		STREAM_READ(&vert.u);
		STREAM_READ(&vert.v);
		STREAM_READ(&vert.matIndex);
		STREAM_READ(vert.reserved1);
		STREAM_OP_END();
	}

	friend bool operator==(const FUnrealVertex& a, const FUnrealVertex& b) {
		return (a.pointIndex == b.pointIndex) &&
			(a.u == b.u) &&
			(a.v == b.v) &&
			(a.matIndex == b.matIndex);
	}

	uint16_t pointIndex = 0;
	uint16_t reserved0 = 0;
	float u = 0.0f;
	float v = 0.0f;
	uint8_t matIndex = 0;
	uint8_t reserved1[3] = { 0 };
};

struct FUnrealTriangle {
	STREAM_WRITE_OP_DECL(FUnrealTriangle, tri) {
		STREAM_WRITE(tri.wedgeIndex);
		STREAM_WRITE(&tri.matIndex);
		STREAM_WRITE(&tri.auxMatIndex);
		STREAM_WRITE(&tri.smoothingGroups);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealTriangle, tri) {
		STREAM_READ(tri.wedgeIndex);
		STREAM_READ(&tri.matIndex);
		STREAM_READ(&tri.auxMatIndex);
		STREAM_READ(&tri.smoothingGroups);
		STREAM_OP_END();
	}

	uint16_t wedgeIndex[3];
	uint8_t matIndex;
	uint8_t auxMatIndex;
	int smoothingGroups;
};

struct FUnrealMaterial {
	STREAM_WRITE_OP_DECL(FUnrealMaterial, mat) {
		STREAM_WRITE(mat.name);
		STREAM_WRITE(&mat.textureIndex);
		STREAM_WRITE(&mat.polyFlags);
		STREAM_WRITE(&mat.auxMaterial);
		STREAM_WRITE(&mat.auxFlags);
		STREAM_WRITE(&mat.lodBias);
		STREAM_WRITE(&mat.lodStyle);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealMaterial, mat) {
		STREAM_READ(mat.name);
		STREAM_READ(&mat.textureIndex);
		STREAM_READ(&mat.polyFlags);
		STREAM_READ(&mat.auxMaterial);
		STREAM_READ(&mat.auxFlags);
		STREAM_READ(&mat.lodBias);
		STREAM_READ(&mat.lodStyle);
		STREAM_OP_END();
	}

	friend bool operator==(const FUnrealMaterial& a, const FUnrealMaterial& b) {
		return (a.textureIndex == b.textureIndex) &&
			(a.polyFlags == b.polyFlags) &&
			(a.auxMaterial == b.auxMaterial) &&
			(a.auxFlags == b.auxFlags) &&
			(a.lodBias == b.lodBias) &&
			(a.lodStyle == b.lodStyle) &&
			(strncmp(a.name, b.name, sizeof(a.name)) == 0);
	}

	char name[64];
	int textureIndex;
	int polyFlags;
	int auxMaterial;
	int auxFlags;
	int lodBias;
	int lodStyle;
};

struct FUnrealJointPos {
	STREAM_WRITE_OP_DECL(FUnrealJointPos, j) {
		stream << j.orientation;
		stream << j.position;
		STREAM_WRITE(&j.length);
		stream << j.size;
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealJointPos, j) {
		stream >> j.orientation;
		stream >> j.position;
		STREAM_READ(&j.length);
		stream >> j.size;
		STREAM_OP_END();
	}

	friend bool operator==(const FUnrealJointPos& a, const FUnrealJointPos& b) {
		return (a.orientation == b.orientation) &&
			(a.position == b.position) &&
			(a.length == b.length) &&
			(a.size == b.size);
	}

	FQuatf orientation;
	FVec3f position;
	float length;
	FVec3f size;
};

struct FUnrealBone {
	STREAM_WRITE_OP_DECL(FUnrealBone, bone) {
		STREAM_WRITE(bone.name);
		STREAM_WRITE(&bone.boneFlags);
		STREAM_WRITE(&bone.numChildren);
		STREAM_WRITE(&bone.parentIndex);
		stream << bone.bonePos;
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealBone, bone) {
		STREAM_READ(bone.name);
		STREAM_READ(&bone.boneFlags);
		STREAM_READ(&bone.numChildren);
		STREAM_READ(&bone.parentIndex);
		stream >> bone.bonePos;
		STREAM_OP_END();
	}

	friend bool operator==(const FUnrealBone& a, const FUnrealBone& b) {
		return (a.boneFlags == b.boneFlags) &&
			(a.numChildren == b.numChildren) &&
			(a.parentIndex == b.parentIndex) &&
			(strncmp(a.name, b.name, sizeof(a.name)) == 0) &&
			(a.bonePos == b.bonePos);
	}

	friend bool operator!=(const FUnrealBone& a, const FUnrealBone& b) {
		return !(a == b);
	}

	char name[64];
	int boneFlags;
	int numChildren;
	int parentIndex;
	FUnrealJointPos bonePos;
};

struct FUnrealBoneWeight {
	STREAM_WRITE_OP_DECL(FUnrealBoneWeight, inf) {
		STREAM_WRITE(&inf.weight);
		STREAM_WRITE(&inf.pointIndex);
		STREAM_WRITE(&inf.boneIndex);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealBoneWeight, inf) {
		STREAM_READ(&inf.weight);
		STREAM_READ(&inf.pointIndex);
		STREAM_READ(&inf.boneIndex);
		STREAM_OP_END();
	}

	float weight;
	int pointIndex;
	int boneIndex;
};

// PSA data types
struct FUnrealAnimInfo {
	STREAM_WRITE_OP_DECL(FUnrealAnimInfo, anim) {
		STREAM_WRITE(anim.name);
		STREAM_WRITE(anim.group);
		STREAM_WRITE(&anim.totalBones);
		STREAM_WRITE(&anim.rootInclude);
		STREAM_WRITE(&anim.keyCompressionStyle);
		STREAM_WRITE(&anim.keyQuotum);
		STREAM_WRITE(&anim.keyReduction);
		STREAM_WRITE(&anim.trackTime);
		STREAM_WRITE(&anim.animRate);
		STREAM_WRITE(&anim.startBone);
		STREAM_WRITE(&anim.firstRawFrame);
		STREAM_WRITE(&anim.numRawFrames);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealAnimInfo, anim) {
		STREAM_READ(anim.name);
		STREAM_READ(anim.group);
		STREAM_READ(&anim.totalBones);
		STREAM_READ(&anim.rootInclude);
		STREAM_READ(&anim.keyCompressionStyle);
		STREAM_READ(&anim.keyQuotum);
		STREAM_READ(&anim.keyReduction);
		STREAM_READ(&anim.trackTime);
		STREAM_READ(&anim.animRate);
		STREAM_READ(&anim.startBone);
		STREAM_READ(&anim.firstRawFrame);
		STREAM_READ(&anim.numRawFrames);
		STREAM_OP_END();
	}

	char name[64];
	char group[64];
	int totalBones;
	int rootInclude;
	int keyCompressionStyle;
	int keyQuotum;
	float keyReduction;
	float trackTime;
	float animRate;
	int startBone;
	int firstRawFrame;
	int numRawFrames;
};

struct FUnrealQuatAnimKey {
	STREAM_WRITE_OP_DECL(FUnrealQuatAnimKey, key) {
		stream << key.position;
		stream << key.orientation;
		STREAM_WRITE(&key.time);
		STREAM_OP_END();
	}

	STREAM_READ_OP_DECL(FUnrealQuatAnimKey, key) {
		stream >> key.position;
		stream >> key.orientation;
		STREAM_READ(&key.time);
		STREAM_OP_END();
	}

	FVec3f position;
	FQuatf orientation;
	float time;
};

class FUnrealSkeletalMesh {
public:
	FUnrealSkeletalMesh();
	virtual ~FUnrealSkeletalMesh();

	int Read(const std::string& path);
	int Write(const std::string& outputPath, const std::string& modelName);
	int Test(const std::string& path);

	int AddPoint(const FVec3f& p);
	int AddVertex(const FUnrealVertex& v);
	int AddTriangle(const FUnrealTriangle& t);
	int AddMaterial(const FUnrealMaterial& m);
	int AddBone(const FUnrealBone& b);
	int AddWeight(const FUnrealBoneWeight& w);
	int AddAnimation(const FUnrealAnimInfo& a);
	int AddAnimKey(const FUnrealQuatAnimKey& k);

private:
	void GetPskPsaPaths(const std::string& path, std::string& pskPath, std::string& psaPath);

	FUnrealSkelChunk pskHeader;
	FUnrealSkelChunk psaHeader;
	std::vector<FVec3f> points;
	std::vector<FUnrealVertex> vertices;
	std::vector<FUnrealTriangle> faces;
	std::vector<FUnrealMaterial> materials;
	std::vector<FUnrealBone> bones;
	std::vector<FUnrealBoneWeight> weights;
	std::vector<FUnrealAnimInfo> animations;
	std::vector<FUnrealQuatAnimKey> animKeys;
};

class FUnrealSkeletalMeshConverter {
public:
	virtual int Read(const std::string& path) = 0;
	virtual int ReadMesh(const std::string& path) { 
		return -1; 
	}
	virtual int ReadAnim(const std::string& path) {
		return -1;
	}
	virtual FUnrealSkeletalMesh Convert() = 0;
};