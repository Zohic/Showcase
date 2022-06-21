#define OLC_PGE_APPLICATION
#include "Zohich3DEngine.h"

#include "SFML/Audio.hpp"

float aov = PI / 1.7f;

struct Slope
{
	AABB boxCollider;
	Pnt3 toUp;
	float ang;
	float height;
	int direct;
	Pnt3 begin;
	Pnt3 beginSize;
	Slope(Pnt3 pos, Pnt3 size, int iang)
	{
		Pnt3 nPs;
		if (iang == 0)
		{
			nPs = Pnt3(pos.x, pos.y, pos.z);
			boxCollider = AABB(nPs, size, true);
			toUp = Pnt3(0, 0, 1);
			ang = 0;
		}
		else if (iang == 1)
		{
			nPs = Pnt3(pos.x - size.x, pos.y, pos.z);
			boxCollider = AABB(nPs, Pnt3(2*size.x-size.z,size.y,size.x), true);
			toUp = Pnt3(-1, 0, 0);
			ang = PI/2;
		}
		else if (iang == 2)
		{
			nPs = Pnt3(pos.x-size.x, pos.y, pos.z-size.z);
			boxCollider = AABB(nPs, size, true);
			toUp = Pnt3(0, 0, -1);
			ang = PI;
		}
		else if (iang == 3)
		{
			nPs = Pnt3(pos.x, pos.y, pos.z-size.z);
			boxCollider = AABB(nPs, Pnt3(size.z, size.y, size.x), true);
			toUp = Pnt3(1, 0, 0);
			ang = 3*PI/2;
		}

		
		begin = pos;
		beginSize = size;
		direct = iang;
		height = boxCollider.size.y;
	}


};




struct ComplexDMesh
{
	std::vector<DrawableMesh> dMeshes;
	Pnt3 pos;
	Pnt3 ang;
	void draw(std::vector<Triangle>& buf)
	{
		for (int i = 0; i < dMeshes.size(); i++)
		{
			dMeshes[i].pos = pos;
			dMeshes[i].ang = ang;
			drawMesh(dMeshes[i], buf);
		}
	}
};


struct Scene;

struct Interactive
{
	bool activated = false;
	bool inside = false;
	bool justEntered = false;
	AABB boxCollider;

	Interactive()
	{

	}
	Interactive(AABB c)
	{
		boxCollider = c;
	}
	virtual void onEnter()
	{

	}
	
	virtual void action(float delta)
	{
		
	}
	void checkInter(AABB& other, float delta)
	{
		justEntered = false;
		if (boxCollider.aabbCollision(other.begin, other.end))
		{
			if (!activated)
			{
				justEntered = true;
				onEnter();
			}
			activated = true;
			inside = true;
		}
		else
			inside = false;

		if (inside)
		{
			action(delta);
		}
	}
};









struct Scene
{

	
public:

	
	sf::Music backMusic;
	sf::Music backMusic2;
	sf::SoundBuffer walkSoundSandBuff;
	sf::SoundBuffer walkSoundWoodBuff;
	sf::Sound walkSoundSand;
	sf::Sound walkSoundWood;

	
	bool playable = true;

	sf::Sound* walkSound;

	std::vector<Collider*> colliders;

	std::vector<Slope> slopes;

	std::vector<DrawableMesh> objects;

	olc::PixelGameEngine* game;

	std::vector<Triangle> mainBuffer;
	std::vector<Triangle> addBuffer;
	
	std::vector<Pnt3> palms;
	std::vector<Pnt3> palmsAng;

	std::vector<Pnt3> piles;
	std::vector<Pnt3> pilesAng;
	std::vector<bool> pilesType;

	WaveVShader* waveShader;
	WaveVSShader* leaveShader;

	PerPixelLightShader* pLig = new PerPixelLightShader();
	SurfaceLightShader* surShader = new SurfaceLightShader(false);
	BorderShader* bShader = new BorderShader();
	FillTexture* fillShader = new FillTexture();
	FillTexture* fillShaderPile = new FillTexture();

	PlasticLightShader* waterShader = new PlasticLightShader();
	
	olc::Sprite* boxSpr;
	olc::Sprite* bridgeSpr;
	olc::Sprite* beachSpr;
	olc::Sprite* seaSpr;

	olc::Sprite* palmSpr;
	olc::Sprite* palmLeavesSpr;
	olc::Sprite* palmCocosSpr;
	olc::Sprite* letterSpr;
	olc::Sprite* skyBoxSpr;
	olc::Sprite* letterOpenSpr;

	Mesh palm;
	Mesh palmLeaves;
	Mesh palmCocos;
	Mesh cocosMesh;
	olc::Sprite* landSpr;
	Mesh land[3];

	Mesh cube;
	Mesh bridge;
	Mesh beach;
	Mesh plane;
	Mesh slope;
	Mesh skyBoxMesh;

	Mesh letterMesh;
	Mesh letterOpenMesh;
	DrawableMesh Dletter;
	DrawableMesh Dcocos;

