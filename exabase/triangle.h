
#ifndef _EXA_TRIANGLE_H_
#define _EXA_TRIANGLE_H_

#include "plane.h"

class triangle
{
public:
	vector v[3];

	triangle (const vector& p1, const vector& p2, const vector& p3) {
		v[0] = p1;
		v[1] = p2;
		v[2] = p3;
	}

	inline const triangle & operator= (const triangle& tr) {
		v[0] = tr.v[0];
		v[1] = tr.v[1];
		v[2] = tr.v[2];
		return *this;
	}

	inline operator plane() {
		plane p (v[0], v[1], v[2]);
		return p;
	}

	float planedist (const vector& P) {
		vector k = v[1] - v[0];
		vector l = v[2] - v[0];
		vector n = k ^ l;

		vector x = P - v[0];

		x -= n.unitvector() * (n.unitvector() % x);

		float x1, x2, k1, k2, div, p, q;
		int i;

		if ( (fabs (n.x) > fabs (n.y)) && (fabs (n.x) > fabs (n.z))) {
			x1 = x.y;
			x2 = x.z;
			k1 = k.y;
			k2 = k.z;
			div = n.x;
		} else if (fabs (n.y) > fabs (n.z)) {
			x1 = x.x;
			x2 = x.z;
			k1 = k.x;
			k2 = k.z;
			div = n.y;
		} else 	{
			x1 = x.x;
			x2 = x.y;
			k1 = k.x;
			k2 = k.y;
			div = n.z;
		}
		q = ( (x1 * k2) - (x2 * k1)) / div;
		if ( (fabs (k.x) > fabs (k.y)) && (fabs (k.x) > fabs (k.z)))
			i = 0;
		else if (fabs (k.y) > fabs (k.z)) i = 1;
		else i = 2;
		p = (x.v[i] - q * l.v[i]) / k.v[i];

		if (p < 0) {
			if (q < 0) return x.length();
			if (q < 1) return (n ^ l).unitvector() % x;
			return (x - l).length();
		}
		if (q < 0) {
			if (p < 1) return (k ^ n).unitvector() % x;
			return (x - k).length();
		}
		if (p + q < 1) return 0;
		if (p > q + 1) return (x - k).length();
		if (q > p + 1) return (x - l).length();
		return (k + l).length() * (p + q - 1) / 2;
	}
};


#endif
