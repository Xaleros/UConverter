#include <fstream>
#include "u_v3d.h"

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
	vertices.push_back( (int(vertex[0]*8.0) & 0x7ff) |
					  ( (int(vertex[1]*8.0) & 0x7ff) << 11 ) |
					  ( (int(vertex[2]*4.0) & 0x3ff) << 22) );
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

void FUnrealLodMesh::Write(std::string& modelname) {
	std::string datafile = "Models\\" + modelname + "_d.3d";
	std::string anivfile = "Models\\" + modelname + "_a.3d";

	std::string datapath = "\\" + datafile;
	std::string anivpath = "\\" + anivfile;
	std::string ucpath   = "\\Classes\\" + modelname + ".uc";

	int numFrames = 0;
	for (FUnreal3DSeq& seq : sequences) {
		numFrames += seq.length;
	}

	if (numFrames > UINT16_MAX) {
		throw std::runtime_error("too many animation frames: 65536 < " + std::to_string(numFrames));
	}

	std::ofstream stream;

	stream.open(datapath, std::ofstream::out);

	header.numPolygons = polygons.size();
	header.numVertices = vertices.size() / numFrames;
	stream << header;

	for (FUnreal3DTri& polygon : polygons) {
		stream << polygon;
	}

	stream.close();

	stream.open(anivpath, std::ofstream::out);
	stream << uint16_t(numFrames);
	stream << uint16_t(vertices.size() * sizeof(uint32_t) / numFrames);
	for (uint32_t vertex : vertices) {
		stream << vertex;
	}

	stream.close();

	std::string comment = "//" + std::string(77, '=') + "\n";
	std::string seqstr = "#exec MESH SEQUENCE MESH=" + modelname + "SEQ=";
	stream.open(ucpath, std::ofstream::out);

	stream << comment;
	stream << "// " + modelname + ".\n";
	stream << comment;
	stream << "class " + modelname + "expands Actor;\n\n";
	stream << "#exec MESH IMPORT MESH=" + modelname + "ANIVFILE=" + anivfile + "DATAFILE=" + datafile + "X=0 Y=0 Z=0\n";
	stream << "#exec MESH ORIGIN MESH=" + modelname + "X=0 Y=0 Z=0\n\n";

	stream << seqstr + "All STARTFRAME=0 NUMFRAMES=" + std::to_string(numFrames) + "\n";

	int startFrame = 0;
	for (FUnreal3DSeq seq : sequences) {
		stream << seqstr + seq.name + " STARTFRAME=" + std::to_string(startFrame) + "NUMFRAMES=" + std::to_string(seq.length) + "\n";
	}

	stream << "\n";

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelname + std::to_string(i);
			stream << "#exec TEXTURE IMPORT NAME=" + texname + " FILE=Models\\" + texname + "GROUP=Skins";
		}
		stream << "\n";
	}

	stream << "#exec MESHMAP NEW MESHMAP=" + modelname + "MESH=" + modelname + "\n";
	stream << "#exec MESHMAP SCALE MESHMAP=" + modelname + "X=0.1 Y=0.1 Z=0.2\n\n";

	if (textures.size() > 0) {
		for (int i = 0; i < textures.size(); i++) {
			std::string texname = modelname + std::to_string(i);
			stream << "#exec MESHMAP SETTEXTURE MESHMAP=" + modelname + "NUM=" + std::to_string(i) + "TEXTURE=" + texname + "\n";
		}
	}

	stream << "\ndefaultproperties\n{\n";
	stream << "\tDrawType=DT_Mesh\n";
	stream << "\tMesh=" + modelname + "\n";
	stream << "}\n";

	stream.close();
}