	Mesh sandPile1;
	Mesh sandPile2;

	ComplexDMesh Dpalm;
	ComplexDMesh Dland;
	DrawableMesh Dpile;

	DrawableMesh skyBox;


	struct Collectable : public Interactive
	{

		DrawableMesh dMesh;
		std::string message;
		int id;
		float timer = 3.5f;
		bool activeShow = false;
		Scene* sc;
		bool used = false;
		Collectable()
		{

		}
		Collectable(Mesh* m, std::string mes)
		{
			message = mes;
			dMesh.mesh = m;
			dMesh.drawOnTop = true;
			dMesh.pos.z = 10;
		}
		void onEnter() override
		{
			
			std::cout << "gocha";
			
		}

		void action(float delta) override
		{
			if (!used)
			{
				sc->game->DrawString(sc->game->ScreenWidth() / 2, sc->game->ScreenHeight() / 2, "press E to collect",olc::WHITE, 2);
				if (!activeShow)
					if (sc->game->GetKey(olc::E).bPressed)
					{
						activeShow = true;
						sc->playable = false;
						used = true;
					}
			}
			
		}

		void update(float delta)
		{
			if (activeShow)
			{
				std::cout << "gocha";
				Pnt3 prevP = camPos;
				Pnt3 prevA = camAng;
				camPos = Pnt3(0);
				camAng = Pnt3(0);
				dMesh.ang.y += delta * PI / 2;
				drawMesh(dMesh, sc->addBuffer);
				camPos = prevP;
				camAng = prevA;
				timer -= delta;
				if (timer < 0)
				{
					activeShow = false;
					sc->playable = true;
				}
			}
		}

	};

	Collectable letterColl;
	Collectable cocosColl;


	struct SetSoundIntr : public Interactive
	{

	public:

		Scene* sc;
		sf::Sound* s;
		void onEnter() override
		{
		
			sc->walkSound = s;
		}
		void action(float delta) override
		{

			sc->walkSound = s;
		}
	};

	SetSoundIntr setSoundSand;
	SetSoundIntr setSoundWood;

