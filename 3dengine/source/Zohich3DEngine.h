#include "olcPixelGameEngine.h"
#include "Vectors.h"
#include<algorithm>
#include <fstream>
#include <strstream>

const float PI = 3.141592f;

float xCof = 1;// 1 / tanf((3.1415f / 2) / 2);
float yCof = 1;// 1 / tanf((3.1415f / 2) / 2);
float zCof = 1;// 1;

#undef max
#undef min

void setPerspective(float AngOfView)
{
	xCof = 1.0f / tanf((AngOfView) / 2);
	yCof = 1.0f / tanf((AngOfView) / 2);
	zCof = 0.001f;
}

float deg = PI / 180.0f;

Pnt3 camPos = Pnt3();//main camera position
Pnt3 camAng = Pnt3();//main camera look angle

float clipDistance = 1.1f;
float maxDistance = 40000;
float* zBuffer;
float globW = 1.0f;


struct LightSoucrce
{
	Pnt3 position;
	olc::Pixel color;
	float power = 1;
	int type;//0-directional, 1-point
	LightSoucrce()
	{}

	LightSoucrce(int itype, Pnt3 dirpos)
	{
		type = itype;
		position = dirpos;
		color = olc::WHITE;
	}

};

Pnt2 convert(Pnt3 pi)//converts 3d point to 2d screen , projection
{

	if (pi.z > 0)
	{
		pi.z = std::max(clipDistance, pi.z);
		
		return Pnt2(pi.x * xCof / (pi.z * zCof), pi.y * yCof / (pi.z * zCof));
	}
		
	else return Pnt2(100, 100);
}

Pnt3 antiConvert(Pnt2 pi, float iz)//converts 3d point to 2d screen , projection
{

	if (iz > 0)
	{
		return Pnt3(pi.x / xCof * (iz * zCof), pi.y / yCof * (iz * zCof), iz);
	}
	else return Pnt3(0);
}

void drawVector(Pnt3 p1, Pnt3 p2, olc::Pixel clr, olc::PixelGameEngine p)
{
	Pnt2 pd1 = convert(rotate3d(sub3d(p1, camPos), camAng, 2));
	Pnt2 pd2 = convert(rotate3d(sub3d(p2, camPos), camAng, 2));

	int cenX = floorf(p.ScreenWidth() / 2);
	int cenY = floorf(p.ScreenHeight() / 2);
	p.DrawLine(pd1.x + cenX, pd1.y + cenY, pd2.x + cenX, pd2.y + cenY, clr);
}


struct Collider
{

	int type = 0;
	Collider()
	{

	}
	virtual bool RayCollision(Pnt3 begin, Pnt3 dir, Pnt3& outPoint)
	{
		return false;
	}
	virtual bool aabbCollision(Pnt3 begin, Pnt3 end)
	{
		return false;
	}
	virtual void Draw(olc::PixelGameEngine gfx)
	{
		gfx.DrawCircle(20,20,10,olc::RED);
	}
	virtual void Move(Pnt3 moveV)
	{

	}
	virtual Pnt3 getPosition()
	{
		return Pnt3(0);
	}
	virtual Pnt3 getSize()
	{
		return Pnt3(0);
	}

};

struct Sphere: public Collider
{
	float radius;
	Pnt3 position;
	Sphere()
	{
		radius = 1;
		type = 0;
	}
	Sphere(Pnt3 pos, float r)
	{
		position = pos;
		radius = r;
	}
	bool RayCollision(Pnt3 begin, Pnt3 dir, Pnt3& outPoint) override
	{
		Pnt3 toCen = position - begin;

		float closestT = DotProduct(toCen, dir);

		Pnt3 closest = begin + dir * closestT;

		outPoint = closest;

		if (Distance3d(closest, position) <= radius)
			return true;
		else return false;
	}
	void Draw(olc::PixelGameEngine gfx) override
	{
		Pnt3 viewPos = rotate3d(sub3d(position, camPos), camAng, 2);
		Pnt2 screenPos = convert(viewPos);

		drawVector(position, position + Pnt3(radius, 0, 0), olc::GREEN, gfx);
		drawVector(position, position + Pnt3(-radius, 0, 0), olc::GREEN, gfx);
		drawVector(position, position + Pnt3(0, radius, 0), olc::GREEN, gfx);
		drawVector(position, position + Pnt3(0, -radius, 0), olc::GREEN, gfx);
		drawVector(position, position + Pnt3(0, 0, radius), olc::GREEN, gfx);
		drawVector(position, position + Pnt3(0, 0, -radius), olc::GREEN, gfx);

		int cenX = floorf(gfx.ScreenWidth() / 2);
		int cenY = floorf(gfx.ScreenHeight() / 2);

		gfx.DrawCircle(screenPos.x+cenX, screenPos.y+cenY,radius/(viewPos.z*zCof),olc::GREEN);
	}
	void Move(Pnt3 moveV) override
	{
		position += moveV;
	}
	Pnt3 getPosition() override
	{
		return position;
	}
	Pnt3 getSize() override
	{
		return Pnt3(radius);
	}
};

bool RaySphereCollision(Pnt3 begin, Pnt3 dir, Sphere s)
{
	Pnt3 toCen = s.position - begin;

	float closestT = DotProduct(toCen, dir);

	Pnt3 closest = begin + dir * closestT;

	if (Distance3d(closest, s.position) <= s.radius)
		return true;
	else return false;

}

bool RaySphereCollision(Pnt3 begin, Pnt3 dir, Sphere s, Pnt3& outPoint)
{
	Pnt3 toCen = s.position - begin;

	float closestT = DotProduct(toCen, dir);

	Pnt3 closest = begin + dir * closestT;

	outPoint = closest;

	if (Distance3d(closest, s.position) <= s.radius)
		return true;
	else return false;

}

bool RectLineIntersection(Pnt2 rB, Pnt2 rS, Pnt2 lO, Pnt2 lD)
{
	Pnt2 enter = Pnt2(0, 0);
	float t = (rB.y - lO.y)/lD.y;
	enter.y = rB.y;
	enter.x = lO.x + lD.x * t;
	if (enter.x >= rB.x && enter.x <= rB.x + rS.x && t > 0)
	{
		return true;
	}
	else
	{
		t = (rB.y+rS.y - lO.y) / lD.y;
		enter.y = rB.y + rS.y;
		enter.x = lO.x + lD.x * t;
		if (enter.x >= rB.x && enter.x <= rB.x + rS.x && t> 0)
		{
			return true;
		}
		else
		{

			t = (rB.x - lO.x) / lD.x;
			enter.x = rB.x;
			enter.y = lO.y + lD.y * t;
			if (enter.y >= rB.y && enter.y <= rB.y + rS.y && t > 0)
			{
				return true;
			}
			else
			{
				t = (rB.x + rS.x - lO.x) / lD.x;
				enter.x = rB.x + rS.x;
				enter.y = lO.y + lD.y * t;
				if (enter.y >= rB.y && enter.y <= rB.y + rS.y && t > 0)
				{
					return true;
				}
				else
					return false;
			}

		}
		
	}
	
}

struct AABB : public Collider
{
	Pnt3 begin;
	Pnt3 end;
	Pnt3 size;
	AABB()
	{
		type = 1;
	}
	AABB(Pnt3 b, Pnt3 e)
	{
		begin = b;
		end = e;
		size = end - begin;
		type = 1;
	}
	AABB(Pnt3 b, Pnt3 s, bool yeah)
	{
		begin = b;
		size = s;
		end = begin + size;
		type = 1;
	}
	void setBegin(Pnt3 b)
	{
		begin = b;
		end = b + size;
	}

	void moveBegin(Pnt3 m)
	{
		setBegin(begin+m);
	}

	void setSize(Pnt3 s)
	{
		size = s;
		end = begin + s;
	}

