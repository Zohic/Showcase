#pragma once
#include "EInstance.h"

static EInstance* thrower;

enum class ShapeType
{
	CIRCLE,
	RECT,
	POLYGON
};



class Geometry
{
	std::vector<Vec2d> points;
	int pointsNumber;
	DirectGraphics* gfx;
public:
	ShapeType shapeType = ShapeType::POLYGON;

	Geometry()
	{
		pointsNumber = 0;
		gfx = nullptr;
	}
	Geometry(DirectGraphics* g)
	{
		pointsNumber = 0;
		SetRenderTarget(g);
	}
	int Size() const
	{
		return points.size();
	}
	void AddPnt(const Vec2d& pnt)
	{
		points.push_back(pnt);
	}
	Vec2d GetPoint(int ind) const
	{
		
		if (ind < points.size())
			return points[ind];
		else return(Vec2d(0));
	}
	void SetPnt(int ind, const Vec2d& newVec)
	{
		
		points[ind] = newVec;
	}
	void CreatePolygon(float r, int sides)
	{
		points.clear();
		shapeType = ShapeType::POLYGON;

		float ang = 2 * EPI / sides;

		for (int cv = 0; cv < sides; cv++)
		{
			AddPnt(
				Vec2d(
					cos(ang * -cv),
					sin(ang * -cv)) * r
			);
		}
		pointsNumber = sides;
	}
	void CreateRect(float w, float h, bool centered=true)
	{
		points.clear();
		shapeType = ShapeType::RECT;
		if (centered)
		{
			AddPnt(Vec2d(-w / 2, -h / 2));
			AddPnt(Vec2d(-w / 2, h / 2));
			AddPnt(Vec2d(w / 2, h / 2));
			AddPnt(Vec2d(w / 2, -h / 2));
			//EInstance::EException::CallCatch(Ex)
			
		}
		else
		{
			AddPnt(Vec2d(0, 0));
			AddPnt(Vec2d(w, 0));
			AddPnt(Vec2d(w, h));
			AddPnt(Vec2d(0, h));
		}
		
		pointsNumber = 4;
	}
	void SetRenderTarget(DirectGraphics* g)
	{
		gfx = g;
	}

	void Draw(Vec2d position, EColor clr = EPalette::WHITE) const
	{
		assert(gfx != nullptr);
		for (int pn = 1; pn < pointsNumber; pn++)
		{
			gfx->DrawLine(
				position.x+points[pn-1].x, position.y + points[pn - 1].y,
				position.x + points[pn].x, position.y + points[pn].y, clr);
		}
		gfx->DrawLine(
			position.x + points[pointsNumber-1].x, position.y + points[pointsNumber -1].y,
			position.x + points[0].x,              position.y + points[0].y, clr);


	}
	void Draw(Vec2d position, DirectGraphics* g, EColor clr = EPalette::WHITE) const
	{
		assert(g != nullptr);
		for (int pn = 1; pn < pointsNumber; pn++)
		{
			g->DrawLine(
				position.x + points[pn - 1].x, position.y + points[pn - 1].y,
				position.x + points[pn].x, position.y + points[pn].y, clr);
		}
		g->DrawLine(
			position.x + points[pointsNumber - 1].x, position.y + points[pointsNumber - 1].y,
			position.x + points[0].x, position.y + points[0].y, clr);


	}

	void Draw(Vec2d p, float angle, EColor clr = EPalette::WHITE) const
	{
		assert(gfx != nullptr);
		Mat22 rotation2x2 = Mat22::RotationMat(angle);
		Vec2d prevRot = rotation2x2 * points[0];
		Vec2d firstRot = prevRot;
		Vec2d newRot;

		for (int pn = 1; pn < pointsNumber; pn++)
		{
			newRot = rotation2x2 * points[pn];

			gfx->DrawLine(
				p.x + prevRot.x, p.y + prevRot.y,
				p.x + newRot.x,  p.y + newRot.y, clr);

			prevRot = newRot;
			
		}
		gfx->DrawLine(
			p.x + newRot.x,   p.y + newRot.y,
			p.x + firstRot.x, p.y + firstRot.y, clr);
	}
	void Draw(Vec2d p, float angle, DirectGraphics* g, EColor clr = EPalette::WHITE) const
	{
		assert(g != nullptr);
		Mat22 rotation2x2 = Mat22::RotationMat(angle);
		Vec2d prevRot = rotation2x2 * points[0];
		Vec2d firstRot = prevRot;
		Vec2d newRot;

		for (int pn = 1; pn < pointsNumber; pn++)
		{
			newRot = rotation2x2 * points[pn];

			g->DrawLine(
				p.x + prevRot.x, p.y + prevRot.y,
				p.x + newRot.x, p.y + newRot.y, clr);

			prevRot = newRot;

		}
		g->DrawLine(
			p.x + newRot.x, p.y + newRot.y,
			p.x + firstRot.x, p.y + firstRot.y, clr);
	}

