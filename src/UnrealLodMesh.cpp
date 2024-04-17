#include <fstream>
#include "UnrealLodMesh.h"

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

	std::string comment = "//" + std::string(77, '=') + "\n";
	std::string seqstr = "#exec MESH SEQUENCE MESH=" + modelName + " SEQ=";

	stream.open(ucpath, std::ofstream::out);
	if (!stream.is_open()) {
		std::cout << "Failed to open " << ucpath << std::endl;
		return -1;
	}

	stream << comment;
	stream << "// " + modelName + ".\n";
	stream << comment;
	stream << "class " + modelName + "expands Actor;\n\n";
	stream << "#exec MESH IMPORT MESH=" + modelName + " ANIVFILE=" + anivfile + " DATAFILE=" + datafile + " MLOD=0\n";
	stream << "#exec MESH ORIGIN MESH=" + modelName + " X=0 Y=0 Z=0 YAW=128\n\n";

	stream << seqstr + "All STARTFRAME=0 NUMFRAMES=" + std::to_string(numFrames) + "\n";

	int startFrame = 0;
	for (FUnreal3DSeq seq : sequences) {
		stream << seqstr + seq.name + " STARTFRAME=" + std::to_string(startFrame) + " NUMFRAMES=" + std::to_string(seq.length) + "\n";
	}

	stream << "\n";

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelName + std::to_string(i);
			stream << "#exec TEXTURE IMPORT NAME=" + texname + " FILE=Models\\" + texname + ".bmp GROUP=Skins";
		}
		stream << "\n";
	}

	stream << "#exec MESHMAP NEW MESHMAP=" + modelName + " MESH=" + modelName + "\n";
	stream << "#exec MESHMAP SCALE MESHMAP=" + modelName + " X=-0.1875 Y=-0.1875 Z=0.375\n\n";

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelName + std::to_string(i);
			stream << "#exec MESHMAP SETTEXTURE MESHMAP=" + modelName + " NUM=" + std::to_string(i) + " TEXTURE=" + texname + "\n";
		}
	}

	stream << "\ndefaultproperties\n{\n";
	stream << "\tDrawType=DT_Mesh\n";
	stream << "\tMesh=" + modelName + "\n";
	stream << "}\n";

	stream.close();
	return 0;
}