	virtual void Load()
	{

		if (!backMusic.openFromFile("seaSounds.ogg"))
		{
			std::cout << "you";
		}
		backMusic.setLoop(true);
		if (!backMusic2.openFromFile("guitar.ogg"))
		{
			std::cout << "you";
		}
		backMusic2.setLoop(true);
		if (!walkSoundSandBuff.loadFromFile("walkSand.wav"))
		{
			std::cout << "you";
		}
		if (!walkSoundWoodBuff.loadFromFile("walkWood.wav"))
		{
			std::cout << "you";
		}

		

		walkSoundSand.setBuffer(walkSoundSandBuff);
		walkSoundWood.setBuffer(walkSoundWoodBuff);
		walkSoundWood.setVolume(50);
		walkSound = &walkSoundSand;
		backMusic.setVolume(4);
		backMusic.play();
		backMusic2.setVolume(4);
		backMusic2.play();

		setSoundSand.sc = this;
		setSoundSand.s = &walkSoundSand;
		setSoundWood.sc = this;
		setSoundWood.s = &walkSoundWood;
		
		pLig->minLigth = 0.45f;

		fillShader->offset.x = 5;
		fillShader->offset.y = 3;

		fillShaderPile->offset.x = 1;
		fillShaderPile->offset.y = 1;

		waterShader->minLight = 0;
		waterShader->shiness = 2;

		waveShader = new WaveVShader();
		waveShader->amplitude = 0.44f;
		waveShader->freqTime = 2.4f;
		//waveShader->freqXWave = 0;

		leaveShader = new WaveVSShader();
		leaveShader->amplitude = 0.08f;
		leaveShader->freqXWave = 0.28f;
		leaveShader->freqZWave = 0.28f;
		leaveShader->freqTime = 2;
		//waveShader->freqXWave = 0;

		boxSpr = new olc::Sprite("box.png");
		bridgeSpr = new olc::Sprite("bridge/bridgeSprite.png");
		beachSpr = new olc::Sprite("sandBeach/sandBeachSprite.png");
		seaSpr = new olc::Sprite("sea/seaSprite.png");
		skyBoxSpr = new olc::Sprite("skyBox/skyBoxSprite.png");

		letterSpr = new olc::Sprite("letter/letterSprite.png");
		palmSpr = new olc::Sprite("palm/palm/palmSprite.png");
		palmLeavesSpr = new olc::Sprite("palm/leaves/palmLeavesSprite.png");
		palmCocosSpr = new olc::Sprite("palm/palmCocos/palmCocosSprite.png");
		landSpr = new olc::Sprite("land/landSpr.png");
		letterOpenSpr = new olc::Sprite("letter/letterOpen.png");

		sandPile1.LoadFromObjectFile("sandPile/sandPile1.obj", true);
		sandPile1.hasTexture = true;
		sandPile1.spr = beachSpr;

		letterMesh.LoadFromObjectFile("letter/letterMesh.obj", true);
		letterMesh.hasTexture = true;
		letterMesh.spr = letterSpr;

	
		cocosMesh.LoadFromObjectFile("palm/cocos/cocos.obj", true);
		cocosMesh.hasTexture = true;
		cocosMesh.spr = palmCocosSpr;

		letterOpenMesh.LoadFromObjectFile("letter/letterOpenMesh.obj", true);
		letterOpenMesh.hasTexture = true;
		letterOpenMesh.spr = letterOpenSpr;

		sandPile2.LoadFromObjectFile("sandPile/sandPile2.obj", true);
		sandPile2.hasTexture = true;
		sandPile2.spr = beachSpr;

		skyBoxMesh.LoadFromObjectFile("skyBox/skyBoxMesh.obj", true);
		skyBoxMesh.hasTexture = true;
		skyBoxMesh.spr = skyBoxSpr;

		palm.LoadFromObjectFile("palm/palm/palmMesh.obj", true);
		palm.hasTexture = true;
		palm.spr = palmSpr;

		palmLeaves.LoadFromObjectFile("palm/leaves/palmLeavesMesh.obj", true);
		palmLeaves.hasTexture = true;
		palmLeaves.spr = palmLeavesSpr;
		

		palmCocos.LoadFromObjectFile("palm/palmCocos/palmCocosMesh.obj", true);
		palmCocos.hasTexture = true;
		palmCocos.spr = palmCocosSpr;

		land[0].LoadFromObjectFile("land/land1Mesh.obj", true);
		land[0].hasTexture = true;
		land[0].spr = landSpr;
		land[0].shader.push_back(surShader);
		land[0].shaderAccuracy = 3;

		land[1].LoadFromObjectFile("land/land2Mesh.obj", true);
		land[1].hasTexture = true;
		land[1].spr = landSpr;
		land[1].shader.push_back(surShader);
		land[1].shaderAccuracy = 3;

		land[2].LoadFromObjectFile("land/land3Mesh.obj", true);
		land[2].hasTexture = true;
		land[2].spr = landSpr;
		land[2].shader.push_back(surShader);
		land[2].shaderAccuracy = 3;

		cube.LoadFromObjectFile("cubeMeshUV.obj", true);
		cube.hasTexture = true;
		cube.spr = boxSpr;

		letterColl = Collectable(&letterOpenMesh, "eeeee");
		letterColl.sc = this;
		

		cocosColl = Collectable(&cocosMesh, "eeeee");
		cocosColl.sc = this;
		cocosColl.dMesh.scale = Pnt3(2.3f);

		slope.LoadFromObjectFile("slope.obj", true);
		slope.hasTexture = true;
		slope.spr = boxSpr;


		bridge.LoadFromObjectFile("bridge/bridgeMesh.obj", true);
		bridge.hasTexture = true;
		bridge.spr = bridgeSpr;


		beach.LoadFromObjectFile("sandBeach/sandBeachMesh.obj", true);
		beach.hasTexture = true;
		beach.spr = beachSpr;


		plane.LoadFromObjectFile("sea/dividedPlane.obj", true);
		plane.hasTexture = true;
		plane.spr = seaSpr;
		plane.color = olc::Pixel(0,170,140);


		sceneLights.push_back(LightSoucrce(0, Pnt3(0, -1.5f, -1)));
		sceneLights[0].power = 1.5f;

		skyBox.mesh = &skyBoxMesh;
		skyBox.drawBackFaces = true;
		skyBox.pos = Pnt3(0, 0, 0);
		skyBox.rot = Pnt3(0.5f);
		skyBox.scale = Pnt3(300);

		Dletter.mesh = &letterMesh;

		Dcocos.mesh = &cocosMesh;
		
		DrawableMesh bri;
		bri.mesh = &bridge;
		bri.shader.push_back(pLig);
		bri.useBaseShader = false;
		bri.shaderAccuracy = 2;

		DrawableMesh beachD;
		beachD.mesh = &beach;
		beachD.drawBackFaces = false;
		beachD.useBaseShader = false;
		beachD.shader.push_back(fillShader);
		beachD.shader.push_back(pLig);
		beachD.shaderAccuracy = 6;


		Dpile.shader.push_back(pLig);
		Dpile.shaderAccuracy = 10;
		Dpile.useBaseShader = false;
		Dpile.rot = Pnt3(0, -1.3f, 0);

		DrawableMesh sea[6];
		for (int i = 0; i < 6; i++)
		{
			sea[i].pos.y = 10;
			sea[i].pos.z = 160;
			sea[i].ang.z = PI;
			sea[i].mesh = &plane;
			sea[i].useBaseShader = false;
			sea[i].shaderAccuracy = 12;
			sea[i].shader.push_back(waterShader);
			waveShader->timer = 1;
			sea[i].vertexSahder.push_back(waveShader);
			sea[i].scale = Pnt3(10);
		}
		sea[5].pos.x = 260;
		sea[5].pos.z = 260;

		sea[4].pos.x = 180;
		sea[4].pos.z = 240;

		//sea[2].pos.x = -100;
		
		//sea[3].pos.x = -300;
		//sea[3].pos.z = 300;






		DrawableMesh palmO;
		palmO.mesh = &palm;
		palmO.scale = Pnt3(1.5f);
		palmO.rot = Pnt3(-14, 1.8f, -7.4f);

		DrawableMesh palmL;
		palmL.mesh = &palmLeaves;
		palmL.scale = Pnt3(1.5f);
		palmL.rot = Pnt3(-14, 1.8f, -7.4f);
		palmL.vertexSahder.push_back(leaveShader);

		DrawableMesh palmC;
		palmC.mesh = &palmCocos;
		palmC.scale = Pnt3(1.5f);
		palmC.rot = Pnt3(-14, 1.8f, -7.4f);

		DrawableMesh land1;
		land1.mesh = &land[0];
		DrawableMesh land2;
		land2.mesh = &land[1];
		DrawableMesh land3;
		land3.mesh = &land[2];



		Dpalm.dMeshes.push_back(palmO);
		Dpalm.dMeshes.push_back(palmL);
		Dpalm.dMeshes.push_back(palmC);


		Dland.dMeshes.push_back(land1);
		Dland.dMeshes.push_back(land2);
		Dland.dMeshes.push_back(land3);
		//Dland.pos.z = -2;
		objects.push_back(bri);
		objects.push_back(beachD);
		objects.push_back(sea[0]);
		objects.push_back(sea[1]);
		objects.push_back(sea[2]);
		objects.push_back(sea[3]);
		objects.push_back(sea[4]);
		objects.push_back(sea[5]);

		loadFromFile("testScene.tasc");


	}