	void DrawOut(Vec2d p, float angle, DirectGraphics* g, std::vector<Vec2d>& out, EColor clr = EPalette::WHITE) const
	{
		assert(g != nullptr);
		assert(out.size() == pointsNumber);

		Mat22 rotation2x2 = Mat22::RotationMat(angle);
		Vec2d prevRot = rotation2x2 * points[0];
		Vec2d firstRot = prevRot;
		Vec2d newRot;

		out[0] = p+firstRot;

		for (int pn = 1; pn < pointsNumber; pn++)
		{
			newRot = rotation2x2 * points[pn];
			out[pn] = p+newRot;
			g->DrawLine(
				p.x + prevRot.x, p.y + prevRot.y,
				p.x + newRot.x, p.y + newRot.y, clr);
			prevRot = newRot;

		}
		g->DrawLine(
			p.x + newRot.x, p.y + newRot.y,
			p.x + firstRot.x, p.y + firstRot.y, clr);
	}

	static void Draw(const std::vector<Vec2d>& pts, DirectGraphics* g, EColor clr = EPalette::WHITE)
	{
		

		for (int pn = 1; pn < pts.size(); pn++)
		{
			g->DrawLine(
				pts[pn - 1].x, pts[pn - 1].y,
				pts[pn].x,     pts[pn].y, clr);
		}
		g->DrawLine(
			pts[pts.size() - 1].x, pts[pts.size() - 1].y,
			pts[0].x, pts[0].y, clr);
	}
	static void Draw(const std::vector<Vec2d>& pts, Vec2d offset, DirectGraphics* g, EColor clr = EPalette::WHITE)
	{
		assert(g != nullptr);
		for (int pn = 1; pn < pts.size(); pn++)
		{
			g->DrawLine(offset + pts[pn - 1], offset + pts[pn], clr);
		}
		g->DrawLine(offset + pts[pts.size() - 1], offset + pts[0], clr);
	}

};

class Body
{
	
	std::vector<Vec2d> transformed;

	Vec2d position;
	float orientation;
	
	bool isStatic = false;

	DirectGraphics* gfx;
	
public:
	Body()
	{
		shape = nullptr;
		gfx = nullptr;

		orientation = 0;
		angularVelocity = 0;

		rigidity = restitution = friction = 0;
		
	}
	Geometry* shape;
	Body(Geometry* sh, float m, DirectGraphics* g, bool bstatic=false)
	{
		SetShape(sh);
		gfx = g;
		orientation = 0;
		angularVelocity = 0;
		isStatic = bstatic;
		rigidity = restitution = friction = 0;
		SetMass(m);
	}

	Vec2d velocity;
	float angularVelocity;
	float I_;
	float _I;
	std::vector<Vec2d> forceFields;

	float mass_, _mass;
	float rigidity;
	float restitution;
	float friction;
	float staticFriction;


	const Geometry& GetShape() const
	{
		return *shape;
	}
	void SetShape(Geometry* s)
	{
		
		shape = s;
		transformed.resize(s->Size());
		
		
	}

	Vec2d GetPoint(int ind) const
	{
		assert(ind < shape->Size());
		return transformed[ind];
	}

	void SetRenderTarget(DirectGraphics* g)
	{
		gfx = g;
	}

	void Transform()
	{
		int sz = shape->Size();
		
		Mat22 rotation2x2 = Mat22::RotationMat(orientation);
		for (int p = 0; p < sz; p++)
		{
			
			transformed[p] = position+rotation2x2 * shape->GetPoint(p);
		}
		int sz2 = shape->Size();
		
	}

