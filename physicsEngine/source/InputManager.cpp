#include "InputManager.h"

void InputManager::InitKeys()
{
	keyStates.resize(53);
}

int InputManager::GetIndex(char s)
{
	if (s > 47)
	{
		if (s >= 97)
		{
			return s - 97;
		}
		else if (s >= 65)
		{
			return s - 65;
		}else if (s >= 48)
		{
			return -22 + s;
		}


		
	}
	else
	{
		switch (s)
		{
		case 0x25:
			return sLEFT;
			break;
		case 0x26:
			return sUP;
			break;
		case 0x27:
			return sRIGHT;
			break;
		case 0x28:
			return sDOWN;
			break;
		}

		switch (s)
		{
		case 0x12:
			return sALT;
			break;
		case 0x11:
			return sCTRL;
			break;
		case 0x0D:
			return sENTR;
			break;
		case 0x08:
			return sBCKS;
			break;
		case 0x20:
			return sSPACE;
			break;
		case 0x10:
			return sSHIFT;
			break;
		case 0x09:
			return sTAB;
			break;
		case 0x1B:
			return sESC;
			break;
		}
	}

	return -1;
}


void InputManager::ResetKeys()
{
	for (int reset = 0; reset < 53; reset++)
	{
		keyStates[reset].pressed = false;
		keyStates[reset].released = false;
	}
	mouseMoved = false;
	wheelDelta = 0;
}

void InputManager::SetKeyPressed(int id)
{
	keyStates[id].pressed = true;
	keyStates[id].down = true;
}

void InputManager::SetKeyReleased(int id)
{
	keyStates[id].down = false;
	keyStates[id].released = true;
}

void InputManager::UpdateMousePosition(int x, int y)
{
	mouseX = x;
	mouseY = y;
}
