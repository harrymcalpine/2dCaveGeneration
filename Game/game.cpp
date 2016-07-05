#include "game.h"
#include "GameData.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include "gameobject.h"
#include "ObjectList.h"
#include "helper.h"
#include <windows.h>
#include <time.h>
#include "DDSTextureLoader.h"
#include <d3d11shader.h>

#include "Grid2D.h"
#include "RenderTarget.h"
#include <d3d11.h>
#include <AntTweakBar.h>
#include <iostream>

using namespace DirectX;

Game::Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance) :m_playTime(0), m_fxFactory(nullptr), m_states(nullptr)
{
	//Create DirectXTK spritebatch stuff
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));

	//seed the random number generator
	srand((UINT)time(NULL));

	//Direct Input Stuff
	m_hWnd = _hWnd;
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;
	m_pMouse = nullptr;

	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//create GameData struct and populate its pointers
	m_GD = new GameData;
	m_GD->m_keyboardState = m_keyboardState;
	m_GD->m_prevKeyboardState = m_prevKeyboardState;
	m_GD->m_GS = GS_PLAY_MAIN_CAM;
	m_GD->m_mouseState = &m_mouseState;

	//set up DirectXTK Effects system
	m_fxFactory = new EffectFactory(_pd3dDevice);

	//Tell the fxFactory to look to the correct build directory to pull stuff in from
#ifdef DEBUG
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Release");
#endif

	// Create other render resources here
	m_states = new CommonStates(_pd3dDevice);

	//init render system for VBGOs
	VBGO::Init(_pd3dDevice);

	//find how big my window is to correctly calculate my aspect ratio
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_GD->width = rc.right - rc.left;
	m_GD->height = rc.bottom - rc.top;
	float AR = (float)m_GD->width / (float)m_GD->height;

	TwInit(TW_DIRECT3D11, _pd3dDevice);
	TwWindowSize(m_GD->width, m_GD->height);

	//create a base camera
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3(0.0f, 0.0f, -1000.0f));
	m_cam->SetPos(Vector3(0.0f, 0.0f, 100.0f));
	m_GameObjects.push_back(m_cam);

	//create a base light
	m_light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_light);

	//create DrawData struct and populate its pointers
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;
	m_DD->m_states = m_states;
	m_DD->m_cam = m_cam;
	m_DD->m_light = m_light;

	//add a player
	Player* pPlayer = new Player("BirdModelV1.cmo", _pd3dDevice, m_fxFactory);
	m_GameObjects.push_back(pPlayer);

	//add a secondary camera
	m_TPScam = new TPSCamera(0.25f * XM_PI, AR, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 10.0f, 50.0f));
	m_GameObjects.push_back(m_TPScam);

	srand(time(0));

	//RENDER TARGET INFORMATION AND THE PLANE IT IS LOADED ONTO
	m_RD = new RenderTarget(_pd3dDevice, m_GD->width, m_GD->height);
	m_GD->m_RD = m_RD;
	m_SecRD = new RenderTarget(_pd3dDevice, m_GD->width, m_GD->height);
	m_GD->m_SecRD = m_SecRD;

	VBPlane* LBox = new VBPlane();
	LBox->init(100, _pd3dDevice, m_RD->GetShaderResourceView());
	LBox->SetPos(Vector3(0.0f, 0.0f, -50.0f));
	LBox->SetYaw(270.0f);
	LBox->SetScale(1.0f);
	m_GameObjects.push_back(LBox);

	VBPlane* TBox = new VBPlane();
	TBox->init(100, _pd3dDevice, m_SecRD->GetShaderResourceView());
	TBox->SetPos(Vector3(0.0f, 0.0f, -50.0f));
	TBox->SetYaw(270.0f);
	TBox->SetScale(1.0f);
	m_GameObjects.push_back(TBox);

	m_GD->m_maxRow = 50;
	m_GD->m_maxCol = 50;

	ObjectsManager* m_OM = new ObjectsManager(_pd3dDevice, m_GD->m_maxRow, m_GD->m_maxCol);
	m_GD->m_OM = m_OM;

	Grid2D* cellGrid2D = new Grid2D();
	cellGrid2D->run(m_GD);
}

void TW_CALL RunCB(void * /*clientData*/)
{
	std::cout << "Clicked";
}

