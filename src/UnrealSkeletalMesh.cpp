#pragma once

#include "UnrealSkeletalMesh.h"

FUnrealSkeletalMesh::FUnrealSkeletalMesh() {}
FUnrealSkeletalMesh::~FUnrealSkeletalMesh() {}

static const char pskActrHead[] = "ACTRHEAD";
static const char pskPnts0000[] = "PNTS0000";
static const char pskVtxw0000[] = "VTXW0000";
static const char pskFace0000[] = "FACE0000";
static const char pskMatt0000[] = "MATT0000";
static const char pskRefskelt[] = "REFSKELT";
static const char pskRawweights[] = "RAWWEIGHTS";

static const char psaAnimHead[] = "ANIMHEAD";
static const char psaBoneNames[] = "BONENAMES";
static const char psaAnimInfo[] = "ANIMINFO";
static const char psaAnimKeys[] = "ANIMKEYS";

int FUnrealSkeletalMesh::Read(const std::string& path) {
	std::string pskPath;
	std::string psaPath;
	GetPskPsaPaths(path, pskPath, psaPath);

	if (pskPath.empty()) {
		std::cout << "Bad psk/psa path: " + path << std::endl;
		return -1;
	}

	// Parse psk file
	std::ifstream stream(pskPath, std::ifstream::in | std::ifstream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << pskPath << std::endl;
		return -1;
	}

	stream >> pskHeader;
	
	if (!pskHeader.IsValid(pskActrHead, 0)) {
		std::cout << "ACTRHEAD chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}

	FUnrealSkelChunk chunk;

	// PSK points
	stream >> chunk;
	if (!chunk.IsValid(pskPnts0000, sizeof(FVec3f))) {
		std::cout << "PNTS0000 chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	points.resize(chunk.dataCount);
	for (FVec3f& p : points) {
		stream >> p;
	}

	// PSK vertices
	stream >> chunk;
	if (!chunk.IsValid(pskVtxw0000, sizeof(FUnrealVertex))) {
		std::cout << "VTXW0000 chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	vertices.resize(chunk.dataCount);
	for (FUnrealVertex& w : vertices) {
		stream >> w;
	}

	// PSK faces
	stream >> chunk;
	if (!chunk.IsValid(pskFace0000, sizeof(FUnrealTriangle))) {
		std::cout << "FACE0000 chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	faces.resize(chunk.dataCount);
	for (FUnrealTriangle& f : faces) {
		stream >> f;
	}

	// PSK materials
	stream >> chunk;
	if (!chunk.IsValid(pskMatt0000, sizeof(FUnrealMaterial))) {
		std::cout << "MATT0000 chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	materials.resize(chunk.dataCount);
	for (FUnrealMaterial& m : materials) {
		stream >> m;
	}

	// PSK bones
	stream >> chunk;
	if (!chunk.IsValid(pskRefskelt, sizeof(FUnrealBone))) {
		std::cout << "REFSKELT chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	bones.resize(chunk.dataCount);
	for (FUnrealBone& b : bones) {
		stream >> b;
	}

	// PSK weights
	stream >> chunk;
	if (!chunk.IsValid(pskRawweights, sizeof(FUnrealBoneWeight))) {
		std::cout << "RAWWEIGHTS chunk invalid or missing in " << pskPath << std::endl;
		return -1;
	}
	weights.resize(chunk.dataCount);
	for (FUnrealBoneWeight& w : weights) {
		stream >> w;
	}

	stream.close();

	// Parse psa file
	stream.open(psaPath, std::ifstream::in | std::ifstream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << psaPath << std::endl;
		return -1;
	}

	stream >> psaHeader;

	if (!psaHeader.IsValid(psaAnimHead, 0)) {
		std::cout << "ANIMHEAD chunk invalid or missing in " << psaPath << std::endl;
		return -1;
	}

	// PSA bones
	stream >> chunk;
	if (!chunk.IsValid(psaBoneNames, sizeof(FUnrealBone))) {
		std::cout << "BONENAMES chunk invalid or missing in " << psaPath << std::endl;
		return -1;
	}

	if (bones.size() != chunk.dataCount) {
		std::cout << "Bone count mismatch in PSA " << psaPath << std::endl;
		return -1;
	}

	for (FUnrealBone& b : bones) {
		FUnrealBone psaBone;
		stream >> psaBone;
		if (b != psaBone) {
			std::cout << "Bone " << b.name << " mismatch in " << psaPath << std::endl;
			return -1;
		}
	}

	// PSA animations
	stream >> chunk;
	if (!chunk.IsValid(psaAnimInfo, sizeof(FUnrealAnimInfo))) {
		std::cout << "ANIMINFO chunk invalid or missing in " << psaPath << std::endl;
		return -1;
	}
	animations.resize(chunk.dataCount);
	for (FUnrealAnimInfo& a : animations) {
		stream >> a;
	}

	// PSA anim keys
	stream >> chunk;
	if (!chunk.IsValid(psaAnimKeys, sizeof(FUnrealAnimInfo))) {
		std::cout << "ANIMKEYS chunk invalid or missing in " << psaPath << std::endl;
		return -1;
	}
	animKeys.resize(chunk.dataCount);
	for (FUnrealQuatAnimKey& k : animKeys) {
		stream >> k;
	}

	stream.close();
	return 0;
}

int FUnrealSkeletalMesh::Write(const std::string& outputPath, const std::string& modelName) {
	std::string pskFile = "Models\\" + modelName + ".psk";
	std::string psaFile = "Models\\" + modelName + ".psa";

	std::string pskPath = outputPath + "\\" + pskFile;
	std::string psaPath = outputPath + "\\" + psaFile;
	std::string ucPath = outputPath + "\\Classes\\" + modelName + ".uc";

	std::ofstream stream;

	// Write PSK file
	stream.open(pskPath, std::ofstream::out | std::istream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << pskPath << std::endl;
		return -1;
	}

	strncpy(pskHeader.id, pskActrHead, sizeof(pskActrHead));
	pskHeader.flags = UNREAL_SKEL_CHUNK_FLAGS;
	stream << pskHeader;

	FUnrealSkelChunk chunk;
	chunk.flags = UNREAL_SKEL_CHUNK_FLAGS;

	strncpy(chunk.id, pskPnts0000, sizeof(pskPnts0000));
	chunk.dataSize = sizeof(FVec3f);
	chunk.dataCount = points.size();

	stream << chunk;
	for (FVec3f& p : points) {
		stream << p;
	}

	strncpy(chunk.id, pskVtxw0000, sizeof(pskVtxw0000));
	chunk.dataSize = sizeof(FUnrealVertex);
	chunk.dataCount = vertices.size();

	stream << chunk;
	for (FUnrealVertex& v : vertices) {
		stream << v;
	}

	strncpy(chunk.id, pskFace0000, sizeof(pskFace0000));
	chunk.dataSize = sizeof(FUnrealTriangle);
	chunk.dataCount = faces.size();

	stream << chunk;
	for (FUnrealTriangle& f : faces) {
		stream << f;
	}

	strncpy(chunk.id, pskMatt0000, sizeof(pskMatt0000));
	chunk.dataSize = sizeof(FUnrealMaterial);
	chunk.dataCount = materials.size();

	stream << chunk;
	for (FUnrealMaterial& m : materials) {
		stream << m;
	}

	strncpy(chunk.id, pskRefskelt, sizeof(pskRefskelt));
	chunk.dataSize = sizeof(FUnrealBone);
	chunk.dataCount = bones.size();

	stream << chunk;
	for (FUnrealBone& b : bones) {
		stream << b;
	}

	strncpy(chunk.id, pskRawweights, sizeof(pskRawweights));
	chunk.dataSize = sizeof(FUnrealBoneWeight);
	chunk.dataCount = weights.size();

	stream << chunk;
	for (FUnrealBoneWeight& w : weights) {
		stream << w;
	}

	stream.close();

	// Write PSA file
	stream.open(psaPath, std::ofstream::out | std::istream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << psaPath << std::endl;
		return -1;
	}

	strncpy(psaHeader.id, psaAnimHead, sizeof(psaAnimHead));
	psaHeader.flags = UNREAL_SKEL_CHUNK_FLAGS;
	stream << psaHeader;

	strncpy(chunk.id, psaBoneNames, sizeof(psaBoneNames));
	chunk.dataSize = sizeof(FUnrealBone);
	chunk.dataCount = bones.size();

	stream << chunk;
	for (FUnrealBone& b : bones) {
		stream << b;
	}

	strncpy(chunk.id, psaAnimInfo, sizeof(psaAnimInfo));
	chunk.dataSize = sizeof(FUnrealAnimInfo);
	chunk.dataCount = animations.size();

	stream << chunk;
	for (FUnrealAnimInfo& a : animations) {
		stream << a;
	}

	strncpy(chunk.id, psaAnimKeys, sizeof(psaAnimKeys));
	chunk.dataSize = sizeof(FUnrealQuatAnimKey);
	chunk.dataCount = animKeys.size();

	stream << chunk;
	for (FUnrealQuatAnimKey& k : animKeys) {
		stream << k;
	}

	stream.close();

	// TODO: write UC file
	// TODO: FUnrealScriptFile, use in FUnrealLodMesh
	return 0;
}

int FUnrealSkeletalMesh::Test(const std::string& path) {
	return -1;
}

int FUnrealSkeletalMesh::AddPoint(const FVec3f& p) {
	int i = 0;
	for (; i < points.size(); i++) {
		if (p == points[i]) {
			return i;
		}
	}
	points.push_back(p);
	return i;
}

int FUnrealSkeletalMesh::AddVertex(const FUnrealVertex& v) {
	int i = 0;
	for (; i < vertices.size(); i++) {
		if (v == vertices[i]) {
			return i;
		}
	}
	vertices.push_back(v);
	return i;
}

int FUnrealSkeletalMesh::AddTriangle(const FUnrealTriangle& t) {
	int i = faces.size();
	faces.push_back(t);
	return i;
}

int FUnrealSkeletalMesh::AddMaterial(const FUnrealMaterial& m) {
	int i = 0;
	for (; i < materials.size(); i++) {
		if (m == materials[i]) {
			return i;
		}
	}
	materials.push_back(m);
	return i;
}

int FUnrealSkeletalMesh::AddBone(const FUnrealBone& b) {
	int i = 0;
	for (; i < bones.size(); i++) {
		if (b == bones[i]) {
			return i;
		}
	}
	bones.push_back(b);
	return i;
}

int FUnrealSkeletalMesh::AddWeight(const FUnrealBoneWeight& w) {
	int i = weights.size();
	weights.push_back(w);
	return i;
}

int FUnrealSkeletalMesh::AddAnimation(const FUnrealAnimInfo& a) {
	int i = animations.size();
	animations.push_back(a);
	return i;
}

int FUnrealSkeletalMesh::AddAnimKey(const FUnrealQuatAnimKey& k) {
	int i = animKeys.size();
	animKeys.push_back(k);
	return i;
}

void FUnrealSkeletalMesh::GetPskPsaPaths(const std::string& path, std::string& pskPath, std::string& psaPath) {
	std::string ext = GetFileExt(path);
	if (ext.compare("psk") == 0) {
		pskPath.assign(path);
		psaPath.assign(pskPath.substr(path.find_last_of('.')) + "psa");
	}
	else if (ext.compare("psa") == 0) {
		pskPath.assign(pskPath.substr(path.find_last_of('.')) + "psk");
		psaPath.assign(path);
	}
	else {
		pskPath.clear();
		psaPath.clear();
	}
}
