#pragma once

#include "common.h"
#include <string>
#include <vector>

class FUnreal3DHeader {
public:
	void Serialize(std::ostream& stream);
	void Deserialize(std::istream& stream);

	uint16_t numPolygons;
	uint16_t numVertices;
	uint16_t bogusRot;
	uint16_t bogusFrame;
	uint32_t bogusNormX;
	uint32_t bogusNormY;
	uint32_t bogusNormZ;
	uint32_t fixScale;
	uint32_t unused[3];
	uint8_t  unknown[12];
};

class FUnreal3DTri {
public:
	void Serialize(std::ostream& stream);
	void Deserialize(std::istream& stream);

	uint16_t vertex[3];
	char	 type;
	char	 color;
	uint8_t  uv[3][2];
	char     texNum;
	char	 flags;
};

class FUnreal3DSeq {
public:
	FUnreal3DSeq();
	FUnreal3DSeq(std::string& _name, int _length);

	std::string name;
	int length;
};

class FUnreal3DModel {
public:
	FUnreal3DModel();
	virtual ~FUnreal3DModel();

	void AddVertex(FVec3f& vertex);
	void AddPolygon(FUnreal3DTri& tri);
	void AddTexture(std::string& name, int n);
	void AddSequence(std::string& name, int len);

	void Write(std::string& modelname);

private:
	std::vector<uint32_t> vertices;
	std::vector<FUnreal3DTri> polygons;
	std::vector<FUnreal3DSeq> sequences;
	std::vector<std::string> textures;
};
/*
class FUnreal3DAniv {
public:
	void Serialize(std::ostream& stream) {
		stream << vertices.size() * sizeof(uint32_t) / 
		for (FVec3f& vertex : vertices) {
			stream << assemble_vertex(vertex);
		}
	}

private:
	inline uint32_t assemble_vertex(FVec3f& vertex) {
		return	(((int)(vertex[0] * 8.0) & 0x7ff)) |
				(((int)(vertex[1] * 8.0) & 0x7ff) << 11) |
				(((int)(vertex[2] * 4.0) & 0x3ff) << 22);
	}
	std::vector<FVec3f> vertices;
};
*/