Game::~Game()
{
	//delete Game Data & Draw Data
	delete m_GD;
	delete m_DD;

	//tidy up VBGO render system
	VBGO::CleanUp();

	//tidy away Direct Input Stuff
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
	}

	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();

	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}
	m_GameObject2Ds.clear();

	for (list<GameObject2D *>::iterator it = m_2DToPlane.begin(); it != m_2DToPlane.end(); it++)
	{
		delete (*it);
	}
	m_2DToPlane.clear();

	//clear away CMO render system
	delete m_states;
	delete m_fxFactory;

	delete m_DD2D;
}

bool Game::Update()
{
	//Poll Keyboard & Mouse
	ReadInput();

	//Upon pressing escape QUIT
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}

	//lock the cursor to the centre of the window
	RECT window;
	GetWindowRect(m_hWnd, &window);
	//SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);

	//upon space bar switch camera state
	if ((m_keyboardState[DIK_SPACE] & 0x80) && !(m_prevKeyboardState[DIK_SPACE] & 0x80))
	{
		if (m_GD->m_GS == GS_PLAY_MAIN_CAM)
		{
			m_GD->m_GS = GS_PLAY_TPS_CAM;
		}
		else
		{
			m_GD->m_GS = GS_PLAY_MAIN_CAM;
		}
	}

	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->m_dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;

	//update all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
	for (list<GameObject2D *>::iterator it = m_2DToPlane.begin(); it != m_2DToPlane.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
	return true;
}

void Game::Render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	//set immediate context of the graphics device
	m_DD->m_pd3dImmediateContext = _pd3dImmediateContext;

	//set which camera to be used
	m_DD->m_cam = m_cam;
	if (m_GD->m_GS == GS_PLAY_TPS_CAM)
	{
		m_DD->m_cam = m_TPScam;
	}

	//update the constant buffer for the rendering of VBGOs
	VBGO::UpdateConstantBuffer(m_DD);

	//draw all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}

	// Draw sprite batch stuff
	m_DD2D->m_Sprites->Begin();
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}
	m_DD2D->m_Sprites->End();

	m_RD->ClearRenderTarget(_pd3dImmediateContext, 0.0f, 0.0f, 0.0f, 0.0f);
	if (firstPass == true)
	{
		if (m_RD->GetMapped())
		{
			m_RD->Unmap(m_DD->m_pd3dImmediateContext);
		}
		// Draw sprite batch stuff
		m_DD2D->m_Sprites->Begin();
		m_RD->Begin(m_DD->m_pd3dImmediateContext, false);
		m_GD->m_OM->AddWall(m_DD2D);
		m_GD->m_OM->AddDeepWall(m_DD2D);
		m_GD->m_OM->AddFloor(m_DD2D);
		for (list<GameObject2D *>::iterator it = m_2DToPlane.begin(); it != m_2DToPlane.end(); it++)
		{
			(*it)->Draw(m_DD2D);
		}
		ImageGO2D* terrain = new ImageGO2D(m_RD->GetShaderResourceView());
		m_GameObject2Ds.push_back(terrain);
		m_DD2D->m_Sprites->End();
		m_RD->End(m_DD->m_pd3dImmediateContext);
		m_RD->Map(m_DD->m_pd3dImmediateContext);
		firstPass = false;
	}

	if (m_SecRD->GetMapped())
	{
		m_SecRD->Unmap(m_DD->m_pd3dImmediateContext);
	}
	// Draw sprite batch stuff
	m_DD2D->m_Sprites->Begin();
	m_SecRD->Begin(m_DD->m_pd3dImmediateContext, false);

	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}
	m_GD->m_OM->AddWater(m_DD2D);
	m_DD2D->m_Sprites->End();
	m_SecRD->End(m_DD->m_pd3dImmediateContext);
	m_SecRD->Map(m_DD->m_pd3dImmediateContext);

	TwDraw();  // draw the tweak bar(s)

	//drawing text screws up the Depth Stencil State, this puts it back again!
	_pd3dImmediateContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
}

bool Game::ReadInput()
{
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(unsigned char) * 256);

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(unsigned char) * 256);
	ZeroMemory(&m_mouseState, sizeof(DIMOUSESTATE));

	// Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// Read the Mouse device.
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(hr))
	{
		// If the Mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}