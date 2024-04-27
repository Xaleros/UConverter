#pragma once
#include <string>
#include "Common.h"

class FUnrealScriptFile {
public:
	FUnrealScriptFile(const std::string& ucName);
	virtual ~FUnrealScriptFile();

	// Generic exec's
	void AddAnimSequence(const std::string& seqName, uint32_t startFrame, uint32_t numFrames, uint32_t trackTime = 0);
	void AddMeshmapScale(const FVec3f& scale);
	void AddNewLine();
	void AddTexture(const std::string& texName, const std::string& path);

	// LodMesh specific exec's
	void AddMeshImport(const std::string& anivPath, const std::string& dataPath);
	void AddMeshmap();
	void SetMeshmapTexture(const std::string& texName, uint32_t index);

	// Skeletal mesh specific exec's
	void AddModelImport(const std::string& pskPath);
	void AddAnimImport(const std::string& psaName, const std::string& psaPath);
	void AddAnimDigest();
	void SetMeshDefaultAnim();

	int Write(const std::string& path);

private:
	void AddCommentLine();

	std::string name;
	std::string anim;
	std::string buf;
};