#include "UnrealScriptFile.h"

FUnrealScriptFile::FUnrealScriptFile(const std::string& ucName) {
	name.assign(ucName);
	buf.clear();

	AddCommentLine();
	buf += "//" + name + ".\n";
	AddCommentLine();
	buf += "class " + name + "expands Actor;\n\n";
}

FUnrealScriptFile::~FUnrealScriptFile() {
	name.clear();
	name.clear();
}

void FUnrealScriptFile::AddNewLine() {
	buf += "\n";
}

void FUnrealScriptFile::AddMeshImport(const std::string& anivPath, const std::string& dataPath) {
	buf += "#exec MESH IMPORT MESH=" + name + " ANIVFILE=" + anivPath + " DATAFILE=" + dataPath + " MLOD=0\n";
	buf += "#exec MESH ORIGIN MESH=" + name + " X=0 Y=0 Z=0 YAW=128\n\n";
}

void FUnrealScriptFile::AddAnimSequence(const std::string& seqName, uint32_t startFrame, uint32_t numFrames) {
	buf += "#exec MESH SEQUENCE MESH=" + name + " SEQ=" + seqName + " STARTFRAME=" + std::to_string(startFrame) + " NUMFRAMES=" + std::to_string(numFrames) + "\n";
}

void FUnrealScriptFile::AddTexture(const std::string& texName, const std::string& path) {
	buf += "#exec TEXTURE IMPORT NAME=" + texName + " FILE=" + path + " GROUP=Skins\n";
}

void FUnrealScriptFile::AddMeshmap(const FVec3f& scale) {
	buf += "#exec MESHMAP NEW MESHMAP=" + name + " MESH=" + name + "\n";
	buf += "#exec MESHMAP SCALE MESHMAP=" + name + " X=" + std::to_string(scale.v[0]) + " Y=" + std::to_string(scale.v[1]) + " Z=" + std::to_string(scale.v[2]) + "\n\n";
}

void FUnrealScriptFile::SetMeshmapTexture(const std::string& texName, uint32_t index) {
	buf += "#exec MESHMAP SETTEXTURE MESHMAP=" + name + " NUM=" + std::to_string(index) + " TEXTURE=" + texName + "\n";
}

int FUnrealScriptFile::Write(const std::string& path) {
	std::ofstream stream(path);
	if (!stream.is_open()) {
		return -1;
	}

	buf += "\ndefaultproperties\n{\n";
	buf += "\tDrawType=DT_Mesh\n";
	buf += "\tMesh=" + name + "\n";
	buf += "}\n";

	stream.write(buf.c_str(), buf.size());
	stream.close();
	return 0;
}

void FUnrealScriptFile::AddCommentLine() {
	buf += "//" + std::string(77, '=') + "\n";
}