	void Draw(EColor clr = EColor(255, 255, 255))
	{
		Geometry::Draw(transformed, gfx, clr);
	}

	void SetMass(float density)
	{
		Vec2d c(0.0f, 0.0f); // centroid
		float area = 0.0f;
		float I = 0.0f;
		const float k_inv3 = 1.0f / 3.0f;

		for (int i1 = shape->Size()-1; i1 >=0; --i1)
		{
			// Triangle vertices, third vertex implied as (0, 0)
			Vec2d p1(shape->GetPoint(i1));
			int i2 = (i1-1 >= 0 ? i1 - 1 : shape->Size()-1);
			Vec2d p2(shape->GetPoint(i2));

			float D = CrossProduct(p1, p2);
			float triangleArea = 0.5f * D;

			area += triangleArea;

			// Use area to weight the centroid average, not just vertex position
			c += (p1 + p2)*triangleArea * k_inv3;

			float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
			float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
			I += (0.25f * k_inv3 * D) * (intx2 + inty2);
		}

		c *= 1.0f / area;

		// Translate vertices to centroid (make the centroid (0, 0)
		// for the polygon in model space)
		// Not really necessary, but I like doing this anyway
		

		mass_ = density * area;
		_mass = mass_ ? 1.0f / mass_ : 0.0f;
		I_ = I * density;
		_I = I_ ? 1.0f / I_ : 0.0f;
	
		
	}

	void Move(const Vec2d& m)
	{
		position += m;
	}
	Vec2d GetPos() const
	{
		return position;
	}
	void SetPos(const Vec2d& v)
	{
		position = v;
	}

	void Rotate(float a)
	{
		orientation += a;
	}
	float GetOrientation() const
	{
		return orientation;
	}
	void SetOrientation(float a)
	{
		orientation = a;
	}

	void MakeStatic()
	{
		mass_ = 0;
		_mass = 0;
		I_ = 0;
		_I = 0;
	}

};



bool SATOverlap(Body& bodyA, Body& bodyB, DirectGraphics* gfx=nullptr)
{
	if (gfx!=nullptr)
	{
		for (int vn = 0; vn < bodyA.GetShape().Size(); vn++)
		{
			int next = (vn + 1) % bodyA.GetShape().Size();

			Vec2d edge = bodyA.GetPoint(next)- bodyA.GetPoint(vn);

			Vec2d normal = edge.GetNormal().Normalized();

			gfx->DrawVector(bodyA.GetPoint(vn) + edge / 2, normal * 30, EColor(255, 100, 0));
		}
		for (int vn = 0; vn < bodyB.GetShape().Size(); vn++)
		{
			int next = (vn + 1) % bodyB.GetShape().Size();
			Vec2d edge = bodyB.GetPoint(next) - bodyB.GetPoint(vn);
			Vec2d normal = edge.GetNormal().Normalized();
			
			gfx->DrawVector(bodyB.GetPoint(vn) + edge / 2, normal * 30, EColor(255, 100, 0));
		}

	}

	for (int vn = 0; vn < bodyA.GetShape().Size(); vn++)
	{
		int next = (vn + 1) % bodyA.GetShape().Size();
		Vec2d edge = bodyA.GetPoint(next) - bodyA.GetPoint(vn);
		Vec2d normal = edge.GetNormal();

		float minV1 = INFINITY; float maxV1 = -INFINITY;
		float minV2 = INFINITY; float maxV2 = -INFINITY;

		for (int vp = 0; vp < bodyA.GetShape().Size(); vp++)
		{
			float proj = DotProduct(bodyA.GetPoint(vp), normal);

			maxV1 = std::fmax(maxV1, proj);
			minV1 = std::fmin(minV1, proj);

		}


		for (int vp = 0; vp < bodyB.GetShape().Size(); vp++)
		{
			float proj = DotProduct(bodyB.GetPoint(vp), normal);
			maxV2 = std::fmax(maxV2, proj);
			minV2 = std::fmin(minV2, proj);
		}

		if (!(maxV2 >= minV1 && maxV1 >= minV2))
			return false;

	}

	for (int vn = 0; vn < bodyB.GetShape().Size(); vn++)
	{
		int next = (vn + 1) % bodyB.GetShape().Size();
		Vec2d edge = bodyB.GetPoint(next) - bodyB.GetPoint(vn);
		Vec2d normal = edge.GetNormal();
		
		float minV1 = INFINITY; float maxV1 = -INFINITY;
		float minV2 = INFINITY; float maxV2 = -INFINITY;

		for (int vp = 0; vp < bodyA.GetShape().Size(); vp++)
		{
			float proj = DotProduct(bodyA.GetPoint(vp) , normal);
			maxV1 = std::fmax(maxV1, proj);
			minV1 = std::fmin(minV1, proj);
		}

		for (int vp = 0; vp < bodyB.GetShape().Size(); vp++)
		{
			float proj = DotProduct(bodyB.GetPoint(vp), normal);
			maxV2 = std::fmax(maxV2, proj);
			minV2 = std::fmin(minV2, proj);
		}

		if (!(maxV2 >= minV1 && maxV1 >= minV2))
			return false;

	}
	
	return true;
}