	virtual void Show(float delta, bool showColliders, bool showSlopeColl)
	{
		mainBuffer.clear();
		addBuffer.clear();
		DrawableMesh dms;
		dms.shader.push_back(bShader);
		dms.useBaseShader = false;
		dms.rot = Pnt3(1);
		dms.mesh = &slope;
		dms.shaderAccuracy = 2;
		DrawableMesh dm;
		dm.shader.push_back(bShader);
		dm.useBaseShader = false;
		dm.rot = Pnt3(1);
		dm.mesh = &cube;
		//objects[5].pos.x += 10 * delta;
		//objects[5].pos.z += 10 * delta;

		

		if (showColliders)
			for (int i = 0; i < slopes.size(); i++)
			{

				dms.pos = slopes[i].begin;
				dms.scale = slopes[i].boxCollider.size * 0.5f;
				dms.ang.y = slopes[i].ang;
				drawMesh(dms, mainBuffer);
				if (showSlopeColl)
				{
					dm.pos = slopes[i].boxCollider.begin;
					dm.scale = slopes[i].boxCollider.size * 0.5f;
					drawMesh(dm, mainBuffer);
				}
			}

		for (int i = 0; i < palms.size(); i++)
		{
			Dpalm.pos = palms[i];
			Dpalm.ang = palmsAng[i];
			Dpalm.draw(mainBuffer);
		}	

		letterColl.update(delta);
		cocosColl.update(delta);

		for (int i = 0; i < piles.size(); i++)
		{
			Dpile.pos = piles[i];
			Dpile.ang = pilesAng[i];

			if (pilesType[i])
				Dpile.mesh = &sandPile1;
			else Dpile.mesh = &sandPile2;

			drawMesh(Dpile, mainBuffer);
		}


		drawMesh(Dcocos, mainBuffer);
		drawMesh(Dletter, mainBuffer);
		Dland.draw(mainBuffer);

		waveShader->timer -= delta;
		leaveShader->timer += delta;

		skyBox.pos = camPos;

		drawMesh(skyBox, mainBuffer);

		for (int i = 0; i < objects.size(); i++)
		{
			drawMesh(objects[i], mainBuffer);
		}
		
		
		
		

		if (showColliders)
		{

			
			for (int i = 0; i < colliders.size(); i++)
			{
				dm.pos = colliders[i]->getPosition();
				dm.scale = colliders[i]->getSize() * 0.5f;

				drawMesh(dm, mainBuffer);
			}

		}

		drawBuffer(mainBuffer, *game);
		drawBuffer(addBuffer, *game);

	}
	void loadFromFile(std::string fileName)
	{
		colliders.clear();
		slopes.clear();
		palms.clear();
		palmsAng.clear();
		piles.clear();
		pilesAng.clear();
		pilesType.clear();

		std::ifstream fin;
		fin.open(fileName);

		AABB newCollider;
		
		int counterB;
		if (fin.is_open())
		{

			std::string type;

			while (!fin.eof())
			{
				type = " ";
				fin >> type;

				if (type == "C")
				{
					fin >> newCollider.begin.x;
					fin >> newCollider.begin.y;
					fin >> newCollider.begin.z;
					fin >> newCollider.size.x;
					fin >> newCollider.size.y;
					fin >> newCollider.size.z;

					newCollider.end.x = newCollider.end.x + newCollider.size.x;
					newCollider.end.y = newCollider.end.y + newCollider.size.y;
					newCollider.end.z = newCollider.end.z + newCollider.size.z;
					colliders.push_back(new AABB(newCollider.getPosition(), newCollider.getSize(), true));
				}
				if (type == "S")
				{
					Pnt3 pS;
					Pnt3 sZ;
					int dR;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;

					fin >> sZ.x;
					fin >> sZ.y;
					fin >> sZ.z;

					fin >> dR;

					slopes.push_back(Slope(pS,sZ,dR));
				}
				if (type == "Palm")
				{
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					palms.push_back(pS);
					palmsAng.push_back(Pnt3(0,-(PI/2)+(float)(rand()%10)/10 * (PI) ,0));
				}
				if (type == "Pile")
				{
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					piles.push_back(pS);
					pilesAng.push_back(Pnt3(0, (float)(rand() % 15)/15 * 2 * (PI), 0));
					pilesType.push_back((rand()%2 == 1));
				}
				if (type == "setSand")
				{
					Pnt3 pP;
					fin >> pP.x;
					fin >> pP.y;
					fin >> pP.z;
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					setSoundSand.boxCollider = AABB(pP,pS,true);
				}
				if (type == "setWood")
				{
					Pnt3 pP;
					fin >> pP.x;
					fin >> pP.y;
					fin >> pP.z;
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					setSoundWood.boxCollider = AABB(pP, pS, true);
				}

				if (type == "letterPos")
				{
					Pnt3 pP;
					fin >> pP.x;
					fin >> pP.y;
					fin >> pP.z;
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					letterColl.boxCollider = AABB(pP, pS, true);
				}
				if (type == "cocosPos")
				{
					Pnt3 pP;
					fin >> pP.x;
					fin >> pP.y;
					fin >> pP.z;
					Dcocos.pos = pP;
				}
				if (type == "cocosColl")
				{
					Pnt3 pP;
					fin >> pP.x;
					fin >> pP.y;
					fin >> pP.z;
					Pnt3 pS;
					fin >> pS.x;
					fin >> pS.y;
					fin >> pS.z;
					cocosColl.boxCollider = AABB(pP, pS, true);
				}
			}

		}
		else
		{
			std::cout << "error with load scene from file";
		}

	}

