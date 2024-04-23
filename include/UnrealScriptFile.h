#pragma once
#include <string>
#include "Common.h"

class FUnrealScriptFile {
public:
	FUnrealScriptFile(const std::string& ucName);
	virtual ~FUnrealScriptFile();

	void AddNewLine();
	void AddMeshImport(const std::string& anivPath, const std::string& dataPath);
	void AddAnimSequence(const std::string& seqName, uint32_t startFrame, uint32_t numFrames);
	void AddTexture(const std::string& texName, const std::string& path);
	void AddMeshmap(const FVec3f& scale);
	void SetMeshmapTexture(const std::string& texName, uint32_t index);

	int Write(const std::string& path);

private:
	void AddCommentLine();

	std::string name;
	std::string buf;
};