	bool RayCollision(Pnt3 beg, Pnt3 dir, Pnt3& outPoint) override
	{

		Pnt2 rectB = Pnt2(begin.x, begin.z);
		Pnt2 rectS = Pnt2(size.x, size.z);

		Pnt2 lineO = Pnt2(beg.x, beg.z);
		float len = sqrt(dir.x* dir.x + dir.z* dir.z);
		Pnt2 lineD = Pnt2(dir.x / len, dir.z / len);

		bool onTop = RectLineIntersection(rectB, rectS, lineO, lineD);

		rectB = Pnt2(begin.x, begin.y);
		rectS = Pnt2(size.x, size.y);

		lineO = Pnt2(beg.x, beg.y);
		len = sqrt(dir.x * dir.x + dir.y * dir.y);
		lineD = Pnt2(dir.x / len, dir.y / len);

		bool fromSide = RectLineIntersection(rectB, rectS, lineO, lineD);

		rectB = Pnt2(begin.y, begin.z);
		rectS = Pnt2(size.y, size.z);

		lineO = Pnt2(beg.y, beg.z);
		len = sqrt(dir.y * dir.y + dir.z * dir.z);
		lineD = Pnt2(dir.y / len, dir.z / len);

		bool inFront = RectLineIntersection(rectB, rectS, lineO, lineD);


		return onTop&&fromSide&&inFront;

	}

	bool aabbCollision(Pnt3 iBegin, Pnt3 iEnd) override
	{
		if (end.x > iBegin.x && begin.x < iEnd.x)
			if (end.y > iBegin.y && begin.y < iEnd.y)
				if (end.z > iBegin.z && begin.z < iEnd.z)
					return true;
		return false;
	}


	void Draw(olc::PixelGameEngine gfx) override
	{
		
		Pnt3 side = Pnt3(size.x,0,0);
		Pnt3 up = Pnt3(0, size.y, 0);
		Pnt3 front = Pnt3(0, 0, size.z);

		drawVector(begin,end, olc::GREEN, gfx);
		drawVector(begin, begin + side, olc::GREEN, gfx);
		drawVector(begin, begin + front, olc::GREEN, gfx);
		drawVector(begin, begin + up, olc::GREEN, gfx);

		drawVector(begin + up, begin + up + side, olc::GREEN, gfx);
		drawVector(begin + up + side, begin + side, olc::GREEN, gfx);

		drawVector(begin + up, begin + up + front, olc::GREEN, gfx);
		drawVector(begin + up + side, begin + up + side + front, olc::GREEN, gfx);
		drawVector(begin + side, begin + side + front, olc::GREEN, gfx);

		drawVector(begin + up + front + side, begin + up + front, olc::GREEN, gfx);
		drawVector(begin + up + front - up, begin + up + front, olc::GREEN, gfx);
		drawVector(begin + up + front - up, begin + up + front - up + side, olc::GREEN, gfx);
		drawVector(begin + up + front - up + side, begin + up + front - up + side + up, olc::GREEN, gfx);

		int cenX = floorf(gfx.ScreenWidth() / 2);
		int cenY = floorf(gfx.ScreenHeight() / 2);
	}

	void Move(Pnt3 moveV) override
	{
		moveBegin(moveV);
	}

	Pnt3 getPosition() override
	{
		return begin;
	}
	Pnt3 getSize() override
	{
		return size;
	}

};




bool RayCollision(Pnt3 begin, Pnt3 dir, Collider* coll, Pnt3& outPoint)
{
	return coll->RayCollision(begin, dir, outPoint);
}



std::vector<LightSoucrce> sceneLights;
std::vector<Sphere> shadowSpheres;

struct ShaderPacket
{
public:
	bool has = false;
	Pnt3 normal;
	Pnt3 pixelPosition;//0
	Pnt3 trianglePoints[3];//1
	Pnt2 uv;//2
	olc::Pixel color;//3
	olc::Sprite* spr;//4
	olc::Sprite* normalMap;//5
	Pnt3 center;
	int accuracy = 1;
	bool drawOnTop = false;
	ShaderPacket()
	{
		spr = nullptr;
		has = false;
	};
	
};

olc::Pixel mixColor(olc::Pixel c1, olc::Pixel c2,float coof)
{
	float m = 1 - coof;
	return olc::Pixel(std::min(255, (int)(c1.r * m + c2.r * coof)),
					  std::min(255, (int)(c1.g * m + c2.g * coof)),
		              std::min(255, (int)(c1.b * m + c2.b * coof)));
}

struct Shader
{
	public :
		bool request[7];//0-pixel, 1-triangle, 2-uv, 3-clr, 4-spr, 5-normalMap
		Shader() {};
		virtual olc::Pixel proceed(olc::Pixel in, ShaderPacket pack)
		{
			return olc::CYAN;
		};
	
};

struct FullColorShader:public Shader
{
	public:

		olc::Pixel clr;
		float alpha;
		FullColorShader()
		{
			clr = olc::WHITE;
			alpha = 1;
			
		};
		FullColorShader(olc::Pixel c,float a)
		{
			clr = c;
			alpha = a;
		}
		olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
		{
			return mixColor(clr,in,alpha);
		};
	
};


struct BorderShader :public Shader
{
public:

	BorderShader()
	{
		request[0] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		float nowDist = pack.center.x - pack.pixelPosition.x;
		if (nowDist == 0)
			nowDist = pack.center.y - pack.pixelPosition.y;
		if (nowDist == 0)
			nowDist = pack.center.z - pack.pixelPosition.z;
		nowDist = fabsf(nowDist);
		
		if ((int)floorf(nowDist)%4==0)
			return mixColor(in,olc::GREEN,0.5f);
		else return in;
	};

};

struct FillTexture: public Shader
{

	Pnt2 offset;
	Pnt2 scale;
	FillTexture()
	{
		request[2] = true;
		request[4] = true;
		scale = Pnt2(1, 1);
		offset = Pnt2(1, 1);
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		Pnt2 offsetOO = Pnt2(1.0f / offset.x, 1.0f / offset.y);
		Pnt2 newP = Pnt2(floorf(pack.uv.x / offsetOO.x) , floorf(pack.uv.y / offsetOO.y));
		Pnt2 ksp = Pnt2(pack.uv.x - offsetOO.x * newP.x, pack.uv.y - offsetOO.y * newP.y);
		return pack.spr->Sample(ksp.x*offset.x, ksp.y * offset.y);
	};
};


struct InverseShader :public Shader
{
public:
	InverseShader()
	{

	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		return olc::Pixel(255-in.r, 255 - in.g, 255 - in.b);
	};
	
};