	void saveToFile(std::string fileName)
	{
		std::ofstream fout;
		fout.open(fileName);
		if (fout.is_open())
		{
			for (int i = 0; i < colliders.size(); i++)
			{
				fout << "C " + std::to_string(colliders[i]->getPosition().x) + " " + std::to_string(colliders[i]->getPosition().y) + " " + std::to_string(colliders[i]->getPosition().z) +
					" " + std::to_string(colliders[i]->getSize().x) + " " + std::to_string(colliders[i]->getSize().y) + " " + std::to_string(colliders[i]->getSize().z) + " \n";
			}
			for (int i = 0; i < slopes.size(); i++)
			{
				fout << "S " + std::to_string(slopes[i].begin.x) + " " + std::to_string(slopes[i].begin.y) + " " + std::to_string(slopes[i].begin.z) +
					" " + std::to_string(slopes[i].beginSize.x) + " " + std::to_string(slopes[i].beginSize.y) + " " + std::to_string(slopes[i].beginSize.z) + " " + std::to_string(slopes[i].direct) + " \n";
			}
			for (int i = 0; i < palms.size(); i++)
			{
				fout << "Palm " + std::to_string(palms[i].x) + " " + std::to_string(palms[i].y) + " " + std::to_string(palms[i].z) + " \n";
			}

			for (int i = 0; i < piles.size(); i++)
			{
				fout << "Pile " + std::to_string(piles[i].x) + " " + std::to_string(piles[i].y) + " " + std::to_string(piles[i].z) + " \n";
			}
			
			fout << "setSand " + std::to_string(setSoundSand.boxCollider.begin.x) + " " + std::to_string(setSoundSand.boxCollider.begin.y) + " " + std::to_string(setSoundSand.boxCollider.begin.z) + " ";
			fout << std::to_string(setSoundSand.boxCollider.size.x) + " " + std::to_string(setSoundSand.boxCollider.size.y) + " " + std::to_string(setSoundSand.boxCollider.size.z) + " \n";

			fout << "setWood " + std::to_string(setSoundWood.boxCollider.begin.x) + " " + std::to_string(setSoundWood.boxCollider.begin.y) + " " + std::to_string(setSoundWood.boxCollider.begin.z) + " ";
			fout << std::to_string(setSoundWood.boxCollider.size.x) + " " + std::to_string(setSoundWood.boxCollider.size.y) + " " + std::to_string(setSoundWood.boxCollider.size.z) + " \n";

			fout << "letterPos " + std::to_string(letterColl.boxCollider.begin.x) + " " + std::to_string(letterColl.boxCollider.begin.y) + " " + std::to_string(letterColl.boxCollider.begin.z) + " ";
			fout << std::to_string(letterColl.boxCollider.size.x) + " " + std::to_string(letterColl.boxCollider.size.y) + " " + std::to_string(letterColl.boxCollider.size.z) + " \n";
			
			fout << "cocosPos " + std::to_string(Dcocos.pos.x) + " " + std::to_string(Dcocos.pos.y) + " " + std::to_string(Dcocos.pos.z) + " \n";

			fout << "cocosColl " + std::to_string(cocosColl.boxCollider.begin.x) + " " + std::to_string(cocosColl.boxCollider.begin.y) + " " + std::to_string(cocosColl.boxCollider.begin.z) + " ";
			fout << std::to_string(cocosColl.boxCollider.size.x) + " " + std::to_string(cocosColl.boxCollider.size.y) + " " + std::to_string(cocosColl.boxCollider.size.z) + " \n";

		}
		else
		{
			std::cout << "error with saving scene file";
		}
	}

};



