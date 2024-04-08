#pragma once

#include <cstdint>
#include <iostream>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

class FVec3f {
public:
	friend std::ostream& operator<<(std::ostream& stream, FVec3f& vec) {
		stream << vec;
	}

	friend std::istream& operator>>(std::istream& stream, FVec3f& vec) {
		stream >> vec.v[0];
		stream >> vec.v[1];
		stream >> vec.v[2];
	}

	float& operator[](int i) {
		return v[i];
	}

	float v[3];
};
