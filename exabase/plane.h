
#ifndef _EXA_PLANE_H_
#define _EXA_PLANE_H_

#include "vector.h"

class plane
{
public:
	vector n;
	float d;

	plane (const vector & N, const float D) : n (N.unitvector() ), d (D)
	{}

	plane (const plane & P)
	{
		n = P.n;
		d = P.d;
	}

	plane (const vector & p1, const vector & p2, const vector & p3)
	{
		n = ( (p2 - p1) ^ (p3 - p1) ).unitvector();
		d = (n.x * p1.x) + (n.y * p1.y) + (n.z * p1.z);
	}

	plane (const vector & p, const vector & norm)
	{
		n = norm.unitvector();
		d = (n.x * p.x) + (n.y * p.y) + (n.z * p.z);
	}

	float distance (const vector & p)
	{
		return (p % n) + d;
	}

	vector intersection (const vector&raypos, const vector& raydir)
	{
		return raypos -raydir* (distance (raypos) / (n % raydir) );
	}

	inline const plane & operator= (const plane& pl)
	{
		n = pl.n;
		d = pl.d;
		return *this;
	}

	inline const float operator% (const vector &v)
	{
		return distance (v);
	}
};
#endif