struct GradientShader :public Shader
{
public:
	olc::Pixel begin;
	olc::Pixel end;
	int type;
	GradientShader(int t)
	{
		begin = olc::BLACK;
		end = olc::WHITE;
		type = t;
		request[2] = true;
		
	};
	GradientShader(int t, olc::Pixel b, olc::Pixel e)
	{
		begin = b;
		end = e;
		type = t;
		request[2] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{

		float k;
		float u = pack.uv.x;
		float v = pack.uv.y;
		if(type == 0)
			k = u;
		else if (type == 1)
			k = v;
		else if (type == 2)
			k = (u + v) / 2;
		
		return olc::Pixel(
			begin.r + (end.r - begin.r) * k, 
			begin.g + (end.g - begin.g) * k,
			begin.b + (end.b - begin.b) * k);
	};

};

struct SurfaceLightShader :public Shader
{
public:
	float minLight = 0.0f;
	bool center;
	SurfaceLightShader()
	{
		center = false;
		request[1] = true;
	};
	SurfaceLightShader(bool cen)
	{
		center = cen;
		request[1] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		//std::cout << pack.originalPoints[0].x;
		float dl = minLight;
		if (sceneLights.size() > 0)
		{
			Pnt3 cen, toLight;
			for (int lig = 0; lig < sceneLights.size(); lig++)
			{

				if (sceneLights[lig].type == 0)
					toLight = NormaliseVec(sceneLights[lig].position);
				else
				{
					Pnt3 p1, p2, p3;
					p1 = pack.trianglePoints[0];
					if (center)
					{
						p2 = pack.trianglePoints[1];
						p3 = pack.trianglePoints[2];
						cen = Pnt3((p1.x + p2.x + p3.x) / 3.0f, (p1.y + p2.y + p3.y) / 3.0f, (p1.z + p2.z + p3.z) / 3.0f);
					}
					else
					{
						cen = p1;
					}

	
					toLight = sceneLights[lig].position - cen;
					toLight = NormaliseVec(toLight);

				}

				float newD = DotProduct(pack.normal, toLight);
			
				if (newD > dl)
					dl = newD;

				if (dl < minLight)dl = minLight;
			}
		}
		else
		{
			dl = minLight;
		}
		
		return olc::Pixel(std::min(255.0f, in.r * dl), std::min(255.0f, in.g * dl), std::min(255.0f, in.b * dl));
	};

};

struct DepthShader :public Shader
{
	int coord;
	float depth;
public:
	DepthShader()
	{
		coord = 2;
		depth = 40.0f;
		request[0] = true;
	};
	DepthShader(int tp, float dep)
	{
		coord = tp;
		depth = dep;
		request[0] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
	
		if (coord == 2)
		{
			float k = depth / pack.pixelPosition.z;
			int  clr = (int)(255 * k);
			clr = clr % 255;
			return olc::Pixel(clr, clr, clr);
		}
		if (coord == 0)
		{
			float k = depth / pack.pixelPosition.x;
			int  clr = (int)(255 * k);
			clr = clr % 255;
			return olc::Pixel(clr, clr, clr);
		}
		if (coord == 1)
		{
			float k = depth / pack.pixelPosition.y;
			int  clr = (int)(255 * k);
			clr = clr % 255;
			return olc::Pixel(clr, clr, clr);
		}
			return olc::RED;
	};

};

struct PerPixelLightShader :public Shader
{
public:
	float minLigth = 0.0f;
	PerPixelLightShader()
	{
		request[0] = true;
		request[2] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		float dl = minLigth;
		if (sceneLights.size() > 0)
		{
			Pnt3 toLight;
			for (int lig = 0; lig < sceneLights.size(); lig++)
			{

				if (sceneLights[lig].type == 0)
					toLight = NormaliseVec(sceneLights[lig].position);
				else
				{
					toLight = sceneLights[lig].position - pack.pixelPosition;
					toLight = NormaliseVec(toLight);
				}


				float newD = DotProduct(pack.normal, toLight);
				newD *= sceneLights[lig].power;

				if (newD > dl)
					dl = newD;

				if (dl < minLigth)
					dl = minLigth;
				
			}
		}
		else
		{
			dl = minLigth;
		}

		return olc::Pixel(std::min(255.0f, in.r * dl), std::min(255.0f, in.g * dl), std::min(255.0f, in.b * dl));
	};

};



struct SphereShadowShader: public Shader
{
public:
	float darkness = 0.0f;
	bool black = true;
	SphereShadowShader()
	{
		black = true;
		request[0] = true;
	};
	SphereShadowShader(float dark)
	{
		darkness = dark;
		black = false;
		request[0] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{
		bool hasLight = true;
		for (int i = 0; i < shadowSpheres.size(); i++)
		{
			if (RaySphereCollision(pack.pixelPosition, NormaliseVec(sceneLights[0].position - pack.pixelPosition), shadowSpheres[0]))
			{
				hasLight = false;
				break;
			}
		}

		if (hasLight)return in;
		else
		{
			if (black)
				return olc::BLACK;
			else return olc::Pixel(in.r*darkness, in.g * darkness, in.b * darkness);
		}
	};
};


Pnt3 reflect(Pnt3 vec, Pnt3 norm)
{
	//        r = v - norm * 2 * dp(v,norm)
	return sub3d(vec, mul3d(norm, 2.0f * DotProduct(vec, norm)));
}

struct PlasticLightShader :public Shader
{
public:

	float minLight = 0.0f; 
	int shiness = 10;

	PlasticLightShader()
	{
		request[0] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{

		float last = 0.0f;
		
		olc::Pixel out = in;
			if (sceneLights.size() > 0)
			{
				for (int lig = 0; lig < sceneLights.size(); lig++)
				{
					
					//normal in pack
					Pnt3 light;
					if (sceneLights[lig].type == 0)
					{
						light = NormaliseVec(sceneLights[lig].position);
					}
					else
					{
						light = NormaliseVec(sub3d(sceneLights[lig].position, pack.pixelPosition));
					}
					
					Pnt3 view = NormaliseVec(sub3d(camPos, pack.pixelPosition));
					Pnt3 ref = NormaliseVec(mul3d(reflect(light, pack.normal), -1.0f));

					out = in;
					

					float lightCoof = std::max(DotProduct(pack.normal, light), 0.0f);
					
					if (lightCoof < minLight)
						lightCoof = minLight;

					out.r *= lightCoof;
					out.g *= lightCoof;
					out.b *= lightCoof;
					
					

					float shine = std::max(DotProduct(ref, view), 0.0f);
					if (shine > 0.45f)
					{
						float shineCoof = shine;

						for (int sh = 0; sh < shiness; sh++)
							shineCoof *= shine;

						if (shineCoof > last)
						{
							last = shineCoof;
						}
					}

					
				}
			}
			else
			{
				return olc::Pixel(in.r*minLight, in.g * minLight, in.b * minLight);
			} 
			return mixColor(out, olc::WHITE,last);
			/*olc::Pixel(std::min(255, (int)floorf(out.r * (1 - last) + lightClr.r * last)), 
						  std::min(255, (int)floorf(out.g * (1 - last) + lightClr.g * last)),
						  std::min(255, (int)floorf(out.b * (1 - last) + lightClr.b * last)));*/
	}
		
};

struct SmoothPlasticLightShader :public Shader
{
public:

	float minLight = 0.0f;
	int shiness = 10;

	SmoothPlasticLightShader()
	{
		request[0] = true;
	};
	olc::Pixel proceed(olc::Pixel in, ShaderPacket pack) override
	{

		float last = 0.0f;

		olc::Pixel out = in;
		if (sceneLights.size() > 0)
		{
			for (int lig = 0; lig < sceneLights.size(); lig++)
			{

				//normal in pack
				Pnt3 normal = NormaliseVec(pack.pixelPosition - pack.center);
				if(DotProduct(normal,pack.normal)<=0)
					normal=pack.normal;
				
				Pnt3 light;
				if (sceneLights[lig].type == 0)
				{
					light = NormaliseVec(sceneLights[lig].position);
				}
				else
				{
					light = NormaliseVec(sub3d(sceneLights[lig].position, pack.pixelPosition));
				}
				Pnt3 view = NormaliseVec(sub3d(camPos, pack.pixelPosition));
				Pnt3 ref = NormaliseVec(mul3d(reflect(light, normal), -1.0f));

				out = in;


				float lightCoof = std::max(DotProduct(normal, light), 0.0f);

				if (lightCoof < minLight)
					lightCoof = minLight;

				out.r *= lightCoof;
				out.g *= lightCoof;
				out.b *= lightCoof;



				float shine = std::max(DotProduct(ref, view), 0.0f);
				if (shine > 0.95f)
				{
					float shineCoof = shine;

					for (int sh = 0; sh < shiness; sh++)
						shineCoof *= shine;

					if (shineCoof > last)
					{
						last = shineCoof;
					}
				}


			}
		}
		else
		{
			return olc::Pixel(in.r * minLight, in.g * minLight, in.b * minLight);
		}
		return mixColor(out, olc::WHITE, last);
		/*olc::Pixel(std::min(255, (int)floorf(out.r * (1 - last) + lightClr.r * last)),
					  std::min(255, (int)floorf(out.g * (1 - last) + lightClr.g * last)),
					  std::min(255, (int)floorf(out.b * (1 - last) + lightClr.b * last)));*/
	}

};





struct Mesh;


struct Triangle//3d triangle
{
	Pnt3 p[3];//points , coords
	Pnt3 op[3];//original points , coords
	Pnt2 tp[3];//texture points, UV coords
	float light = 1.0f;
	Mesh* parentMesh;
	std::vector<Shader*> shader;
	Pnt3 n;
	Pnt3 c;
	int shaderAccuracy = 1;
	bool drawOnTop = false;

	Triangle()
	{
		p[0] = Pnt3(); p[1] = Pnt3(); p[2] = Pnt3();
	}
	Triangle(Pnt3 p1, Pnt3 p2, Pnt3 p3)
	{
		p[0] = p1; p[1] = p2; p[2] = p3;
	}
	Triangle(Pnt3 p1, Pnt3 p2, Pnt3 p3, Pnt2 tp1, Pnt2 tp2, Pnt2 tp3)
	{
		p[0] = p1; p[1] = p2; p[2] = p3;
		op[0] = p1; op[1] = p2; op[2] = p3;
		tp[0] = tp1; tp[1] = tp2; tp[2] = tp3;
	}

};






struct Mesh
{
	std::vector<Pnt3> v;//vertexes of mesh
	std::vector<int*> trisInd;

	std::vector<Pnt2> texs;
	std::vector<int*> texsInd;

