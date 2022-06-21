
#include <string>

#include "EInstance.h"

struct Game :public EInstance
{
	int rectS;
	int divX, divY;

	
	struct Rect
	{
		Vec2d pos;
		Vec2d size;
		Vec2d points[4];
		Rect(Vec2d p, Vec2d s)
		{
			pos = p;
			size = s;
			points[0] = pos;
			points[1] = pos + Vec2d(size.x, 0);
			points[2] = pos + size;
			points[3] = pos + Vec2d(0, size.y);
		}
	};

	std::vector<Rect> blocks;
	std::vector<Vec2d> blockedLight;
	std::vector<Vec2d> blockedLight2;
	std::vector<Vec2d> blockedLightPts1;
	std::vector<Vec2d> blockedLightPts2;

	Vec2d playerPos;
	Vec2d playerSize;
	float playerSpd;

	Vec2d camPos;

	Vec2d begM;

	void OnBegin() override
	{
		rectS = 15;
		divX = roundf((float)dxg.screenWidth / rectS);
		divY = roundf((float)dxg.screenHeight / rectS);
		playerSpd = 350.0f;
		


		blocks.push_back(Rect(Vec2d(100,100), Vec2d(100,70)));

		playerSize = Vec2d(20);

	}

	void FrameWork() override
	{
		dxg.swapTexture.Clear(EColor(0,0,0));

		Vec2d playerCenterCam = playerPos - Vec2d(rectS);

		camPos += (playerCenterCam - Vec2d(dxg.screenWidth, dxg.screenHeight) / 2 - camPos) / 26;

		Vec2d spd;
		spd.y = (input.keyStates[sDOWN].down - (int)input.keyStates[sUP].down) * playerSpd;
		spd.x = (input.keyStates[sRIGHT].down - (int)input.keyStates[sLEFT].down) * playerSpd;

		Vec2d mPos = Vec2d(input.mouseX, input.mouseY);

		if (input.keyStates[BL].pressed)
		{
			begM = mPos+camPos;
		}
		if (input.keyStates[BL].released)
		{
			blocks.push_back(Rect(begM, (mPos+camPos) - begM));
		}

		playerPos += spd*timer.deltaTime;

		blockedLight.clear();
		blockedLight2.clear();
		blockedLightPts1.clear();
		blockedLightPts2.clear();

		Vec2d playerCenter = playerPos + playerSize/2;

		for (int b = 0; b < blocks.size(); b++)
		{
			float dps[4];
			float dps2[4];
			float least, most;
			float least2, most2;
			Vec2d pts[2];
			Vec2d pts2[2];
			for (int dp = 0; dp < 4; dp++)
			{
				Vec2d vec = (blocks[b].points[dp] - playerCenter).Normalized();
				dps[dp] = vec * Vec2d(1, 0);
				dps2[dp] = vec * Vec2d(0, 1);
				if (dp == 0)
				{
					least = dps[0];
					least2 = dps2[0];
					pts[0] = blocks[b].points[0];
					pts2[0] = blocks[b].points[0];

					most = dps[0];
					most2 = dps2[0];
					pts[1] = blocks[b].points[0];
					pts2[1] = blocks[b].points[0];
				}
				else
				{
					if (dps[dp] > most)
					{
						pts[1] = blocks[b].points[dp];
						most = dps[dp];
					}

					if (dps2[dp] > most2)
					{
						pts2[1] = blocks[b].points[dp];
						most2 = dps2[dp];
					}
						

					if (dps[dp] < least)
					{
						pts[0] = blocks[b].points[dp];
						least = dps[dp];
					}


					if (dps2[dp] < least2)
					{
						pts2[0] = blocks[b].points[dp];
						least2 = dps2[dp];
					}

						
				}
			}
			
			if (fabsf(least2 - most2) > fabsf(least - most))
			{
				pts[0] = pts2[0];
				pts[1] = pts2[1];
			}

			blockedLight.push_back(Vec2d(least, most));
			blockedLight2.push_back(Vec2d(least2, most2));
			blockedLightPts1.push_back(pts[0]);
			blockedLightPts2.push_back(pts[1]);
			
		}

		for(int i=0;i<divX;i++)
			for (int j = 0; j < divY; j++)
			{
				bool pass = true;

				for (int b = 0; b < blockedLight.size(); b++)
				{
					Vec2d toPix = Vec2d(i, j) * rectS - playerCenter;
					float dp = toPix.Normalized() * Vec2d(1,0);
					float dp2 = toPix.Normalized() * Vec2d(0, 1);

					float dp3 = toPix.Normalized() * Vec2d(-1, 0);
					float dp4 = toPix.Normalized() * Vec2d(0, -1);

					Vec2d crossLine = (-blockedLightPts2[b] + blockedLightPts1[b]).Normalized();
					Vec2d orth = crossLine.GetNormal();
					Vec2d forProj = Vec2d(i, j) * rectS - blockedLightPts1[b];

					Vec2d toPla = playerCenter - blockedLightPts1[b];
					
					bool xGood = (dp > blockedLight[b].x && dp < blockedLight[b].y);
					bool yGood = (dp2 > blockedLight2[b].x && dp2 < blockedLight2[b].y);

					bool xBad = (dp3 > blockedLight[b].x&& dp3 < blockedLight[b].y);
					bool yBad = (dp4 > blockedLight2[b].x&& dp4 < blockedLight2[b].y);


					float xOG = orth * Vec2d(1, 0);
					float yOG = orth * Vec2d(0, 1);

					bool yeah = xOG > 0;
					bool diag = fabsf(xOG) != 1 && fabsf(yOG) != 1;



					//dxg.DrawText(10, 30, std::to_string(yeah), EColor(255, 0, 0));
					//dxg.DrawText(10, 50, std::to_string(yeah), EColor(255, 0, 0));

					
					if (!(((yBad && !yeah) || (xBad && yeah))) && ((yGood && yeah) || (xGood && !yeah)))
					if(esign(orth*forProj) != esign(orth*toPla))
					{
						pass = false;
					}
					
				}

				if (pass)
				{
					Vec2d tPos = Vec2d(i * rectS, j * rectS);
					float ds = (tPos - playerCenter).sLength();
					ds = ds / 100000;
					if (ds < 1)
					{
						ds = 1 - ds;
						if (ds < 0.1f)
							ds = 0.1f;
						if (ds > 0.85f)
							ds = 0.85f;
					}
					else ds = 0.1f;
					
					dxg.FillRect(i * rectS-camPos.x, j * rectS-camPos.y, rectS, rectS, EColor(100 * ds, 200 * ds, 140 * ds));
				}
			}

		

		dxg.FillRect(playerCenter -camPos, playerSize, EColor(20, 100, 30));

		for (int i = 0; i < blocks.size(); i++)
		{
			dxg.FillRect(blocks[i].pos-camPos, blocks[i].size, EColor(255, 0, 0));
		}
		for (int b = 0; b < blockedLight.size(); b++)
		{
			//dxg.FillRect(blockedLightPts1[b] - camPos, Vec2d(10), EColor(255, 255, 255));
			//dxg.FillRect(blockedLightPts2[b] - camPos, Vec2d(10), EColor(255, 255, 0));

			//dxg.DrawText((blockedLightPts1[b] - camPos).x, (blockedLightPts1[b] - camPos).y	,std::to_string(blockedLight[b].x), EColor(255, 255, 255));
			//dxg.DrawText((blockedLightPts2[b] - camPos).x, (blockedLightPts2[b] - camPos).y, std::to_string(blockedLight[b].y), EColor(255, 255, 255));
		
		}
		//dxg.DrawText(input.mouseX, input.mouseY+20, std::to_string(Vec2d(1,0)*(mPos-(playerCenterCam - camPos)).Normalized()), EColor(255, 255, 255));

		

	}


};
/*Game instance;

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{

	instance.Begin(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return instance.outCode;
}*/