#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================
#include <dinput.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "GameState.h"
#include "RenderTarget.h"
#include "objectManager.h"

class Grid2D;

using namespace DirectX;

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState
	RenderTarget* m_RD;
	RenderTarget* m_SecRD;
	ObjectsManager* m_OM;
	//player input
	unsigned char* m_keyboardState; //current state of the Keyboard
	unsigned char* m_prevKeyboardState; //previous frame's state of the keyboard
	DIMOUSESTATE* m_mouseState; //current state of the mouse
	int m_GameArray[50][50];
	int m_waterArray[50][50];
	UINT width;
	UINT height;
	int m_mouseX = 0, m_mouseY = 0;
	int m_maxRow = 0, m_maxCol = 0;
	int _size = 50;
	Grid2D* grid;
	//void TW_CALL Callback(grid->run());

};
#endif