	//std::vector<Triangle> tris;// faces in triangles
	olc::Sprite *spr;//sprite of texture
	olc::Sprite* normalMap;
	olc::Pixel color;

	int shaderAccuracy = 1;

	bool hasTexture = false;
	std::string fileNames[2];
	std::vector<Shader*> shader;
	
	Mesh copy()//returns same mesh
	{
		Mesh nmo;
		nmo.v = this->v;
		nmo.trisInd = this->trisInd;
		nmo.texs = this->texs;
		nmo.texsInd = this->texsInd;
		//nmo.tris = this->tris;
		nmo.spr = this->spr;
		nmo.fileNames[0] = this->fileNames[0];
		nmo.fileNames[1] = this->fileNames[1];
		nmo.hasTexture = this->hasTexture;
		nmo.shader = this->shader;
		nmo.normalMap = this->normalMap;
		nmo.color = this->color;
		return nmo;
	}

	void SetFiles(std::string meshFilename, std::string textureFilename)
	{
		fileNames[0] = meshFilename;
		fileNames[1] = textureFilename;
	}

	bool LoadFromObjectFile(std::string sFilename, bool bHasTexture = false)//load mesh from .obj file
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;
		hasTexture = bHasTexture;
		// Local cache of verts

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					Pnt2 v;
					s >> junk >> junk >> v.x >> v.y;
					// A little hack for the spyro texture
					//v.u = 1.0f - v.u;
					v.y = 1.0f - v.y;
					texs.push_back(v);
				}
				else
				{
					Pnt3 nv;
					s >> junk >> nv.x >> nv.y >> nv.z;
					v.push_back(nv);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];

					trisInd.push_back(new int[3]{f[0]-1, f[1]-1, f[2]-1});

					//tris.push_back(Triangle(v[f[0] - 1], v[f[1] - 1], v[f[2] - 1]));
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					std::string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();

					trisInd.push_back(new int[3]{ stoi(tokens[0]) - 1, stoi(tokens[2]) - 1, stoi(tokens[4]) - 1 });
					texsInd.push_back(new int[3]{ stoi(tokens[1]) - 1, stoi(tokens[3]) - 1, stoi(tokens[5]) - 1 });


					//tris.push_back(Triangle(v[stoi(tokens[0]) - 1], v[stoi(tokens[2]) - 1], v[stoi(tokens[4]) - 1],
					//	texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1]));

				}

			}
		}
		return true;
	}

};

struct VertexShader
{
	virtual Pnt3 proceed(Pnt3 inV, Pnt3 PosOff)
	{
		return inV + Pnt3((float)(rand() % 2)/10);
	}
};

struct WaveVShader : public VertexShader
{
public :
	float amplitude = 1.6f;
	float freqTime = 3.09f;
	float freqXWave = 0.2f;
	float freqZWave = 0.4f;
	float timer = 0;
	float delta = 0;
	Pnt3 proceed(Pnt3 inV, Pnt3 PosOff) override
	{
		Pnt3 out = inV;
		timer += delta;
		if (timer > 1000)
			timer -= 1000;
			out.y += sinf(timer * freqTime + (out.x) * freqXWave + (out.z) * freqZWave) * amplitude;
		
		
		return out;
	}

};

struct WaveVSShader : public VertexShader
{
public:
	float amplitude = 1.6f;
	float freqTime = 3.09f;
	float freqXWave = 0.2f;
	float freqZWave = 0.4f;
	float timer = 0;
	float delta = 0;
	Pnt3 proceed(Pnt3 inV, Pnt3 PosOff) override
	{
		Pnt3 out = inV;
		timer += delta;
		if (timer > 1000)
			timer -= 1000;
		float xO = inV.x - PosOff.x;
		float zO = inV.z - PosOff.z;
		out.y += sinf(timer * freqTime + (xO*xO) * freqXWave + (zO * zO) * freqZWave) * amplitude;


		return out;
	}

};


struct DrawableMesh
{
	Mesh* mesh;

	Pnt3 pos;//position in world
	Pnt3 ang;//angle
	Pnt3 scale = Pnt3(1, 1, 1);
	Pnt3 rot;//mesh's center
	int rotOrder;

	std::vector<Shader*> shader;
	std::vector<VertexShader*> vertexSahder;
	olc::Pixel color; // color if has no texture
	bool useBaseShader = true;
	bool useBaseColor = true;
	int shaderAccuracy = 1;

	bool drawBackFaces = false;
	bool drawOnTop = false;

	DrawableMesh()
	{

	}
	~DrawableMesh()
	{

	}

	DrawableMesh copy()//returns same mesh
	{
		DrawableMesh nmo;
		nmo.mesh = this->mesh;
		
		nmo.pos = this->pos;
		nmo.ang = this->ang;
		nmo.scale = this->scale;
		nmo.rot = this->rot;
		nmo.rotOrder = this->rotOrder;

		nmo.useBaseShader = this->useBaseShader;
		nmo.useBaseColor = this->useBaseColor;

		nmo.vertexSahder = this->vertexSahder;
		nmo.shader = this->shader;
		nmo.color = this->color;
		nmo.shaderAccuracy = this->shaderAccuracy;
		return nmo;
	}



};







