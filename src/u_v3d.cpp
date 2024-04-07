#include <fstream>
#include "u_v3d.h"

void FUnreal3DHeader::Serialize(std::ostream& stream)
{
	stream << numPolygons;
	stream << numVertices;
	stream << bogusRot;
	stream << bogusFrame;
	stream << bogusNormX;
	stream << bogusNormY;
	stream << bogusNormZ;
	stream << fixScale;
	stream << unused;
	stream << unknown;
}

void FUnreal3DHeader::Deserialize(std::istream& stream)
{
	stream >> numPolygons;
	stream >> numVertices;
	stream >> bogusRot;
	stream >> bogusFrame;
	stream >> bogusNormX;
	stream >> bogusNormY;
	stream >> bogusNormZ;
	stream >> fixScale;

	for (int i = 0; i < ARRAY_SIZE(unused); i++)
		stream >> unused[i];

	for (int i = 0; i < ARRAY_SIZE(unknown); i++)
		stream >> unknown;
}

void FUnreal3DTri::Serialize(std::ostream& stream) {
	stream << vertex;
	stream << type;
	stream << color;
	stream << uv;
	stream << texNum;
	stream << flags;
}

void FUnreal3DTri::Deserialize(std::istream& stream) {
	stream >> vertex[0];
	stream >> vertex[1];
	stream >> vertex[2];
	stream >> type;
	stream >> color;
	stream >> uv[0][0];
	stream >> uv[0][1];
	stream >> uv[1][0];
	stream >> uv[1][1];
	stream >> uv[2][0];
	stream >> uv[2][1];
	stream >> texNum;
	stream >> flags;
}

FUnreal3DSeq::FUnreal3DSeq()
	: name(""), length(0)
{}

FUnreal3DSeq::FUnreal3DSeq(std::string& _name, int _length)
	: name(_name), length(_length)
{}

FUnreal3DModel::FUnreal3DModel()
{}

FUnreal3DModel::~FUnreal3DModel()
{}

void FUnreal3DModel::AddVertex(FVec3f& vertex) {
	vertices.push_back( (int(vertex[0]*8.0) & 0x7ff) |
					  ( (int(vertex[1]*8.0) & 0x7ff) << 11 ) |
					  ( (int(vertex[2]*4.0) & 0x3ff) << 22) );
}

void FUnreal3DModel::AddPolygon(FUnreal3DTri& tri) {
	polygons.push_back(tri);
}

void FUnreal3DModel::AddTexture(std::string& name, int n) {
	if (textures.size() >= n) {
		textures.resize(n);
	}

	textures[n] = name;
}

void FUnreal3DModel::AddSequence(std::string& name, int n) {
	sequences.push_back(FUnreal3DSeq(name, n));
}

void FUnreal3DModel::Write(std::string& modelname) {
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

	FUnreal3DHeader hdr;
	memset(&hdr, 0, sizeof(hdr));

	stream.open(datapath, std::ofstream::out);
	hdr.numPolygons = polygons.size();
	hdr.numVertices = vertices.size() / numFrames;
	hdr.Serialize(stream);

	for (FUnreal3DTri& polygon : polygons) {
		polygon.Serialize(stream);
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