#include <string>

#include "ELogic.h"
#include "EPhysics.h"

#define EEXC_FILENAME "PhysicsTest.cpp"


struct Test :public ELogic
{
	

	void drawVector(Vec2d p, Vec2d v)
	{
		dxg->DrawLine(p.x, p.y, p.x + v.x, p.y + v.y,EColor(255,255,255));


		dxg->DrawLine(p.x + v.x, p.y + v.y,
					 p.x + v.x + v.GetNormal().Normalized().x * 5, 
					 p.y + v.y + v.GetNormal().Normalized().y * 5, EColor(255, 255, 255));

		dxg->DrawLine(p.x + v.x, p.y + v.y,
			p.x + v.x - v.GetNormal().Normalized().x * 5,
			p.y + v.y - v.GetNormal().Normalized().y * 5, EColor(255, 255, 255));
	}

	void showBool(Vec2d p, bool a)
	{
		if (a)
		{
			dxg->DrawText(p.x, p.y, "TRUE", EPalette::YELLOW);
		}
		else
		{
			dxg->DrawText(p.x, p.y, "FALSE", EPalette::YELLOW);
		}
	}

	

	std::vector<Geometry> shapes;
	std::vector<Body> bodies;

	

	std::vector<IVec2d> pairs;

	float timerImp;
	float sumImp;
	float avgImp;

	Vec2d center;
	void Setup() override
	{
		center = Vec2d(dxg->screenWidth / 2.0f, dxg->screenHeight / 2.0f);
		shapes.resize(3);
		shapes[0].CreatePolygon(500,4);
		shapes[1].CreatePolygon(140, 4);
		shapes[2].CreatePolygon(88, 5);

		bodies.push_back(Body(&shapes[0], 99999, dxg, true));
		bodies.push_back(Body(&shapes[1], 1, dxg));
		bodies.push_back(Body(&shapes[2], 1 , dxg));

		bodies[1].forceFields.push_back(Vec2d(0, bodies[1].mass_ * 2000));
		bodies[2].forceFields.push_back(Vec2d(0, bodies[2].mass_ * 2000));

		bodies[0].MakeStatic();

		bodies[0].restitution = 0.0f;
		bodies[1].restitution = 0.0f;
		bodies[2].restitution = 0.0f;

		bodies[0].friction = 0.21f;
		bodies[1].friction = 0.21f;
		bodies[2].friction = 0.21f;

		bodies[0].staticFriction = 0.31f;
		bodies[1].staticFriction = 0.36f;
		bodies[2].staticFriction = 0.36f;

		bodies[0].SetPos(Vec2d(500, 900));
		bodies[1].SetPos(Vec2d(350, 150));
		bodies[2].SetPos(Vec2d(540, 100));

		bodies[0].SetOrientation(EPI/4);
		bodies[1].SetOrientation(EPI / 2+0.18f);


	}

	

	void FrameWork() override
	{
		if (dxg ==nullptr)
			throw EInstance::ECrashHandler("dxg = null", EEXC_FILENAME, 666);
		if (input == nullptr)
			throw EInstance::ECrashHandler("input = null", EEXC_FILENAME, 666);
		if (timer == nullptr)
			throw EInstance::ECrashHandler("timer = null", EEXC_FILENAME, 666);
		
		Vec2d mouseVec = Vec2d(input->mouseX, input->mouseY);

		
		dxg->swapTexture.Clear(EColor(0, 20, 50));
		dxg->DrawText(0, 0, std::to_string(input->mouseX), EPalette::YELLOW);
		dxg->DrawText(0, 20, std::to_string(input->mouseY), EPalette::YELLOW);
		

		int pairsCount = bodies.size() * (bodies.size() - 1) * 0.5f;
		
	
		

		if (input->keyStates[KW].down)
			bodies[0].Rotate(timer->deltaTime * EPI*2.5f);
		if (input->keyStates[KS].down)
			bodies[0].Rotate(-timer->deltaTime * EPI*2.5f);
		if (input->keyStates[KD].down)
			bodies[1].Rotate(timer->deltaTime * EPI);
		if (input->keyStates[KA].down)
			bodies[1].Rotate(-timer->deltaTime * EPI);

		if (input->keyStates[KW].down)
			bodies[1].angularVelocity +=timer->deltaTime * EPI;
		if (input->keyStates[KS].down)
			bodies[1].angularVelocity -= timer->deltaTime * EPI;
		if (input->keyStates[KD].down)
			bodies[2].angularVelocity += timer->deltaTime * EPI*30;
		if (input->keyStates[KA].down)
			bodies[2].angularVelocity -= timer->deltaTime * EPI*30;
	
		
		
		
		
		if (input->keyStates[BL].down)
		{
			bodies[1].velocity = (mouseVec - bodies[1].GetPos()) / timer->deltaTime / 10;
		}
			

		if (input->keyStates[BR].down)
		{
			bodies[2].velocity = (mouseVec-bodies[2].GetPos()) / timer->deltaTime/10;
		}
			
		
		int num1 = 0;
		int num2 = 1;

		if(bodies.size()>0)
		while(num1 < bodies.size() - 1 || num2 < bodies.size())
		{
			if (num2 < bodies.size())//   
			{
				pairs.push_back(IVec2d(num1, num2));
				num2 += 1;
			}
			else
			{
				num1 += 1;
				num2 = num1 + 1;
				
			} 
			
		}
		
		

		

		timerImp += 1;

		
		
		Vec2d sdir = mouseVec - center;
		
		if(pairsCount>0)
			dxg->DrawText(300, 80, std::to_string(pairs[0].x)+" - "+ std::to_string(pairs[0].y), EPalette::WHITE);

		
		for (int i = 0; i < bodies.size(); i++)
		{
			MoveBody(bodies[i], timer->deltaTime);
			bodies[i].Transform();
		}

		for (int i = 0; i < pairsCount; i++)
		{
			bool over = SATOverlap
			(
				bodies[pairs[i].x],
				bodies[pairs[i].y],
				dxg
			);
			if (over)
			{
				dxg->DrawLine(bodies[pairs[i].x].GetPos(), bodies[pairs[i].y].GetPos(), EPalette::WHITE);
				Vec2d posCen = (bodies[pairs[i].x].GetPos() + bodies[pairs[i].y].GetPos())*0.5f;
				showBool(posCen, over);
				DoPhysics(bodies[pairs[i].x], bodies[pairs[i].y],  timer->deltaTime, dxg );
			}
		}
		
		for (int i = 0; i < bodies.size(); i++)
		{
			bodies[i].Transform();
			bodies[i].Draw();
		}

		pairs.clear();
		
	}


};


Test game;

int CALLBACK WinMain(
HINSTANCE hInstance,
HINSTANCE hPrevInstance,
LPSTR lpCmdLine,
int nCmdShow
)
{

	EInstance::GetInstance().Make(hInstance, 1000, 600, "EnErGeNiUs");
	game.Make(&EInstance::GetInstance());
	game.Begin();
	return EInstance::GetInstance().outCode;
}