struct Player
{
public:
	Pnt3 position;
	Pnt3 colliderOffset;
	Pnt3 cameraOffset;

	AABB collider;
	Pnt3 moveVector = Pnt3(0, 0, 0);
	float moveSpeed = 14;
	float jumpForce = -20;
	int xInp = 0;
	int zInp = 0;
	bool jumpInp;

	bool landed = false;
	float gravity = 30;
	olc::PixelGameEngine* game;
	Scene* scene;

	void updateInputs()
	{
		xInp = 0;
		zInp = 0;
		jumpInp = false;

		if (scene->playable)
		{
			if (game->GetKey(olc::W).bHeld)
			{
				zInp = 1;
			}

			if (game->GetKey(olc::S).bHeld)
			{
				zInp = -1;
			}

			if (game->GetKey(olc::A).bHeld)
			{
				xInp = -1;
			}

			if (game->GetKey(olc::D).bHeld)
			{
				xInp = 1;
			}

			if (game->GetKey(olc::SPACE).bPressed)
			{
				jumpInp = true;

			}
		}
	}

	void SetPosition(Pnt3 pos)
	{
		position = pos;
		collider.setBegin(position + colliderOffset);
	}

	void Move(float delta)
	{

		collider.setBegin(position+colliderOffset);

		

		moveVector.x = moveSpeed * xInp;
		moveVector.z = moveSpeed * zInp;
		moveVector.y += gravity * delta;

		moveVector = rotateY(moveVector, -camAng.y);

		if (moveVector.x != 0 & moveVector.z != 0)
		{
			if(scene->walkSound->getStatus()!=sf::Sound::Playing)
				scene->walkSound->play();
		}
		else
		{
			scene->walkSound->pause();
		}

		if (jumpInp)
		{
			if (landed)
			{
				moveVector.y = jumpForce;
				landed = false;
			}
		}

		AABB nextCollider = collider;
		nextCollider.moveBegin(moveVector*delta);

		std::vector<AABB> slopeColliders;

		bool onSlope = false;


		auto getSlopeCollider = [&](Slope sl)
		{
			float upCoof = DotProduct(position+moveVector*delta - sl.begin, sl.toUp) / sl.boxCollider.size.z;
			return AABB(
				Pnt3(sl.boxCollider.begin.x, sl.begin.y + (1 - upCoof) * sl.height, sl.boxCollider.begin.z),
				Pnt3(sl.boxCollider.size.x, sl.boxCollider.size.y * upCoof, sl.boxCollider.size.z),
				true);
		};


		for (int i = 0; i < scene->slopes.size(); i++)
		{
			if (scene->slopes[i].boxCollider.aabbCollision(nextCollider.begin, nextCollider.end))
			{
				slopeColliders.push_back(getSlopeCollider(scene->slopes[i]));
				onSlope = true;
				break;
			}
		}

		bool gotIt = false;

		nextCollider = collider;
		nextCollider.moveBegin(Pnt3(moveVector.x * delta, 0, 0));

		for (int i = 0; i < slopeColliders.size(); i++)
		{
			if (slopeColliders[i].aabbCollision(nextCollider.begin, nextCollider.end)){
				
				
				position.y = slopeColliders[i].begin.y - collider.size.y;

				gotIt = true;
				break;
			}
		}
		
		for (int i = 0; i < scene->colliders.size(); i++)
		{
			if (scene->colliders[i]->aabbCollision(nextCollider.begin, nextCollider.end))
			{
				float yo = position.y + collider.size.y - scene->colliders[i]->getPosition().y;
				if ( yo < 0.4f && yo >=0)
				{
					position.y = scene->colliders[i]->getPosition().y - collider.size.y;

				}
				else
				{
					moveVector.x = 0;
					break;
				}
					
			}
		}

		gotIt = false;
		nextCollider = collider;
		nextCollider.moveBegin(Pnt3(0, 0, moveVector.z * delta));

		for (int i = 0; i < slopeColliders.size(); i++)
		{
			if (slopeColliders[i].aabbCollision(nextCollider.begin, nextCollider.end))
			{
				
				position.y = slopeColliders[i].begin.y - collider.size.y;

				gotIt = true;
				break;
			}
		}
		
		for (int i = 0; i < scene->colliders.size(); i++)
		{
			if (scene->colliders[i]->aabbCollision(nextCollider.begin, nextCollider.end))
			{
				float yo = position.y + collider.size.y - scene->colliders[i]->getPosition().y;
				if ( yo < 0.4f && yo >=0)
				{
					position.y = scene->colliders[i]->getPosition().y - collider.size.y;

				}
				else
				{
					moveVector.z = 0;
					break;
				}
					
				
			}
		}

		gotIt = false;
		landed = false;
		nextCollider = collider;
		nextCollider.moveBegin(Pnt3(0, moveVector.y * delta, 0));

		for (int i = 0; i < slopeColliders.size(); i++)
		{
			if (slopeColliders[i].aabbCollision(nextCollider.begin, nextCollider.end))
			{
				if (moveVector.y > 0)
					landed = true;

				moveVector.y = 0;
				break;
			}
		}
		if(!gotIt)
		for (int i = 0; i < scene->colliders.size(); i++)
		{
			if (scene->colliders[i]->aabbCollision(nextCollider.begin, nextCollider.end))
			{
				if (moveVector.y > 0)
					landed = true;

				moveVector.y = 0;
				break;
			}
		}
		

		position += moveVector * delta;
		camPos = position + cameraOffset;
	}


};


