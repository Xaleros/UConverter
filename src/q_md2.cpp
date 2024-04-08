#include <fstream>
#include <q_md2.h>

// References
// http://tfc.duke.free.fr/coding/md2-specs-en.html

void FMD2Model::Read(std::string& path) {
	std::ifstream stream(path, std::istream::in);
	stream >> header;

	skins.resize(header.numSkins);
	texcoords.resize(header.numSt);
	triangles.resize(header.numTris);
	frames.resize(header.numFrames);
	glcmds.resize(header.numGlCmds);

	stream.seekg(header.offsetSkins, std::ios_base::beg);
	for (FMD2Skin& skin : skins) {
		stream >> skin;
	}

	stream.seekg(header.offsetSt, std::ios_base::beg);
	for (FMD2TexCoord& tc : texcoords) {
		stream >> tc;
	}

	stream.seekg(header.offsetTris, std::ios_base::beg);
	for (FMD2Triangle& tri : triangles) {
		stream >> tri;
	}

	stream.seekg(header.numGlCmds, std::ios_base::beg);
	for (int& glcmd : glcmds) {
		stream >> glcmd;
	}

	stream.seekg(header.offsetFrames, std::ios_base::beg);
	for (FMD2Frame& frame : frames) {
		frame.verts.resize(header.numVertices);
		stream >> frame;
	}

	stream.close();
}

FUnrealLodMesh FMD2Model::Convert() {
	FUnrealLodMesh uMesh;

	uMesh.header.numPolygons = header.numTris;
	uMesh.header.numVertices = header.numVertices;

	FUnreal3DTri uTri;
	for (FMD2Triangle& tri : triangles) {
		for (int i = 0; i < 3; i++) {
			float s = static_cast<float>(texcoords[tri.st[i]].s) / header.skinWidth;
			float t = static_cast<float>(texcoords[tri.st[i]].t) / header.skinWidth;

			uTri.vertex[i] = tri.v[i];
			uTri.uv[i][0] = static_cast<uint16_t>(s * 255);
			uTri.uv[i][1] = static_cast<uint16_t>(t * 255);
		}
	}

	std::string lastAnimGroup = "";
	int lastFrameNum = 0;
	for (FMD2Frame& frame : frames) {
		for (FMD2Vertex& vert : frame.verts) {
			FVec3f v;
			for (int i = 0; i < 3; i++) {
				v[i] = (vert.v[i] * frame.scale[i]) * frame.translate[i];
			}

			uMesh.AddVertex(v);
		}

		std::string animGroup(frame.name);
		int frameNum = 0;
		for (int i = 0; i < animGroup.size(); i++) {
			if (isdigit(animGroup[i])) {
				frameNum = std::stoi(animGroup.substr(i));
				animGroup.erase(i, animGroup.size() - i);
			}
		}

		if (lastAnimGroup.compare(animGroup) != 0) {
			uMesh.AddSequence(lastAnimGroup, lastFrameNum);
		}

		lastAnimGroup = animGroup;
		lastFrameNum = frameNum;
	}

	for (FMD2Skin& skin : skins) {
		uMesh.AddTexture(std::string(skin.name));
	}
}