#include <fstream>
#include "UnrealLodMesh.h"
#include "UnrealScriptFile.h"

FUnreal3DSeq::FUnreal3DSeq()
	: name(""), length(0)
{}

FUnreal3DSeq::FUnreal3DSeq(std::string& _name, int _length)
	: name(_name), length(_length)
{}

FUnrealLodMesh::FUnrealLodMesh()
{}

FUnrealLodMesh::~FUnrealLodMesh()
{}

void FUnrealLodMesh::AddVertex(FVec3f& vertex) {
	int v = ((int)(vertex[0] * 8.0) & 0x7ff) | (((int)(vertex[1] * 8.0) & 0x7ff) << 11) | (((int)(vertex[2] * 4.0) & 0x3ff) << 22);
	vertices.push_back(v);
}

void FUnrealLodMesh::AddPolygon(FUnreal3DTri& tri) {
	polygons.push_back(tri);
}

void FUnrealLodMesh::AddTexture(std::string& name, int n) {
	if (n < 0) {
		textures.push_back(name);
	}
	else if (textures.size() >= n) {
		textures.resize(n);
		textures[n] = name;
	}
}

void FUnrealLodMesh::AddSequence(std::string& name, int n) {
	sequences.push_back(FUnreal3DSeq(name, n));
}

int FUnrealLodMesh::Write(std::string& outputPath, std::string& modelName) {
	std::string datafile = "Models\\" + modelName + "_d.3d";
	std::string anivfile = "Models\\" + modelName + "_a.3d";

	std::string datapath = outputPath + "\\" + datafile;
	std::string anivpath = outputPath + "\\" + anivfile;
	std::string ucpath   = outputPath + "\\Classes\\" + modelName + ".uc";

	int numFrames = 0;
	for (FUnreal3DSeq& seq : sequences) {
		numFrames += seq.length;
	}

	if (numFrames == 0) {
		numFrames = 1;
	}

	if (numFrames > UINT16_MAX) {
		std::cout << "too many animation frames: 65536 < " << std::to_string(numFrames) << std::endl;
		return -1;
	}

	/////////////////////////////////////////////////////////
	// Write data file
	/////////////////////////////////////////////////////////

	std::ofstream stream;

	stream.open(datapath, std::ofstream::out | std::istream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << datapath << std::endl;
		return -1;
	}

	header.numPolygons = polygons.size();
	header.numVertices = vertices.size() / numFrames;
	stream << header;

	for (FUnreal3DTri& polygon : polygons) {
		stream << polygon;
	}

	stream.close();

	/////////////////////////////////////////////////////////
	// Write aniv file
	/////////////////////////////////////////////////////////

	stream.open(anivpath, std::ofstream::out | std::istream::binary);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << anivpath << std::endl;
		return -1;
	}

	uint16_t anivNumFrames = uint16_t(numFrames);
	uint16_t anivFrameSize = uint16_t(vertices.size() * sizeof(uint32_t) / numFrames);
	stream.write((char*)&anivNumFrames, sizeof(anivNumFrames));
	stream.write((char*)&anivFrameSize, sizeof(anivFrameSize));
	for (uint32_t vertex : vertices) {
		stream.write((char*)&vertex, sizeof(vertex));
	}

	stream.close();

	/////////////////////////////////////////////////////////
	// Write UC file
	/////////////////////////////////////////////////////////

	FUnrealScriptFile script(modelName);
	script.AddMeshImport(anivfile, datafile);
	script.AddAnimSequence("All", 0, numFrames);
	script.AddNewLine();

	int startFrame = 0;
	for (FUnreal3DSeq seq : sequences) {
		script.AddAnimSequence(seq.name, startFrame, seq.length);
		startFrame += seq.length;
	}

	script.AddNewLine();

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelName + std::to_string(i);
			script.AddTexture(texname, "Models\\" + texname + ".bmp");
		}
		script.AddNewLine();
	}

	script.AddMeshmap();
	script.AddMeshmapScale(FVec3f(-0.1875f, -0.1875f, 0.375f));

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelName + std::to_string(i);
			script.SetMeshmapTexture(texname, i);
		}
	}

	int rc = script.Write(ucpath);
	if (rc < 0) {
		std::cout << "Failed to write script to " + ucpath << std::endl;
	}

	return rc;
}