void drawMesh(DrawableMesh& m, std::vector<Triangle> &VdrawBuffer)
{
	
	for (int f = 0; f < m.mesh->trisInd.size(); f++)
	{
		
		Triangle out;
		out.c = m.rot + m.pos;

		std::vector<Pnt3> vers = m.mesh->v;
		for (int vs = 0; vs < m.vertexSahder.size(); vs++)
		{
			for (int ver = 0; ver < m.mesh->v.size(); ver++)
			{
				vers[ver] = m.vertexSahder[vs]->proceed(vers[ver], m.pos);
			}
			
		}
		

		//Pnt3 p1 = m.mesh->tris[f].p[0];
		//Pnt3 p2 = m.mesh->tris[f].p[1];
		//Pnt3 p3 = m.mesh->tris[f].p[2];

		Pnt3 p1 = vers[m.mesh->trisInd[f][0]];
		Pnt3 p2 = vers[m.mesh->trisInd[f][1]];
		Pnt3 p3 = vers[m.mesh->trisInd[f][2]];

		

		p1 = sum3d(p1, m.rot);
		p2 = sum3d(p2, m.rot);
		p3 = sum3d(p3, m.rot);


		p1.x *= m.scale.x;
		p1.y *= m.scale.y;
		p1.z *= m.scale.z;

		p2.x *= m.scale.x;
		p2.y *= m.scale.y;
		p2.z *= m.scale.z;

		p3.x *= m.scale.x;
		p3.y *= m.scale.y;
		p3.z *= m.scale.z;

		
		p1 = rotate3d(p1, m.ang, m.rotOrder);
		p2 = rotate3d(p2, m.ang, m.rotOrder);
		p3 = rotate3d(p3, m.ang, m.rotOrder);
		
		
		p1 = sum3d(p1, m.pos);
		p2 = sum3d(p2, m.pos);
		p3 = sum3d(p3, m.pos);

		Pnt3 normal, l1, l2;

		l1 = sub3d(p1, p3);
		l2 = sub3d(p2, p3);

		normal = CrossProduct(l1, l2);

		normal = NormaliseVec(normal);
		Pnt3 outNorm = normal;

		if (m.useBaseShader)
		{
			for (int i = 0; i < m.mesh->shader.size(); i++)
				if (m.mesh->shader[i]->request[0] || m.mesh->shader[i]->request[1])
				{
					out.op[0] = p1.copy();
					out.op[1] = p2.copy();
					out.op[2] = p3.copy();
					break;
				}
		}
		else
		{
			for (int i = 0; i < m.shader.size(); i++)
				if (m.shader[i]->request[0] || m.shader[i]->request[1])
				{
					out.op[0] = p1.copy();
					out.op[1] = p2.copy();
					out.op[2] = p3.copy();
					break;
				}
		}
		
		p1 = sub3d(p1, camPos);
		p2 = sub3d(p2, camPos);
		p3 = sub3d(p3, camPos);


		p1 = rotate3d(p1,camAng, 2);
		p2 = rotate3d(p2,camAng, 2);
		p3 = rotate3d(p3,camAng, 2);


		l1 = sub3d(p1, p3);
		l2 = sub3d(p2, p3);


		normal.x = l1.y*l2.z - l1.z*l2.y;

		normal.y = l1.z*l2.x - l1.x*l2.z;

		normal.z = l1.x*l2.y - l1.y*l2.x;

		normal = NormaliseVec(normal);

		float dc = DotProduct(normal, p1);


		if (dc <= 0.0f || m.drawBackFaces)
		if (p1.z > clipDistance || p2.z > clipDistance || p3.z > clipDistance)
		{

			std::vector<Triangle> outTris;

			const auto Clip1Out = [&](Pnt3 outP, Pnt3 inP1, Pnt3 inP2, int outTN, int in1TN, int in2TN)
			{
				float to1 = (-outP.z + clipDistance) / (inP1.z - outP.z);
				float to2 = (-outP.z + clipDistance) / (inP2.z - outP.z);

				Pnt3 newPnt1 = outP + (inP1 - outP) * to1;
				Pnt3 newPnt2 = outP + (inP2 - outP) * to2;

				Pnt2 newTPnt1 = m.mesh->texs[m.mesh->texsInd[f][outTN]] + (m.mesh->texs[m.mesh->texsInd[f][in1TN]] - m.mesh->texs[m.mesh->texsInd[f][outTN]]) * to1;
				Pnt2 newTPnt2 = m.mesh->texs[m.mesh->texsInd[f][outTN]] + (m.mesh->texs[m.mesh->texsInd[f][in2TN]] - m.mesh->texs[m.mesh->texsInd[f][outTN]]) * to2;

				outTris.push_back(
					Triangle(
						newPnt1, inP1, inP2,
						newTPnt1, m.mesh->texs[m.mesh->texsInd[f][in1TN]], m.mesh->texs[m.mesh->texsInd[f][in2TN]]));

				outTris[outTris.size() - 1].op[0] = out.op[outTN] + (out.op[in1TN] - out.op[outTN]) * to1;
				outTris[outTris.size() - 1].op[1] = out.op[in1TN];
				outTris[outTris.size() - 1].op[2] = out.op[in2TN];

				outTris.push_back(
					Triangle(
						newPnt2, newPnt1, inP2,
						newTPnt2, newTPnt1, m.mesh->texs[m.mesh->texsInd[f][in2TN]]));

				outTris[outTris.size() - 1].op[0] = out.op[outTN] + (out.op[in2TN] - out.op[outTN]) * to2;
				outTris[outTris.size() - 1].op[1] = out.op[outTN] + (out.op[in1TN] - out.op[outTN]) * to1;
				outTris[outTris.size() - 1].op[2] = out.op[in2TN];

			


			};

			const auto Clip2Out = [&](Pnt3 outP1, Pnt3 outP2, Pnt3 inP, int out1TN, int out2TN, int inTN)
			{
				float to1 = (-outP1.z + clipDistance) / (inP.z - outP1.z);
				float to2 = (-outP2.z + clipDistance) / (inP.z - outP2.z);

				Pnt3 newPnt1 = outP1 + (inP - outP1) * to1;
				Pnt3 newPnt2 = outP2 + (inP - outP2) * to2;

				Pnt2 newTPnt1 = m.mesh->texs[m.mesh->texsInd[f][out1TN]] + (m.mesh->texs[m.mesh->texsInd[f][inTN]] - m.mesh->texs[m.mesh->texsInd[f][out1TN]]) * to1;
				Pnt2 newTPnt2 = m.mesh->texs[m.mesh->texsInd[f][out2TN]] + (m.mesh->texs[m.mesh->texsInd[f][inTN]] - m.mesh->texs[m.mesh->texsInd[f][out2TN]]) * to2;

				outTris.push_back(
					Triangle(
						newPnt1, inP, newPnt2,
						newTPnt1, m.mesh->texs[m.mesh->texsInd[f][inTN]], newTPnt2));

				outTris[outTris.size() - 1].op[0] = out.op[out1TN] + (out.op[inTN] - out.op[out1TN]) * to1;
				outTris[outTris.size() - 1].op[1] = out.op[inTN];
				outTris[outTris.size() - 1].op[2] = out.op[out2TN] + (out.op[inTN] - out.op[out2TN]) * to2;

			

			};


			if (p1.z > clipDistance&& p2.z > clipDistance&& p3.z > clipDistance)
			{
				outTris.push_back(
					Triangle(
						p1, p2, p3,
						m.mesh->texs[m.mesh->texsInd[f][0]], m.mesh->texs[m.mesh->texsInd[f][1]], m.mesh->texs[m.mesh->texsInd[f][2]]));

				outTris[outTris.size() - 1].op[0] = out.op[0];
				outTris[outTris.size() - 1].op[1] = out.op[1];
				outTris[outTris.size() - 1].op[2] = out.op[2];
			}
			else
			{
				if (p1.z < clipDistance)
				{

					if (p2.z < clipDistance)
					{
						Clip2Out(p1, p2, p3, 0, 1, 2);
					}
					else if (p3.z < clipDistance)
					{
						Clip2Out(p1, p3, p2, 0, 2, 1);
					}
					else
					{
						Clip1Out(p1, p2, p3, 0, 1, 2);
					}


				}else if (p2.z < clipDistance)
				{

					if (p3.z < clipDistance)
					{
						Clip2Out(p2, p3, p1, 1, 2, 0);
					}
					else
					{
						Clip1Out(p2, p1, p3, 1, 0, 2);
					}
				}
				else if (p3.z < clipDistance)
				{
					Clip1Out(p3, p1, p2, 2, 0, 1);
				}
			}
				

			for(int i=0;i< outTris.size();i++)
			{

				//out.p[0] = outTris[i].p[0];
				//out.p[1] = outTris[i].p[1];
				//out.p[2] = outTris[i].p[2];

				//out.tp[0] = outTris[i].tp[0];
				//out.tp[1] = outTris[i].tp[1];
				//out.tp[2] = outTris[i].tp[2];

				//out.tp[0] = m.mesh->tris[f].tp[0];
				//out.tp[1] = m.mesh->tris[f].tp[1];
				//out.tp[2] = m.mesh->tris[f].tp[2];
				

				outTris[i].parentMesh = m.mesh;
				outTris[i].c = out.c;

				if (m.useBaseShader)
				{
					outTris[i].shader = m.mesh->shader;
					outTris[i].shaderAccuracy = m.mesh->shaderAccuracy;
				}
				else
				{
					outTris[i].shader = m.shader;
					outTris[i].shaderAccuracy = m.shaderAccuracy;
				}

				outTris[i].n = outNorm.copy();
				outTris[i].drawOnTop = m.drawOnTop;

				VdrawBuffer.push_back(outTris[i]);

			}
		}
		
	}
}






