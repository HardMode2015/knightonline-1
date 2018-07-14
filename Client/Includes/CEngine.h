//-----------------------------------------------------------------------------
// File: CGameApp.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CENGINE_H_
#define _CENGINE_H_

#include "Main.h"
#include "CMesh.h"
#include "CTimer.h"
#include "CTerrainManager.h"
#include "Scene/SceneBase.h"
#include "Scene/SceneGame.h"

enum VERTEXPROCESSING_TYPE
{
	SOFTWARE_VP = 1,
	MIXED_VP = 2,
	HARDWARE_VP = 3,
	PURE_HARDWARE_VP = 4
};

class CEngine
{
public:

	CEngine();
	virtual ~CEngine();

	void InitEngine(HINSTANCE hWndInstance, LPTSTR sCmdLine);
	void CreateMainWindow(HINSTANCE hWndInstance, LPTSTR sCmdLine);
	void InitD3D();
	int StartGame();
	void AdvanceFrame();

	LPDIRECT3D9 m_pD3D = nullptr;
	LPDIRECT3DDEVICE9 m_pD3DDevice = nullptr;

private:

	D3DFORMAT FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType);
	static LRESULT CALLBACK StaticWindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	D3DPRESENT_PARAMETERS m_D3DPresentParams;

	HWND m_hMainWindow = nullptr;
	ULONG m_nViewX;
	ULONG m_nViewY;
	ULONG m_nViewWidth;
	ULONG m_nViewHeight;

	CTimer m_Timer;

	bool m_bActive = true;
	bool m_bLostDevice = false;

	HCURSOR m_hCursorDefault;
	HCURSOR m_hCursorDefaultClicked;
	HCURSOR m_hCursorBlue;
	HCURSOR m_hCursorBlueClicked;
	HCURSOR m_hCursorRepair;
	HCURSOR m_hCursorRepairClicked;
	HCURSOR m_hCursorAttack;
	HCURSOR m_hCursorWrite;
	
	SceneBase m_cCurrentScene = SceneGame();

	CTerrainManager m_cTerrainManager;
};

#endif