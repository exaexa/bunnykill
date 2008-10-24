

#ifndef _EXA_COMPLEX_H_
#define _EXA_COMPLEX_H_

#include <math.h>


class complex
{

public:

	union{
		struct{
			float x, y;
		};
		float v[2];
	};

	complex (float a = 0, float b = 0) : x (a), y (b)
	{}

	complex (const complex & a)
	{
		x = a.x;
		y = a.y;
	}

	complex operator+ (const complex & a) const
	{
		return complex (a.x + x, a.y + y);
	}

	complex operator- (const complex & a) const
	{
		return complex (x -a.x, y - a.y);
	}

	complex operator* (float a) const
	{
		return complex (a*x, a*y);
	}

	friend complex operator* (float a, const complex & c)
	{
		return complex (a* (c.x), a* (c.y) );
	}

	complex operator/ (float a) const
	{
		return complex (x / a, y / a);
	}

	complex operator* (const complex& a) const
	{
		return complex ( (x*a.x) - (y*a.y) , (x*a.y) + (y*a.x) );
	}

	complex operator/ (const complex& a) const
	{
		return (*this*complex (a.x, -a.y) ) / (a.x*a.x + a.y*a.y);
	}

	const complex& operator+= (const complex& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	const complex& operator-= (const complex& a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	const complex operator*= (const float a)
	{
		(*this) = (*this) * a;
		return *this;
	}
	const complex operator/= (const float a)
	{
		(*this) = (*this) / a;
		return *this;
	}

	float length() const
	{
		return (float) pow ( (x*x) + (y*y), 0.5f);
	}
	complex unit() const
	{
		return (*this) / length();
	}
	complex operator| (const float& size) const
	{
		return size*unit();
	}
	const complex & operator|= (const float& size)
	{
		(*this) = size * unit();
		return *this;
	}
	float operator% (const complex& a)
	{  //dot product
		return x*a.x + y*a.y;
	}

	bool operator== (const complex& a) const
	{
		return (x == a.x) && (y == a.y);
	}
};

#endif //_EXA_COMPLEX_H_