void drawTextureTriangle
(
	int x1, int y1,float z1,
	int x2, int y2,float z2,
	int x3, int y3,float z3,
	float u1, float v1,
	float u2, float v2,
	float u3, float v3,
	olc::Sprite *tex, olc::PixelGameEngine p,std::vector<Shader*> sd, ShaderPacket pack
)
{

	bool need0 = false;
	bool need1 = false;
	for (int i = 0; i < sd.size(); i++)
		if (sd[i]->request[0])
		{
			need0 = true;
			break;
		}

	for (int i = 0; i < sd.size(); i++)
		if (sd[i]->request[1])
		{
			need1 = true;
			break;
		}

	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(z1, z2);
		std::swap(u1, u2);
		std::swap(v1, v2);
		if(need0)
			std::swap(pack.trianglePoints[0], pack.trianglePoints[1]);

	}
	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(z1, z3);
		std::swap(u1, u3);
		std::swap(v1, v3);
		if (need0)
			std::swap(pack.trianglePoints[0], pack.trianglePoints[2]);
	}
	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(z2, z3);
		std::swap(u2, u3);
		std::swap(v2, v3);
		if (need0)
			std::swap(pack.trianglePoints[1], pack.trianglePoints[2]);
	}

	int dx1 = x2 - x1, 
		dy1 = y2 - y1, 
		dx2 = x3 - x1, 
		dy2 = y3 - y1;

	float dz1 = z2 - z1,
		  du1 = u2 - u1, 
		  dv1 = v2 - v1,
	      dz2 = z3 - z1,
		  du2 = u3 - u1, 
		  dv2 = v3 - v1,
		  ka = 0, 
		  kb = 0, 
		  kz1=0, kz2 = 0,
		  ku1 = 0, 
		  ku2 = 0, 
		  kv1 = 0, 
		  kv2 = 0,
		  texU, texV;


	if (dy1)
	{
		ka =  dx1 / (float)abs(dy1);
		kz1 = dz1 / (float)abs(dy1);
		ku1 = du1 / (float)abs(dy1);
		kv1 = dv1 / (float)abs(dy1);
	}
	if (dy2)
	{
		kb =  dx2 / (float)abs(dy2);
		kz2 = dz2 / (float)abs(dy2);
		ku2 = du2 / (float)abs(dy2);
		kv2 = dv2 / (float)abs(dy2);
	}

	int toDrawY1 = y1;
	int toDrawY2 = y2;

	olc::Pixel prevPix = olc::MAGENTA;
	int pixCounter = -1;

		if (dy1)
		{
			if (toDrawY1 < 0)toDrawY1 = 0;
			if (toDrawY2 > p.ScreenHeight())toDrawY2 = p.ScreenHeight() - 1;

			for (int drawY = toDrawY1; drawY <= toDrawY2; drawY++)
			{

				int SX = x1 + (float)(drawY -   y1) * ka;
				int EX = x1 + (float)(drawY -   y1) * kb;

				float SPX,SPY,SPZ; 
				float EPX,EPY,EPZ; 

				if (need0)
				{
					SPX = pack.trianglePoints[0].x + (float)(drawY - y1) * ((pack.trianglePoints[1].x - pack.trianglePoints[0].x) / (float)dy1);
					EPX = pack.trianglePoints[0].x + (float)(drawY - y1) * ((pack.trianglePoints[2].x - pack.trianglePoints[0].x) / (float)dy2);
	
					SPY = pack.trianglePoints[0].y + (float)(drawY - y1) * ((pack.trianglePoints[1].y - pack.trianglePoints[0].y) / (float)dy1);
					EPY = pack.trianglePoints[0].y + (float)(drawY - y1) * ((pack.trianglePoints[2].y - pack.trianglePoints[0].y) / (float)dy2);

					SPZ = pack.trianglePoints[0].z + (float)(drawY - y1) * ((pack.trianglePoints[1].z - pack.trianglePoints[0].z) / (float)dy1);
					EPZ = pack.trianglePoints[0].z + (float)(drawY - y1) * ((pack.trianglePoints[2].z - pack.trianglePoints[0].z) / (float)dy2);
				}


				float SU = u1 + (float)(drawY - y1) * ku1;
				float SV = v1 + (float)(drawY - y1) * kv1;

				float EU = u1 + (float)(drawY - y1) * ku2;
				float EV = v1 + (float)(drawY - y1) * kv2;

				float SZ = z1 + (float)(drawY - y1) * kz1;
				float EZ = z1 + (float)(drawY - y1) * kz2;


				if (SX > EX)
				{
					std::swap(SX, EX);
					std::swap(SZ, EZ);
					std::swap(SU, EU);
					std::swap(SV, EV);
					if (need0)
						{
						std::swap(SPX, EPX);
						std::swap(SPY, EPY);
						std::swap(SPZ, EPZ);
						}
					
				}
				texU = SU; texV = SV;
				

				float tSX = SX;
				float tEX = EX;

				float t = 0;

				if (SX < 0)
				{
					SX = 0;
					t = abs(tSX)*(1.0f / ((float)(tEX - tSX)));
				}
				if (EX > p.ScreenWidth())
				{
					EX = p.ScreenWidth() - 1;
				}

				

				for (int drawX = SX; drawX < EX; drawX++)
				{
						float w = (SZ + (EZ - SZ) * (t));
	
						texU = SU + (EU - SU) * t;
						texV = SV + (EV - SV) * t;

						float invW = 1.0f / w;

						texU *= invW;
						texV *= invW;
	

					

						if(drawX + drawY * p.ScreenWidth() >=0 && drawX + drawY * p.ScreenWidth()< p.ScreenWidth()* p.ScreenHeight()-1)
						if ((zBuffer[drawX + drawY * p.ScreenWidth()] > invW && invW > clipDistance && invW < maxDistance) || pack.drawOnTop)
						{
							if(!pack.drawOnTop)
								zBuffer[drawX + drawY * p.ScreenWidth()] = invW;
							else zBuffer[drawX + drawY * p.ScreenWidth()] = 0;

							olc::Pixel finalPix;
							if(tex!=nullptr)
								finalPix = tex->Sample(texU, texV);
							else finalPix = pack.color;

							pack.uv.x = texU;
							pack.uv.y = texV;

							if (need0)
							{
								pack.pixelPosition.x = (SPX + (EPX - SPX) * (t));
								pack.pixelPosition.y = (SPY + (EPY - SPY) * (t));
								pack.pixelPosition.z = (SPZ + (EPZ - SPZ) * (t));

							    pack.pixelPosition.x = pack.pixelPosition.x * invW;
								pack.pixelPosition.y = pack.pixelPosition.y * invW;
								pack.pixelPosition.z = pack.pixelPosition.z * invW;
							}

							
							pixCounter += 1;
							if (pixCounter >= pack.accuracy || pixCounter == 0)
							{
								if (sd.size() > 0)
								{
									for (int i = 0; i < sd.size(); i++)
									{
										finalPix = sd[i]->proceed(finalPix, pack);
									}
								}
								prevPix = finalPix;
								pixCounter = 0;
							}
							else
							{
								finalPix = prevPix;
							}
							p.Draw(drawX, drawY, finalPix);
							
						}

					t += 1.0f / ((float)(tEX - tSX));
				}
			

			}
		}

	dx1 = x3 - x2; dy1 = y3 - y2;

	du1 = u3 - u2; dv1 = v3 - v2;

	dz1 = z3 - z2;

	if (dy1)
	{
		ka =  dx1 / (float)abs(dy1);
		kz1 = dz1 / (float)abs(dy1);
		ku1 = du1 / (float)abs(dy1);
		kv1 = dv1 / (float)abs(dy1);

	}
	if (dy2)
	{
		kb = dx2 / (float)abs(dy2);
	}


	toDrawY1 = y2;
	toDrawY2 = y3;
	
		if (toDrawY1 < 0)toDrawY1 = 0;
		if (toDrawY2 > p.ScreenHeight())toDrawY2 = p.ScreenHeight() - 1;
		float prevT = 0;
		for (int drawY = toDrawY1; drawY <= toDrawY2; drawY++)
		{
			int SX = x2 + (float)(drawY - y2) * ka;
			int EX = x1 + (float)(drawY - y1) * kb;

			float SPX, SPY, SPZ;
			float EPX, EPY, EPZ;

			if (need0)
			{
				SPX = pack.trianglePoints[1].x + (float)(drawY - y2) * ((pack.trianglePoints[2].x - pack.trianglePoints[1].x) / (float)dy1);
				EPX = pack.trianglePoints[0].x + (float)(drawY - y1) * ((pack.trianglePoints[2].x - pack.trianglePoints[0].x) / (float)dy2);

				SPY = pack.trianglePoints[1].y + (float)(drawY - y2) * ((pack.trianglePoints[2].y - pack.trianglePoints[1].y) / (float)dy1);
				EPY = pack.trianglePoints[0].y + (float)(drawY - y1) * ((pack.trianglePoints[2].y - pack.trianglePoints[0].y) / (float)dy2);

				SPZ = pack.trianglePoints[1].z + (float)(drawY - y2) * ((pack.trianglePoints[2].z - pack.trianglePoints[1].z) / (float)dy1);
				EPZ = pack.trianglePoints[0].z + (float)(drawY - y1) * ((pack.trianglePoints[2].z - pack.trianglePoints[0].z) / (float)dy2);
			}


			float SU = u2 + (float)(drawY - y2) * ku1;
			float EU = u1 + (float)(drawY - y1) * ku2;

			float SV = v2 + (float)(drawY - y2) * kv1;
			float EV = v1 + (float)(drawY - y1) * kv2;

			float SZ = z2 + (float)(drawY - y2) * kz1;
			float EZ = z1 + (float)(drawY - y1) * kz2;

			float SDZ = z2;
			float EDZ = z2 + (z3 - z2) * ((float)(drawY - toDrawY1) / (toDrawY2 - toDrawY1));

			if (SX > EX)
			{
				std::swap(SX, EX);
				std::swap(SZ, EZ);
				std::swap(SU, EU);
				std::swap(SV, EV);	
				if (need0)
				{
					std::swap(SPX, EPX);
					std::swap(SPY, EPY);
					std::swap(SPZ, EPZ);
				}
			}
			texU = SU; texV = SV;
			float t = 0;

			float tSX = SX;
			float tEX = EX;

			if (SX < 0)
			{
				SX = 0;
				t = abs(tSX) * (1.0f / ((float)(tEX - tSX)));
			}
			if (EX > p.ScreenWidth())
			{
				EX = p.ScreenWidth() - 1;
				//t = abs(tEX - EX) * (1.0f / ((float)(tEX - tSX)));
			}

			prevT = t;

			for (int drawX = SX; drawX < EX; drawX++)
			{
				float w = (SZ + (EZ - SZ) * (t));

				texU = SU + (EU - SU) * t;
				texV = SV + (EV - SV) * t;

				float invW = 1.0f / w;

				texU *= invW;
				texV *= invW;

				
	

				if (drawX + drawY * p.ScreenWidth() >= 0 && drawX + drawY * p.ScreenWidth() < p.ScreenWidth() * p.ScreenHeight() - 1)
				if ((zBuffer[drawX + drawY * p.ScreenWidth()] > invW&& invW > clipDistance&& invW < maxDistance) || pack.drawOnTop)
				{
					if (!pack.drawOnTop)
						zBuffer[drawX + drawY * p.ScreenWidth()] = invW;
					else zBuffer[drawX + drawY * p.ScreenWidth()] = 0;

					olc::Pixel finalPix;
					if (tex != nullptr)
						finalPix = tex->Sample(texU, texV);
					else finalPix = pack.color;
					
					pack.uv.x = texU;
					pack.uv.y = texV;

					if (need0)
					{

						pack.pixelPosition.x = (SPX + (EPX - SPX) * (t));
						pack.pixelPosition.y = (SPY + (EPY - SPY) * (t));
						pack.pixelPosition.z = (SPZ + (EPZ - SPZ) * (t));

						pack.pixelPosition.x = pack.pixelPosition.x * invW;
						pack.pixelPosition.y = pack.pixelPosition.y * invW;
						pack.pixelPosition.z = pack.pixelPosition.z * invW;

					}
					pixCounter += 1;
					if (pixCounter >= pack.accuracy || pixCounter == 0)
					{
						if (sd.size() > 0)
						{
							for (int i = 0; i < sd.size(); i++)
							{
								finalPix = sd[i]->proceed(finalPix, pack);
							}
						}
						prevPix = finalPix;
						pixCounter = 0;
					}
					else
					{
						finalPix = prevPix;
					}
					p.Draw(drawX, drawY, finalPix);
					
				}
				t += 1.0f / ((float)(tEX - tSX));
			}

		}
	



}


