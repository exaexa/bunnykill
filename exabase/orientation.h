
#ifndef _EXA_ORIENTATION_H_
#define _EXA_ORIENTATION_H_
#include "vector.h"

#define rotation vector

#ifndef _EXA_OMIT_GL_
#include <GL/gl.h>
#endif

class orientation
{
public:
	vector fw, up;
	orientation()
	{
		fw = vector (0, 0, -1);
		up = vector (0, 1, 0);
	}
	orientation (const vector &forward, const vector &top)
	{
		fw = forward;
		up = top;
	}
	orientation (float fwx, float fwy, float fwz, float upx, float upy, float upz)
	{
		fw = vector (fwx, fwy, fwz);
		up = vector (upx, upy, upz);
	}

	void check()
	{
		fw = fw.unitvector();
		up = up.unitvector();
	}

	void checkangle() //keep angle of vectors!
	{
		check();
		up = (fw ^ up) ^ fw;
	}

	inline const orientation checked() const
	{
		return orientation (fw.unitvector(), up.unitvector() );
	}

	const orientation& operator= (const orientation& ori)
	{
		up = ori.up;
		fw = ori.fw;
		return *this;
	}

	const bool operator== (const orientation& ori) const
	{
		return (fw == ori.fw) && (up == ori.up);
	}

	const bool operator!= (const orientation& ori) const
	{
		return ! (*this == ori);
	}

	const orientation operator+ (const rotation r) const
	{
		/*
		 * following way:
		 * 1- subtract plane distances from vectors
		 * 	(plane's normal is R)
		 * 2- rotate them
		 * 3- add back distances
		 */

		vector t, u, nfw, nup;
		float size, size2, fwd, upd;

		//ROTATION -- +Z axis is "forward" (!!)
		//X axis is L/R, Y axis is topdown
		//u is the real rotation vector - 1 done

		size = !r; //get vector size
		if (size == 0) return orientation (fw, up);
		u = r / size; //normalize it

		fwd = fw % u;
		upd = up % u;
		nfw = fw - (u * fwd);
		nup = up - (u * upd);

		size2 = cosf (size);
		size = sinf (size);
		t = u ^ up;
		nup = size2 * nup + size * t;
		t = u ^ fw;
		nfw = size2 * nfw + size * t;

		nfw += fwd * u;
		nup += upd * u;

		return orientation (nfw, nup);
	}

	inline const vector right()
	{
		return fw ^ up;
	}
	inline const vector left()
	{
		return up ^ fw;
	}

	/* //TODO this one needs fixing - could be usuable later
	 * 
	const rotation rotatefrom(const orientation& ori1) const
	{
		//FIXME doesn't work!
		vector axis;
		axis=((ori1.fw+this->fw)^(ori1.fw^this->fw))
			^((ori1.up+this->up)^(ori1.up^this->up));
		if(!axis==0)return axis;
		axis.normalize();
		float t1=axis%ori1.fw;
		float t2=axis%ori1.up;
		vector t;
		if(t2>t1)
		{
			t=axis*t1;
			if((ori1.fw^this->fw)%t>0)
			return -axis|(((ori1.fw+t).unitvector()).angle((this->fw+t).unitvector()));
			else
			return axis|(((ori1.fw+t).unitvector()).angle((this->fw+t).unitvector()));
		}
		else
		{
			t=axis*t2;
			if((ori1.up^this->up)%t>0)
			return -axis|(((ori1.up+t).unitvector()).angle((this->up+t).unitvector()));
			else
			return axis|(((ori1.up+t).unitvector()).angle((this->up+t).unitvector()));
		}
	}

	*/

#ifndef _EXA_ORIENTATION_OMIT_GL_
	void glrotate() const
	{
		float m[4][4];
		glrotmatrix (m);
		glMultMatrixf (&m[0][0]);
	}

	void glrotmatrix (float m[4][4]) const
	{
		vector side = fw ^ up;
		m[0][0] = side.x;
		m[0][1] = side.y;
		m[0][2] = side.z;
		m[0][3] = 0;
		m[1][0] = up.x;
		m[1][1] = up.y;
		m[1][2] = up.z;
		m[1][3] = 0;
		m[2][0] = fw.x;
		m[2][1] = fw.y;
		m[2][2] = fw.z;
		m[2][3] = 0;
		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		m[3][3] = 1;
	}

#endif

	const orientation operator- (const rotation& r) const
	{
		return *this + (-r);
	}

	//relates a vector to the orientation. +Z is FORWARD!
	const vector relate (const vector& r) const
	{
		return vector (fw* (r.z) + up* (r.y) + (fw ^ up) * (r.x) );
	}

	inline const orientation& operator+= (const rotation& r)
	{
		*this = *this + r;
		return *this;
	}

	inline const orientation& operator-= (const rotation& r)
	{
		*this = *this + (-r);
		return *this;
	}

	// -ori = rotate 180deg by the side axis
	inline const orientation reverse() const
	{
		return orientation (-fw, -up);
	}

	//!ori = "backwards" - rotate 180deg around vertical axis
	inline const orientation backwards() const
	{
		return orientation (-fw, up);
	}

	//^ori = "upside down" - rotate 180deg around "forward" axis
	inline const orientation bottomup() const
	{
		return orientation (fw, -up);
	}
};


#endif