class Adventure :public olc::PixelGameEngine
{

	Pnt2 prevmouse;
	

	
	Player player;
	Scene testScene;

	//for edit mode
	bool editMode = false;
	float camSpd = 15;

	float colldierPlaceDistance = 10;
	bool placingCollider = false;
	bool placingObj = true;
	
	Pnt3 newColSize = Pnt3(1);
	int slopeDir = 0;
	AABB newCollider = AABB(Pnt3(0), Pnt3(1), true);
	Slope newSlope = Slope(Pnt3(),Pnt3(),0);
	olc::Sprite* boxSprite;
	Mesh cube;
	Mesh slope;
	DrawableMesh dm;
	DrawableMesh dms;
	BorderShader* bShader = new BorderShader();

	std::vector<Triangle> collidersBuffer;

	bool slopeMode = false;
	bool slopeCollShow = false;

	

public:
	Adventure()
	{
		sAppName = "TentAdventure";
	}
	bool OnUserCreate() override
	{
		setPerspective(aov);
		zBuffer = new float[ScreenWidth() * ScreenHeight()];

		boxSprite = new olc::Sprite("box.png");

		cube.LoadFromObjectFile("cubeMeshUV.obj", true);
		cube.hasTexture = true;
		cube.spr = boxSprite;

		slope.LoadFromObjectFile("slope.obj", true);
		slope.hasTexture = true;
		slope.spr = boxSprite;

		dm.shader.push_back(bShader);
		dm.useBaseShader = false;
		dm.rot = Pnt3(1);
		dm.mesh = &cube;

		dms.shader.push_back(bShader);
		dms.useBaseShader = false;
		dms.rot = Pnt3(1);
		dms.mesh = &slope;


		testScene.game = this;
		testScene.Load();
		

		player.game = this;
		player.scene = &testScene;
		player.position = Pnt3(0,-10,0);
		player.collider = AABB(Pnt3(0,0,0),Pnt3(1,5,1));
		player.colliderOffset = Pnt3(0,0,0);
	
		
		return true;
	}