void drawBuffer(std::vector<Triangle> &VdrawBuffer, olc::PixelGameEngine p)
{
	/*sort(VdrawBuffer.begin(), VdrawBuffer.end(), [](Triangle &t1, Triangle &t2) {

		float zd1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
		float zd2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
		return zd1 > zd2;
	});*/
	

	for (int f = 0; f < VdrawBuffer.size(); f++) {
		Triangle td = VdrawBuffer[f];

		bool needP = false;
		for (int i = 0; i < td.shader.size(); i++)
			if (td.shader[i]->request[0] || td.shader[i]->request[1])
			{
				needP = true;
				break;
			}

		for (int g = 0; g < 3; g++)
		{
			td.p[g].x = convert(td.p[g]).x + floorf((float)p.ScreenWidth() / 2);
			td.p[g].y = convert(td.p[g]).y + floorf((float)p.ScreenHeight() / 2);
			
			td.tp[g].x = td.tp[g].x / td.p[g].z;
			td.tp[g].y = td.tp[g].y / td.p[g].z;

			if(needP)
			{
				
				td.op[g].x = td.op[g].x / td.p[g].z;
				td.op[g].y = td.op[g].y / td.p[g].z;
				td.op[g].z = td.op[g].z / td.p[g].z;
			}

			td.p[g].z = 1.0f / td.p[g].z;
		}

		ShaderPacket pack = ShaderPacket();
		pack.normal = td.n;
		pack.color = td.parentMesh->color;
		pack.center = td.c;
		pack.spr = td.parentMesh->spr;
		pack.accuracy = td.shaderAccuracy;
		pack.drawOnTop = td.drawOnTop;

		if (td.shader.size()>0)
		{
			pack.has = true;
			if (needP)
			{
				pack.trianglePoints[0] = td.op[0].copy();
				pack.trianglePoints[1] = td.op[1].copy();
				pack.trianglePoints[2] = td.op[2].copy();
			}
			pack.normalMap = td.parentMesh->normalMap;
		}
		else pack.has = false;
		
		
		

		drawTextureTriangle(
			td.p[0].x, td.p[0].y, td.p[0].z, // 1point
			td.p[1].x, td.p[1].y, td.p[1].z, // 2point
			td.p[2].x, td.p[2].y, td.p[2].z, // 3point
			td.tp[0].x, td.tp[0].y,// 1 UV
			td.tp[1].x, td.tp[1].y,// 2 UV
			td.tp[2].x, td.tp[2].y // 3 UV
			,td.parentMesh->spr, p, td.shader, pack);

		/*p.DrawTriangle(
			td.p[0].x, td.p[0].y, // 1point
			td.p[1].x, td.p[1].y, // 2point
			td.p[2].x, td.p[2].y,
			olc::WHITE);*/

	}

	

		
}




