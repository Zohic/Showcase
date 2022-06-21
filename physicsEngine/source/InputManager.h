#pragma once
#include <vector>

enum Keys
{
	//keyboard - 36
	KA, KB, KC, KD, KE, KF, KG, 
	KH, KI, KJ, KK, KL, KM, KN, 
	KO, KP, KQ, KR, KS, KT, KU, 
	KV, KW, KX, KY, KZ, D0, D1, 
	D2, D3, D4, D5, D6, D7, D8, 
	D9,
	//mouse - 3
	BL,BR,BM,
	//sys keys - 12
	sALT,sCTRL,sENTR,sBCKS,sSPACE,sSHIFT,sTAB,sESC,
	sUP,sDOWN,sLEFT,sRIGHT,
	//any key - 2
	kANY, kANYM
};

struct InputManager
{
	struct KeyState
	{
		bool pressed = false;
		bool down = false;
		bool released = false;
	};

private:
	const char* symbolsList = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

public:
	std::vector<KeyState> keyStates;
	int mouseX;
	int mouseY;
	int wheelDelta;
	bool mouseMoved = false;
	void InitKeys();
	int GetIndex(char s);
	void ResetKeys();
	void SetKeyPressed(int id);
	void SetKeyReleased(int id);
	void UpdateMousePosition(int x, int y);
};