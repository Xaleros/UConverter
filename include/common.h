#pragma once

#include <cstdint>
#include <iostream>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

class FVec3f {
public:
	void Serialize(std::ostream& stream) {
		stream << v[0];
		stream << v[1];
		stream << v[2];
	}

	void Deserialize(std::istream& stream) {
		stream >> v[0];
		stream >> v[1];
		stream >> v[2];
	}

	float operator[](int i) {
		return v[i];
	}

	float v[3];
};