//END OF RASTERIZER-----------------------------------------BEGIN OF ANIMATION--------------------------------------------------------
/*
struct Bone
{
	Mesh mesh;
	Pnt3 pos;
	Pnt3 ang;
	Pnt3 scale;
	Pnt3 globPos;
	Pnt3 globAng;
	std::vector<Pnt3> outPoint;
	Bone* parent = nullptr;
	Bone* child = nullptr;
	int id, parId, outPointId;

	Bone()
	{

	}

	Bone(Mesh m)
	{
		mesh = m.copy();
		parent = nullptr;
		child = nullptr;
		parId = -1;
	}
};

//               child    parent
void AttachBone(Bone& b1, Bone& b2, int outPoint)
{
	b1.parent = &b2;
	b1.outPointId = outPoint;
	b2.child = &b1;
	b1.parId = b2.id;
}



struct Body
{
	std::vector<Bone> bones;
	Body()
	{

	}

};

void AddBone(Body& body, Bone& bone)
{
	body.bones.push_back(bone);
	body.bones[body.bones.size() - 1].id = body.bones.size() - 1;
}


void DrawBody(Body& b, std::vector<Triangle> &buf)
{
	bool allGotit = false;
	std::vector<bool> gotit;
	gotit.resize(b.bones.size());
	
	while (!allGotit)
	{
		allGotit = true;
		for (int i = 0; i < b.bones.size(); i++)
		{
			if (gotit[i] == false)
			{
				if (b.bones[i].parent != nullptr)
				{
					if (gotit[b.bones[i].parId] == true)
					{
						Pnt3 totalPos = sum3d(b.bones[i].parent->outPoint[b.bones[i].outPointId], b.bones[i].pos);
						Pnt3 rotatePnt = rotate3d(totalPos, b.bones[i].parent->globAng, b.bones[i].parent->mesh.rotOrder);

						b.bones[i].globPos = rotatePnt.copy();
						b.bones[i].globPos = sum3d(b.bones[i].globPos, b.bones[i].parent->globPos);

						b.bones[i].globAng = sum3d(b.bones[i].ang, b.bones[i].parent->globAng);

						b.bones[i].mesh.pos = b.bones[i].globPos;
						b.bones[i].mesh.ang = b.bones[i].globAng;
						drawMesh(b.bones[i].mesh, buf);
						gotit[i] = true;
					}
					else
					{
						allGotit = false;
					}
						
				}
				else
				{

					b.bones[i].globPos = b.bones[i].pos;
					b.bones[i].globAng = b.bones[i].ang;

					b.bones[i].mesh.pos = b.bones[i].globPos;
					b.bones[i].mesh.ang = b.bones[i].globAng;
					drawMesh(b.bones[i].mesh, buf);
					gotit[i] = true;
				}
			}
			


		}

	}
}

void saveBody(Body b, std::string wha)
{
	std::ofstream fout;
	fout.open(wha);
	if (fout.is_open())
	{
		for (int i = 0; i < b.bones.size(); i++)
		{
			fout << "N " + b.bones[i].mesh.fileNames[0] + " " + b.bones[i].mesh.fileNames[1] + " " + //mesh and texture file names
					std::to_string(b.bones[i].outPoint.size()) + " ";//amount of attach points
			for (int k = 0; k < b.bones[i].outPoint.size(); k++)
			{
				fout << std::to_string(b.bones[i].outPoint[k].x) + " " + std::to_string(b.bones[i].outPoint[k].y) + " " + std::to_string(b.bones[i].outPoint[k].z) + " "; //attach points positions
			}
			fout <<	std::to_string(b.bones[i].mesh.rot.x) + " " + std::to_string(b.bones[i].mesh.rot.y) + " " + std::to_string(b.bones[i].mesh.rot.z) + " " + //meshs center points
					std::to_string(b.bones[i].mesh.rotOrder) + " " +
					std::to_string(b.bones[i].outPointId) + " " + std::to_string(b.bones[i].parId) + "\n";// parents and childs ids in body bones vector
		}//         attach point number where i attach

	}
	else
	{
		std::cout << "error with saving file";
	}
}

void loadBody(Body &in, std::string wha)
{
	std::ifstream fin;
	fin.open(wha);

	int counterB;
	if (fin.is_open())
	{

		std::string type;

		counterB = -1;

		std::string meshName;
		std::string texName;
		int outCnt;
		std::vector<Pnt3> outPts;
		float rotx, roty, rotz;
		int MrotOrder;
		int outId;

		int boneParId;

		in.bones.clear();

		while (!fin.eof())
		{
			type = " ";
			fin >> type;

			if (type == "N")
			{
				outPts.clear();
				counterB = counterB + 1;

				Bone newBone;
				fin >> meshName;
				fin >> texName;

				//std::cout << "got mesh: " + meshName << std::endl;
				//std::cout << "got tex: " + texName << std::endl;

				fin >> outCnt;
				for (int i = 0; i < outCnt; i++)
				{
					Pnt3 newOutPt;
					fin >> newOutPt.x;
					fin >> newOutPt.y;
					fin >> newOutPt.z;
					outPts.push_back(newOutPt);
				}
				

				//std::cout << "got outP: " + std::to_string(outx) + ";"  + std::to_string(outy) + ";" + std::to_string(outz)<< std::endl;

				fin >> rotx;
				fin >> roty;
				fin >> rotz;

				fin >> MrotOrder;
				//std::cout << "got rotP: " + std::to_string(rotx) + ";" + std::to_string(roty) + ";" + std::to_string(rotz) << std::endl;

				
				fin >> outId;
				

				fin >> boneParId;

				//std::cout << "got par: " + std::to_string(boneParId) << std::endl;

				olc::Sprite* spr;
				spr = new olc::Sprite(texName);

				Mesh mesh;
				mesh.LoadFromObjectFile(meshName, true);
				mesh.spr = spr;
				mesh.rot = Pnt3(rotx,roty,rotz);
				mesh.rotOrder = MrotOrder;
				mesh.SetFiles(meshName, texName);
				newBone = Bone(mesh);
				
				AddBone(in, newBone);

				for(int i=0;i<outCnt;i++)
					in.bones[counterB].outPoint.push_back(outPts[i].copy());

				in.bones[counterB].outPointId = outId;
				in.bones[counterB].id = counterB;
				in.bones[counterB].parId = boneParId;
				

				

				//std::cout << "ended with" + std::to_string(in.bones[counterB].mesh.rot.z) << std::endl;
				//std::cout << "ended with" + std::to_string(counterB) << std::endl;

				
			}
		}

		for(int i=0;i< in.bones.size();i++)
			if (in.bones[i].parId != -1)
				AttachBone(in.bones[i], in.bones[in.bones[i].parId], in.bones[i].outPointId);

		
	}
	else
	{
		std::cout << "error";
	}

}

struct AnimationKey
{
	float start, end;
	Pnt3 beginAng, ang;
	Pnt3 locker;
	int type;
	int bone;
	AnimationKey(float istart, float iend, int itype, int ibone, Pnt3 ibang, Pnt3 iang)//, Pnt3 ilocker)
	{
		start = istart;
		end = iend;
		bone = ibone;
		beginAng = ibang;
		ang = iang;
		type = itype;
		//locker = ilocker;
	}
};

struct Animation
{
	float timer;
	float speed;
	std::vector<AnimationKey> keys;
	float duration;
	Animation()
	{
		speed = 1.0f;
	}
};

void saveAnimation(Animation a, std::string wha)
{
	std::ofstream fout;
	fout.open(wha);
	if (fout.is_open())
	{
		fout << std::to_string(a.duration) + "\n";
		fout << std::to_string(a.speed) + "\n";

		for (int i = 0; i < a.keys.size(); i++)
		{
			fout << "N " + std::to_string(a.keys[i].start) + " " + std::to_string(a.keys[i].end) + " " + //time limits
				std::to_string(a.keys[i].type) + " " + //type of animation (position/rotation)
				std::to_string(a.keys[i].bone) + " " + //bone index
				//vector on the begining
				std::to_string(a.keys[i].beginAng.x) + " " + std::to_string(a.keys[i].beginAng.y) + " " + std::to_string(a.keys[i].beginAng.z) + " " +
				//delta vector
				std::to_string(a.keys[i].ang.x) + " " + std::to_string(a.keys[i].ang.y) + " " + std::to_string(a.keys[i].ang.z) + "\n";

		}

	}
	else
	{
		std::cout << "error with saving animation";
	}
}

void loadAnimation(Animation& in, std::string wha)
{
	std::ifstream fin;
	fin.open(wha);

	if (fin.is_open())
	{
		float floatVar;
		
		std::string type;
		Pnt3 vec1,vec2;
		float begin, end;
		int atype, bone;

		fin >> floatVar;
		in.duration = floatVar;

		fin >> floatVar;
		in.speed = floatVar;

		while (!fin.eof())
		{
			type = " ";
			fin >> type;

			if (type == "N")
			{
				fin >> begin;
				fin >> end;

				fin >> atype;
				fin >> bone;

				fin >> vec1.x;
				fin >> vec1.y;
				fin >> vec1.z;

				fin >> vec2.x;
				fin >> vec2.y;
				fin >> vec2.z;

				in.keys.push_back(AnimationKey(begin,end,atype,bone,vec1.copy(),vec2.copy()));

			}
		}

	}
	else
	{
		std::cout << "error";
	}

}



void playAnimation(Body& body, Animation& anim, float t)
{
	anim.timer += t*anim.speed;
	if (anim.timer > anim.duration)
		anim.timer = anim.timer - anim.duration;
	for (int i = 0; i < anim.keys.size(); i++)
	{
		if (anim.timer >= anim.keys[i].start && anim.timer <= anim.keys[i].end)
		{
			float proc = 0;
			if(anim.keys[i].end - anim.keys[i].start>0)
				proc = 1-(anim.keys[i].end- anim.timer) / (anim.keys[i].end-anim.keys[i].start);
			if (anim.keys[i].end == anim.keys[i].start)
				proc = 1;

			if (anim.keys[i].type == 0)
			{
				Pnt3 deltaAng = anim.keys[i].ang - anim.keys[i].beginAng;
				Pnt3 start = anim.keys[i].beginAng;
				body.bones[anim.keys[i].bone].ang = start + deltaAng * proc;
			}
			else
			{
				body.bones[anim.keys[i].bone].pos = mul3d(anim.keys[i].ang, proc);
				body.bones[anim.keys[i].bone].pos = sum3d(anim.keys[i].beginAng, body.bones[anim.keys[i].bone].pos);
			}
		}
	}
}
*/

float lookAtX(Pnt3 p1, Pnt3 p2)
{
	float dz = p2.z - p1.z;
	float dy = p2.y - p1.y;
	float ang = std::atan((dz / dy));
	if (dy < 0)
		ang = ang - 3.1415f;
	return ang;
}


float lookAtY(Pnt3 p1, Pnt3 p2)
{
	float dx = p2.x - p1.x;
	float dz = p2.z - p1.z;
	float ang = std::atan((dz / dx));
	if (dx < 0)
		ang = ang - PI;
	return ang;
}



float lookAtZ(Pnt3 p1, Pnt3 p2)
{
	float dx = p2.x - p1.x;
	float dy = p2.y - p1.y;
	float ang = std::atan((dx / dy));
	if (dy < 0)
		ang = ang - PI;
	return ang;
}