Vec2d GetSupportPoint(const Body& body, Vec2d dir)
{

	Vec2d furthest = body.GetPoint(0);
	float dp = body.GetPoint(0) * dir;

	for (int sp = 1; sp < body.GetShape().Size(); sp++)
	{
		float ndp = body.GetPoint(sp) * dir;
		if (ndp > dp)
		{
			dp = ndp;
			furthest = body.GetPoint(sp);
		}
	}
	return furthest;

}

float FindLeastPenetration(const Body& bodyA, const Body& bodyB, int& ind, Vec2d& bnorm)
{
	float bestDist = -INFINITY;
	int bestInd=-1;
	Vec2d bestNorm = Vec2d(0);
	
	for (int cv = 0; cv < bodyA.GetShape().Size(); cv++)
	{
		Vec2d norm = (bodyA.GetPoint((cv + 1) % bodyA.GetShape().Size() ) - bodyA.GetPoint(cv)).GetNormal().Normalized();

		Vec2d supp = GetSupportPoint(bodyB, -norm);

		float dist = norm * (supp - bodyA.GetPoint(cv));
		if (dist > bestDist)
		{
			bestNorm = norm;
			bestInd = cv;
			bestDist = dist;
		}
	}

	ind = bestInd;
	bnorm = bestNorm;
	return bestDist;
}


void ImpulseNormResolution(Body* b1, Body* b2, Vec2d norm, float dt, Vec2d contactPoint, DirectGraphics* dxg)
{

	Vec2d contact1 = contactPoint - b1->GetPos();
	Vec2d contact2 = contactPoint - b2->GetPos();
	Vec2d rotVel1 = CrossProduct(b1->angularVelocity, contact1);
	Vec2d rotVel2 = CrossProduct(b2->angularVelocity, contact2);

	

	Vec2d relV = (b2->velocity+rotVel2) - (b1->velocity+rotVel1);

	float proj = relV * norm;

	if (proj > 0)
		return;

	float e = (b1->restitution + b2->restitution)*0.5f;

	float k = sqrtf(b1->friction * b2->friction);
	float sk = sqrtf(b1->staticFriction * b2->staticFriction);

	float contactCross1 = fabsf(CrossProduct(contact1, norm));
	float contactCross2 = fabsf(CrossProduct(contact2, norm));

	
	
	float sumInvMass = (b1->_mass + b2->_mass) +(b1->_I * sqrtf(contactCross1) + b2->_I * sqrtf(contactCross2));

	float j = (-(1 + e) * proj) / sumInvMass;

	Vec2d impulse = norm * j;

	b1->velocity -= (impulse * b1->_mass);
	b2->velocity += (impulse * b2->_mass);

	b1->angularVelocity -= b1->_I * CrossProduct(contact1, impulse);
	b2->angularVelocity += b2->_I * CrossProduct(contact2, impulse);


	
	Vec2d tangent = relV - norm*(relV * norm);
	tangent.Normalize();

	dxg->DrawVector(b1->GetPos(), tangent * 10, EPalette::YELLOW);
	dxg->DrawVector(b2->GetPos(), -tangent * 10, EPalette::YELLOW);
	dxg->DrawVector(b1->GetPos(), norm * 10, EPalette::YELLOW);

	float friction = (norm * relV) / sumInvMass*k;

	Vec2d frictionImp = tangent * friction;

	/*if (fabsf(friction) < j * sk)
	{
		frictionImp = tangent * friction;
		dxg->DrawVector(b2->GetPos(), -frictionImp * 30, GREEN);
	}
	else
	{
		frictionImp = tangent * friction * k;
		dxg->DrawVector(b2->GetPos(), -frictionImp * 30, GREEN);
	}*/
	

	//b1->velocity -= (frictionImp * b1->_mass);
	//b2->velocity += (frictionImp * b2->_mass);

	//b1->angularVelocity -= b1->_I * CrossProduct(contact1, frictionImp)*e;
	//b2->angularVelocity += b2->_I * CrossProduct(contact2, frictionImp)*e;


}


