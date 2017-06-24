#ifndef __VECTOR_H
#define __VECTOR_H

//vector.h
//used by [exa]

#include <math.h>

class vector
{
public:
	union {
		struct {
			float x, y, z;
		};
		float v[3];
	};


public:
	vector (float a = 0, float b = 0, float c = 0) : x (a), y (b), z (c)
	{}
	vector (const vector &vec) : x (vec.x), y (vec.y), z (vec.z)
	{}

	const vector &operator= (const vector &vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;

		return *this;
	}

	const bool operator== (const vector &vec) const {
		return ( (x == vec.x) && (y == vec.y) && (z == vec.z));
	}

	const bool operator!= (const vector &vec) const {
		return ! (*this == vec);
	}

	const vector operator+ (const vector &vec) const {
		return vector (x + vec.x, y + vec.y, z + vec.z);
	}

	const vector operator+() const {
		return vector (*this);
	}

	const vector& operator+= (const vector vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}

	const vector operator- (const vector& vec) const {
		return vector (x - vec.x, y - vec.y, z - vec.z);
	}

	const vector operator-() const {
		return vector (-x, -y, -z);
	}

	const vector &operator-= (const vector vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return *this;
	}

	const vector &operator*= (const float &s) {
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	const vector &operator/= (const float &s) {
		const float t = 1 / s;

		x *= t;
		y *= t;
		z *= t;

		return *this;
	}

	const vector operator* (const float &s) const {
		return vector (x * s, y * s, z * s);
	}

	friend inline const vector operator* (const float &s, const vector &vec) {
		return vec * s;
	}

	const vector operator/ (float s) const {
		s = 1 / s;

		return vector (s * x, s * y, s * z);
	}

	const vector crossproduct (const vector &vec) const {
		return vector (y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	const vector operator^ (const vector &vec) const {
		return vector (y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	const float dotproduct (const vector &vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}

	const float operator% (const vector &vec) const {
		return x * vec.x + y * vec.y + z * vec.z;
	}


	const float length() const {
		return (float) sqrt ( (double) (x * x + y * y + z * z));
	}

	const vector unitvector() const {
		return (*this) / length();
	}

	void normalize() {
		(*this) /= length();
	}

	const float operator!() const {
		return sqrtf (x * x + y * y + z * z);
	}

	const vector operator | (const float length) const {
		if (length == 0) return vector (0, 0, 0);
		register float x;
		if ( (x = ! (*this)) == 0) return vector (0, 0, 0);
		return *this * (length / ! (*this));
	}

	const vector& operator |= (const float length) {
		return *this = *this | length;
	}

	inline const float angle (const vector& normal) const {
		return acosf (*this % normal);
	}

	inline const vector reflection (const vector& normal) const {
		const vector vec (*this | 1);
		return (vec - normal * 2.0 * (vec % normal)) * !*this;
	}

};

#endif
