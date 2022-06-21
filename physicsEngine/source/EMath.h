#pragma once

#include <math.h> 

const float EPI = 3.14159265f;


struct IVec2d
{
	int x, y;
	IVec2d()
	{
		x = y = 0;
	}
	IVec2d(int ix, int iy)
	{
		x = ix;
		y = iy;
	}
};

struct Vec2d
{
	float x, y;

	Vec2d()
	{
		x = y = 0;
	}

	Vec2d(float v)
	{
		x = y = v;
	}

	Vec2d(float ix, float iy)
	{
		x = ix;
		y = iy;
	}

	operator IVec2d() const
	{
		return IVec2d((int)x, (int)y);
	}

	Vec2d operator +(Vec2d v)
	{
		return Vec2d(x + v.x, y + v.y);
	}
	Vec2d operator -(Vec2d v)
	{
		return Vec2d(x - v.x, y - v.y);
	}
	Vec2d operator *(float v)
	{
		return Vec2d(x * v, y * v);
	}
	Vec2d operator /(float v)
	{
		return Vec2d(x / v, y / v);
	}

	Vec2d operator -()
	{
		return Vec2d(-x, -y);
	}

	bool operator ==(Vec2d v)
	{
		return (x == v.x) && (y == v.y);
	}

	void operator +=(Vec2d v)
	{
		x += v.x;
		y += v.y;
	}
	void operator -=(Vec2d v)
	{
		x -= v.x;
		y -= v.y;
	}
	void operator *=(float v)
	{
		x *= v;
		y *= v;
	}
	void operator /=(float v)
	{
		x /= v;
		y /= v;
	}

	float operator *(Vec2d v)// dot product
	{
		return x * v.x + y * v.y;
	}


	float operator &(Vec2d v)// cross product with vec
	{
		return x * v.y - y * v.y;
	}

	Vec2d operator &(float v)// cross product with float
	{
		return Vec2d(y * v, -x * v);
	}

	// (vec   X float) ^
	// (float X vec) = -(vec X float)


	float Length()
	{
		return sqrtf(x*x+y*y);
	}

	float sLength()
	{
		return x * x + y * y;
	}

	void Normalize()
	{
		float l = Length();
		if (l != 0)
		{
			x = x / l;
			y = y / l;
		}
	}

	Vec2d Normalized()
	{
		float l = Length();
		if(l!=0)
			return Vec2d(x / l, y / l);
		else
			return Vec2d(0, 0);
	}

	Vec2d GetNormal()
	{
		return Vec2d(-y, x);
	}

	float CrossProduct(Vec2d v)
	{
		return  x * v.y - y * v.x;
	}

	Vec2d CrossProduct(float v)
	{
		return Vec2d(y * v, -x * v);
	}

	float DotProduct(Vec2d v)
	{
		return x * v.x + y * v.y;
	}


};

static int esign(float x)
{
	return (int)(x > 0) - (int)(x < 0);
}

static float DotProduct(Vec2d a, Vec2d b)
{
	return a.x * b.x + a.y * b.y;
}

static float CrossProduct(Vec2d a, Vec2d b)//actually returns z value of perpendicular vector as if it was a 3d crossproduct
{
	return a.x * b.y - a.y * b.x;
}

static float SignCrossProduct(Vec2d a, Vec2d b)
{
	return esign(a.x * b.y - a.y * b.y);
}

static Vec2d CrossProduct(Vec2d a, float v)// returns a perpendicular to the given vector multiplied by value
{
	return Vec2d(a.y * v, -a.x * v);
}

static Vec2d CrossProduct(float a, Vec2d v)// returns the same but in opposite direction
{
	return Vec2d(-v.y * a, v.x * a);
}


struct Mat22
{
	float m00, m01, 
		  m10, m11;
	Mat22()
	{
		m00 = m01 = m10 = m11 = 0;
	}
	Mat22(float m1, float m2, float m3, float m4)
	{
		m00 = m1;
		m01 = m2;
		m10 = m3;
		m11 = m4;
	}
	Mat22(Vec2d v1, Vec2d v2)
	{
		m00 = v1.x; m01 = v1.y;
		m10 = v2.x; m11 = v2.y;
	}

	static Mat22 RotationMat(float a)
	{
		float s = sin(a);
		float c = cos(a);
		return Mat22(
					 c, -s, 
					 s,  c);
	}

	Vec2d operator *(Vec2d v)
	{
		return Vec2d(v.x * m00 + v.y * m01, v.x * m10 + v.y * m11);
	}

};