void DoPhysics(Body& b1, Body& b2, float dt, DirectGraphics* dxg = nullptr)
{

	/*b1->velocity += b1->acceleration * dt/2;
	b2->velocity += b2->acceleration * dt/2;

	b1->position += b1->velocity * dt/2;
	b2->position += b2->velocity * dt/2;*/
	
	

	int faceInd1;
	int faceInd2;
	Vec2d norm1;
	Vec2d norm2;
	float dst1 = FindLeastPenetration(b1, b2, faceInd1,norm1);
	float dst2 = FindLeastPenetration(b2, b1, faceInd2,norm2);
	
	Body* body1;
	Body* body2;

	Vec2d face;
	Vec2d norm = norm1;
	body1 = &b1;
	body2 = &b2;
	int faceInd = faceInd2;
	float dst = dst1;
	if (dst2 > dst1)
	{
		faceInd = faceInd1;
		body2 = &b1;
		body1 = &b2;
		norm = norm2;
		dst = dst2;
	}
		
	float penet = std::min(dst, dst2);


	if (dxg != nullptr)
	{

			
		//dxg->DrawCircle(body2->shape->worldPnt[faceInd], 25, EColor(255, 255, 0));
		//dxg->DrawCircle(b2->shape->worldPnt[faceInd2], 25, EColor(0, 255, 255));

		//dxg->DrawVector(b1->shape->worldPnt[faceInd1], norm1 * dst1, EColor(255, 0, 0));
		//dxg->DrawVector(b2->shape->worldPnt[faceInd2], norm2 * dst2, EColor(255, 255, 0));

		//dxg->DrawVector(body2->shape->worldPnt[faceInd], norm * dst, EColor(255, 0, 0));
		//dxg->DrawVector(body2->shape->worldPnt[faceInd2], norm * dst, EColor(0, 255, 0));
			
		dxg->DrawText(30, 200, std::to_string(dst1), EColor(255, 255, 250));
		dxg->DrawText(30, 240, std::to_string(dst2), EColor(255, 255, 250));
		//dxg->DrawVector(b2->position, b2->shape->worldPnt[vInd] - norm * dst - b2->position, EColor(255, 0, 0));

		//dxg->DrawVector(b2->position, b2->shape->worldPnt[vInd] - norm * dst - b2->position, EColor(255, 0, 0));
		//dxg->DrawVector(b2->position, b2->velocity, EColor(255, 255, 0));
	}



	Vec2d PtP = (body2->GetPos() - body1->GetPos()).Normalized();
	Vec2d far1 = GetSupportPoint(*body1, norm);
	Vec2d far2 = GetSupportPoint(*body2, -norm);

	//dxg->DrawVector(body1->GetPos(), far2 - body1->GetPos(), MAGENTA);
	//dxg->DrawVector(body2->GetPos(), far1 - body2->GetPos(), MAGENTA);

	float p1 = (far1 - body2->GetPos()).Normalized() * PtP;
	float p2 = (far2 - body1->GetPos()).Normalized() * PtP;

	Vec2d contact = far1;
	if (p2 > p1)
		contact = far2;




	//dxg->DrawVector(body1->position, PtP, EColor(0, 255, 0));
	//dxg->DrawCircle(far1, 20, EColor(255, 255, 0));
	//dxg->DrawCircle(far2, 20, EColor(255, 255, 0));
	dxg->DrawCircle(contact, 22, EColor(255, 0, 0));

	//dxg->DrawText(130, 240, std::to_string(b1->velocity.Length()*dt), EColor(255, 255, 250));
	dxg->DrawText(130, 250, std::to_string(dst), EColor(255, 255, 250));
		

	

	float mm = b1._mass + b2._mass;//1.0f / (b1.mass_ + b2.mass_);
	//float m1 = b1->mass * mm;

	body1->Move(norm * dst * body1->_mass / mm);
	body2->Move(-norm * dst * body2->_mass / mm);


	ImpulseNormResolution(body1, body2, norm, dt, contact, dxg);

}

