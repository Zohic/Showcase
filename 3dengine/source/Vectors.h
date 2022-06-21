
#include <iostream>

struct Pnt2 // 2d point and vector
{
	float x, y;

	Pnt2()
	{
		x = y = 0;
	}

	Pnt2(float ix, float iy)
	{
		x = ix;
		y = iy;
	}

	Pnt2 operator +(Pnt2 v2)
	{
		return Pnt2(this->x + v2.x, this->y + v2.y);
	}
	Pnt2 operator +=(Pnt2 v2)
	{
		this->x += v2.x;
		this->y += v2.y;
		return *this;
	}
	Pnt2 operator -(Pnt2 v2)
	{
		return Pnt2(this->x - v2.x, this->y - v2.y);
	}
	Pnt2 operator -=(Pnt2 v2)
	{
		this->x -= v2.x;
		this->y -= v2.y;
		return *this;
	}
	Pnt2 operator *(float k)
	{
		return Pnt2(this->x * k, this->y * k);
	}
	Pnt2 operator *=(float k)
	{
		this->x *= k;
		this->y *= k;
		return *this;
	}


};

struct Pnt3 //3d point and vector
{
	float x, y, z;

	Pnt3()
	{
		x = y = z = 0;
	}
	Pnt3(float c)
	{
		x = c;
		y = c;
		z = c;
	}
	Pnt3(float ix, float iy, float iz)
	{
		x = ix;
		y = iy;
		z = iz;
	}

	Pnt3 operator +(Pnt3 v2)
	{
		return Pnt3(this->x + v2.x, this->y + v2.y, this->z + v2.z);
	}
	Pnt3 operator +=(Pnt3 v2)
	{
		this->x += v2.x; 
		this->y += v2.y; 
		this->z += v2.z;
		return *this;
	}
	Pnt3 operator -(Pnt3 v2)
	{
		return Pnt3(this->x - v2.x, this->y - v2.y, this->z - v2.z);
	}
	Pnt3 operator -=(Pnt3 v2)
	{
		this->x -= v2.x;
		this->y -= v2.y;
		this->z -= v2.z;
		return *this;
	}
	Pnt3 operator *(float k)
	{
		return Pnt3(this->x * k, this->y * k, this->z * k);
	}
	Pnt3 operator *=(float k)
	{
		this->x *= k;
		this->y *= k;
		this->z *= k;
		return *this;
	}

	Pnt3 copy()//returns point with same coords
	{
		return Pnt3(this->x, this->y, this->z);
	}

};

float Length(Pnt3 in)//returns length of vector
{
	float s = in.x * in.x + in.y * in.y + in.z * in.z;
	if (s > 0)
		return sqrtf(s);
	else return 0;
}



Pnt3 rotateX(Pnt3 in, float ang)//return point rotated around X coord
{
	float sn = sinf(ang);
	float cs = cosf(ang);
	return {in.x, in.y*cs - in.z*sn , in.z*cs + in.y*sn };
}

Pnt3 rotateXSinCos(Pnt3 in, float s,float c)//return point rotated around X coord
{
	float sn = s;
	float cs = c;
	return { in.x, in.y * cs - in.z * sn , in.z * cs + in.y * sn };
}



Pnt3 rotateY(Pnt3 in, float ang)//return point rotated around Y coord
{
	float sn = sinf(ang);
	float cs = cosf(ang);
	return { in.x*cs - in.z*sn, in.y, in.z*cs + in.x*sn };
}
Pnt3 rotateYSinCos(Pnt3 in, float s,float c)//return point rotated around Y coord
{
	float sn = s;
	float cs = c;
	return { in.x * cs - in.z * sn, in.y, in.z * cs + in.x * sn };
}


Pnt3 rotateZ(Pnt3 in, float ang)//return point rotated around Z coord
{
	float sn = sinf(ang);
	float cs = cosf(ang);
	return { in.x*cs - in.y*sn, in.y*cs + in.x*sn , in.z};
}
Pnt3 rotateZSinCos(Pnt3 in, float s, float c)//return point rotated around Z coord
{
	float sn = s;
	float cs = c;
	return { in.x * cs - in.y * sn, in.y * cs + in.x * sn , in.z };
}


Pnt3 rotate3d(Pnt3 in, Pnt3 ang, int order)//return point rotated around all 3 coords
{
	Pnt3 rp = in.copy();
	
	switch (order)
	{
	case 0:
		rp = rotateX(rp, ang.x);
		rp = rotateY(rp, ang.y);
		rp = rotateZ(rp, ang.z);
		break;
	case 1:
		rp = rotateX(rp, ang.x);
		rp = rotateZ(rp, ang.z);
		rp = rotateY(rp, ang.y);
		break;
	case 2:
		rp = rotateY(rp, ang.y);
		rp = rotateX(rp, ang.x);
		rp = rotateZ(rp, ang.z);
		break;
	case 3:
		rp = rotateZ(rp, ang.z);
		rp = rotateX(rp, ang.x);
		rp = rotateY(rp, ang.y);
		break;
	case 4:
		rp = rotateY(rp, ang.y);
		rp = rotateZ(rp, ang.z);
		rp = rotateX(rp, ang.x);
		break;
	case 5:
		rp = rotateZ(rp, ang.z);
		rp = rotateY(rp, ang.y);
		rp = rotateX(rp, ang.x);
		break;

	default:
		rp = rotateY(rp, ang.y);
		rp = rotateX(rp, ang.x);
		rp = rotateZ(rp, ang.z);
		break;
	}

	

	return rp;
}

Pnt3 sum3d(Pnt3 p1, Pnt3 p2)//returns summ of vectors
{
	return { p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
}

Pnt3 sub3d(Pnt3 p1, Pnt3 p2)//returns difference of vectors
{
	return { p1.x - p2.x, p1.y - p2.y, p1.z - p2.z };
}

Pnt3 mul3d(Pnt3 p1, float k)//returns vector multiplied by float
{
	return { p1.x*k, p1.y*k, p1.z*k };
}

Pnt3 div3d(Pnt3 p1, float k)//returns vector divided by float
{
	if(k!=0)
		return { p1.x / k, p1.y / k, p1.z / k };
	else return { p1.x, p1.y, p1.z };
}

float DotProduct(Pnt3 v1, Pnt3 v2)//returns dot product of two 3d vectors
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Pnt3 CrossProduct(Pnt3 v1, Pnt3 v2)
{
	return Pnt3(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x);
}

Pnt3 NormaliseVec(Pnt3 vec)//returns normalised vector
{
	return div3d(vec, Length(vec));
}

float Distance3d(Pnt3 p1, Pnt3 p2)
{
	Pnt3 s = sub3d(p1, p2);
	return Length(s);
}

/*Pnt3 Vector_IntersectPlane(Pnt3 &plane_p, Pnt3 &plane_n, Pnt3 &lineStart, Pnt3 &lineEnd)
{
	plane_n = NormaliseVec(plane_n);
	float plane_d = -DotProduct(plane_n, plane_p);
	float ad = DotProduct(lineStart, plane_n);
	float bd = DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Pnt3 lineStartToEnd = lineStartToEnd.dis(lineEnd, lineStart);
	Pnt3 lineToIntersect = lineToIntersect.mul(lineStartToEnd, t);
	return lineStartToEnd.sum(lineStart, lineToIntersect);
}*/