	bool OnUserUpdate(float delta) override
	{
		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			zBuffer[i] = INFINITY;

		Clear(olc::BLUE);


		float mx = GetMouseX();
		float my = GetMouseY();
		
		if (GetKey(olc::M).bPressed)
		{
			editMode = !editMode;
			player.moveVector.y = 0;
		}
		
		

		testScene.Show(delta, editMode, slopeCollShow);
		testScene.setSoundSand.checkInter(player.collider,delta);
		testScene.setSoundWood.checkInter(player.collider, delta);
		testScene.letterColl.checkInter(player.collider, delta);
		testScene.cocosColl.checkInter(player.collider, delta);

		if (!editMode)
		{
			player.updateInputs();
			player.Move(delta);
		}
		else
		{
			player.SetPosition(camPos);

			if (GetKey(olc::O).bPressed)
			{
				testScene.saveToFile("testScene.tasc");
			}

			if (GetKey(olc::P).bPressed)
			{
				testScene.loadFromFile("testScene.tasc");
			}

			if (GetKey(olc::C).bPressed && !placingCollider)
			{
				placingCollider = true;
			}

			if (GetKey(olc::X).bPressed)
			{
				slopeMode = !slopeMode;
			}

			if (GetKey(olc::V).bPressed)
			{
				slopeCollShow = !slopeCollShow;
			}

			if (GetKey(olc::Z).bPressed)
			{
				slopeDir += 1;
				if (slopeDir > 3)
					slopeDir = 0;
			}

			if (placingCollider)
			{
				if (GetKey(olc::UP).bHeld)
				{
					colldierPlaceDistance += delta * 6;
				}else
				if (GetKey(olc::DOWN).bHeld)
				{
					colldierPlaceDistance -= delta * 6;
				}
				
				bool xm = GetKey(olc::NP4).bHeld;
				bool xp = GetKey(olc::NP6).bHeld;
				bool ym = GetKey(olc::NP8).bHeld;
				bool yp = GetKey(olc::NP2).bHeld;
				bool zm = GetKey(olc::NP7).bHeld;
				bool zp = GetKey(olc::NP9).bHeld;
			
				newColSize+= Pnt3(1 * xp - 1 * xm, 1 * yp - 1 * ym, 1 * zp - 1 * zm)*delta*5;
				
				Pnt3 mouse3Pos;
				Pnt2 mouse2Pos = Pnt2(GetMouseX() - ScreenWidth() / 2, GetMouseY() - ScreenHeight() / 2);

				mouse3Pos = antiConvert(mouse2Pos, colldierPlaceDistance);

				mouse3Pos = rotate3d(mouse3Pos, camAng*-1 , 3);

				collidersBuffer.clear();

				if (!slopeMode)
				{
					newCollider.setBegin(camPos + mouse3Pos);
					newCollider.setSize(newColSize);
					dm.pos = newCollider.getPosition();
					dm.scale = newCollider.getSize() * 0.5f;
					drawMesh(dm, collidersBuffer);
				}
				else
				{
					newSlope = Slope(camPos + mouse3Pos, newColSize,slopeDir);
					dms.pos = newSlope.begin;
					dms.scale = newSlope.boxCollider.size * 0.5f;
					dms.ang.y = newSlope.ang;
					drawMesh(dms, collidersBuffer);
				}
				


				drawBuffer(collidersBuffer, *this);
			}
			if (GetKey(olc::SPACE).bPressed && placingCollider)
			{
				if (!placingObj)
				{
					if (!slopeMode)
					{
						testScene.colliders.push_back(new AABB(newCollider.getPosition(), newCollider.getSize(), true));
						placingCollider = false;
					}
					else
					{
						testScene.slopes.push_back(newSlope);
						placingCollider = false;
					}
				}
				else
				{
					testScene.cocosColl.boxCollider = AABB(newCollider.getPosition(), newCollider.getSize(), true);
					//testScene.Dcocos.pos = newCollider.getPosition();
				}
				
			}

			DrawString(20, 20, std::to_string(camPos.x));
			DrawString(20, 30, std::to_string(camPos.z));
			if (testScene.playable)
			{
				if (GetKey(olc::W).bHeld)
				{
					camPos.z += camSpd * cosf(camAng.y) * cosf(camAng.x) * delta;
					camPos.x += camSpd * sinf(camAng.y) * cosf(camAng.x) * delta;
					camPos.y += camSpd * sinf(camAng.x) * delta;
				}


				if (GetKey(olc::W).bHeld)
				{
					camPos.z += camSpd * cosf(camAng.y) * cosf(camAng.x) * delta;
					camPos.x += camSpd * sinf(camAng.y) * cosf(camAng.x) * delta;
					camPos.y += camSpd * sinf(camAng.x) * delta;
				}

				if (GetKey(olc::S).bHeld)
				{
					camPos.z -= camSpd * cosf(camAng.y) * cosf(camAng.x) * delta;
					camPos.x -= camSpd * sinf(camAng.y) * cosf(camAng.x) * delta;
					camPos.y -= camSpd * sinf(camAng.x) * delta;
				}

				if (GetKey(olc::A).bHeld)
				{
					camPos.z += camSpd * cosf(camAng.y - PI / 2) * delta;
					camPos.x += camSpd * sinf(camAng.y - PI / 2) * delta;
				}

				if (GetKey(olc::D).bHeld)
				{
					camPos.z -= camSpd * cosf(camAng.y - PI / 2) * delta;
					camPos.x -= camSpd * sinf(camAng.y - PI / 2) * delta;
				}

			}
			if (GetKey(olc::Q).bHeld)
			{

				zCof += delta / 100;
			}

			if (GetKey(olc::E).bHeld)
			{

				zCof -= delta / 100;
			}
		}
		
		
		if(testScene.playable)
		if (GetKey(olc::F).bHeld)
		{
			camAng.y += (mx - prevmouse.x) * delta / ScreenWidth()*200;
			camAng.x += (my - prevmouse.y) * delta / ScreenHeight()*200;
			if (camAng.x > PI / 2.5f)
				camAng.x = PI / 2.5f;
			if (camAng.x < -PI / 2.5f)
				camAng.x = -PI / 2.5f;
		}
			
		DrawString(20, 20, "find the cocos and letter");
		prevmouse.x = mx;
		prevmouse.y = my;

		return true;
	}
};

int main()
{
	Adventure game;

	if (game.Construct(640, 480, 1,1 ,false ,false))
		game.Start();
	return 0;
}