void MoveBody(Body& b, float dt)
{
	for(int i=0;i<b.forceFields.size();i++)
		b.velocity += b.forceFields[i] * b._mass * dt;
	b.Rotate(b.angularVelocity*dt);
	//b.angularVelocity *= 0.99f;
	b.Move(b.velocity * dt);
}



/*
void ImpulseResolution(Body* b1, Body* b2, float dt, DirectGraphics* dxg = nullptr)
{
	int faceInd;
	int faceInd2;
	bool secondB = false;
	float dst = FindLeastPenetration(*b1->shape, *b2->shape, &faceInd);
	float dst2 = FindLeastPenetration(*b2->shape, *b1->shape, &faceInd2);

	if (dst2 > dst)
	{
		dst = dst2;
		secondB = true;
	}
	Vec2d face;
	Vec2d norm;

	if (!secondB)
	{
		face = b1->shape->worldPnt[(faceInd + 1) % b1->shape->worldPnt.size()] - b1->shape->worldPnt[faceInd];
		norm = face.GetNormal().Normalized();

		float toVertexProj = INFINITY;
		int vInd = -1;
		for (int i = 0; i < b2->shape->worldPnt.size(); i++)
		{
			float toV = (b2->shape->worldPnt[i] - b1->position) * norm;
			if (toV < toVertexProj)
			{
				toVertexProj = toV;
				vInd = i;
			}
		
		}

		if (dxg != nullptr)
		{
			
			dxg->DrawCircle(b2->shape->worldPnt[vInd] - norm * dst, 25, EColor(255, 0, 0));
			dxg->DrawVector(b2->position, b2->shape->worldPnt[vInd] - norm * dst - b2->position, EColor(255, 0, 0));
			dxg->DrawVector(b2->position, b2->velocity, EColor(255, 255, 0));
		}

		float masRel = b1->mass * b2->invMass;

		float frictionK = (b1->friction + b2->friction) / 2;

		float N1 = fabsf(dst) / dt / dt * b1->mass;
		float N2 = fabsf(dst) / dt / dt * b2->mass;

		float F1 = N1 * frictionK;
		float F2 = N2 * frictionK;

		float dp1 = F1 * dt;
		float dp2 = F2 * dt;

		b1->velocity -= b1->velocity.Normalized() * dp1 * b1->invMass;
		//b2->velocity -= b2->velocity.Normalized() * dp2 * b2->invMass;


		b1->position += (norm * dst * (1 - masRel));
		b2->position -= (norm * dst * (masRel));

		ImpulseNormResolutionWithRotation(b1, b2, norm, b2->shape->worldPnt[vInd]-norm*dst, dt);
	}
	else
	{
		face = b2->shape->worldPnt[(faceInd2 + 1) % b2->shape->worldPnt.size()] - b2->shape->worldPnt[faceInd2];
		norm = face.GetNormal().Normalized();

		float toVertexProj = INFINITY;
		int vInd = -1;

		for (int i = 0; i < b1->shape->worldPnt.size(); i++)
		{
			float toV = (b1->shape->worldPnt[i] - b2->position) * norm;
			if (toV < toVertexProj)
			{
				toVertexProj = toV;
				vInd = i;
			}

		}

		if (dxg != nullptr)
		{
			dxg->DrawCircle(b1->shape->worldPnt[vInd]-norm*dst, 25, EColor(255, 0, 0));
			dxg->DrawVector(b1->position, b1->shape->worldPnt[vInd] - norm * dst - b1->position, EColor(255, 0, 0));
			dxg->DrawVector(b1->position, b1->velocity, EColor(255, 255, 0));
		}

		float masRel = b1->mass * b2->invMass;

		float frictionK = (b1->friction + b2->friction) / 2;

		float N1 = fabsf(dst) / dt / dt * b1->mass;
		float N2 = fabsf(dst) / dt / dt * b2->mass;

		float F1 = N1 * frictionK;
		float F2 = N2 * frictionK;

		float dp1 = F1 * dt;
		float dp2 = F2 * dt;

		b1->velocity -= b1->velocity.Normalized() * dp1 * b1->invMass;
		//b2->velocity -= b2->velocity.Normalized() * dp2 * b2->invMass;

		b1->position -= (norm * dst * (1 - masRel));
		b2->position += (norm * dst * (masRel));

		ImpulseNormResolutionWithRotation(b2, b1, norm, b1->shape->worldPnt[vInd]-norm*dst, dt);

	}


Vec2d CenterImpulse(float m1, float v1, float m2, float v2)
{
	return Vec2d(
		(v1 * (m1 - m2) + 2.0f * m2 * v2) / (m1 + m2),//v1'
		(v2 * (m2 - m1) + 2.0f * m1 * v1) / (m1 + m2)// v2'
	);
}

void CircleImpulseResolution(Body* body1, Body* body2)
{
	Vec2d cToC = (body2->position - body1->position).Normalized();
	Vec2d tangent = cToC.GetNormal();


	float norVel1 = DotProduct(body1->velocity , cToC);
	float norVel2 = DotProduct(body2->velocity, cToC);

	Vec2d norVel = CenterImpulse(body1->mass, norVel1, body2->mass, norVel2);

	//Vec2d newVel1 = cToC * norVel.x;
	//Vec2d newVel2 = cToC * norVel.y;

	float tanVel1 = DotProduct(body1->velocity, tangent);
	float tanVel2 = DotProduct(body2->velocity, tangent);


	Vec2d newVel2 = tangent * tanVel2;

	body1->velocity = tangent * tanVel1 + cToC * norVel.x;
	body2->velocity = tangent * tanVel2 + cToC * norVel.y;



void DoRigid(Body* b1, Body* b2, DirectGraphics* dxg = nullptr)
{
	bool info = false;
	if (dxg != nullptr)
		info = true;
	float BpenetDist = -INFINITY;

	
	

	for (int pd = 0; pd < b1->shape->worldPnt.size(); pd++)
	{
		Vec2d face = b1->shape->worldPnt[(pd+1)%b1->shape->worldPnt.size()] - b1->shape->worldPnt[pd];
		Vec2d faceNorm = face.GetNormal().Normalized();
		//if (info)
		//	dxg->DrawVector(b1->shape->worldPnt[pd], faceNorm, EColor(255, 255, 0));

		Vec2d supp = GetSupportPoint(*b2->shape, faceNorm);

		if (info)
			dxg->DrawCircle(supp, 10, clrs[pd % 10]);

		float penetDist = (supp - b1->shape->worldPnt[pd]) * faceNorm;

		if (info)
			dxg->DrawVector(b1->shape->worldPnt[pd]+face/2, faceNorm*penetDist, clrs[pd%10]);


	}


}

void ImpulseNormResolutionWithRotation(Body* b1, Body* b2, Vec2d norm, Vec2d contact, float dt)
{
	Vec2d arm1 = (contact - b1->position);
	Vec2d arm2 = (contact - b2->position);

	//Vec2d vel1 = b1->velocity - (arm1.GetNormal().Normalized() * arm1.Length() * b1->velocity.Length());
	//Vec2d vel2 = b2->velocity - (arm2.GetNormal().Normalized() * arm2.Length() * b2->velocity.Length());
	Vec2d relV = b2->velocity - b1->velocity;

	float proj = relV * norm;

	if (proj > 0)
		return;

	float e = std::fmin(b1->restitution, b2->restitution);

	float j = (-(1 + e) * proj) / (b1->_mass + b2->_mass);

	Vec2d impulse = norm * j;
	Vec2d Force = impulse / dt;
	float torq1 = CrossProduct(Force, (contact - b1->position));
	float torq2 = CrossProduct(Force, (contact - b2->position));



	float lCoof2 = (contact - b2->position).Length() / b2->shape->maxDist;



	//b1->angularVelocity -= torq1 * dt / b1->momentOfInertia;
	//b2->angularVelocity -= torq2 * dt / b2->momentOfInertia;

	b1->velocity -= (impulse * b1->invMass);
	b2->velocity += (impulse * b2->invMass);
}

*/









