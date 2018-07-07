//-----------------------------------------------------------------------------
// File: CGameApp.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

#include "Main.h"
#include "CTimer.h"
#include "CObject.h"

enum VERTEXPROCESSING_TYPE
{
	SOFTWARE_VP = 1,
	MIXED_VP = 2,
	HARDWARE_VP = 3,
	PURE_HARDWARE_VP = 4
};

class CGameApp
{
public:
	CGameApp();
	virtual ~CGameApp();

    LRESULT     DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	bool        InitInstance(HANDLE hInstance, LPCTSTR lpCmdLine, int iCmdShow);
    int         BeginGame();
	bool        ShutDown();
	
private:
    bool        BuildObjects();
	bool		ReleaseObjects();
    void        FrameAdvance();
    bool        CreateDisplay();
    void        SetupGameState();
    void        SetupRenderStates();
    void        AnimateObjects();
    void        ProcessInput();
    bool        InitDirect3D();
    D3DFORMAT   FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType);
    
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

    D3DXMATRIX              m_mtxView;          // View Matrix
    D3DXMATRIX              m_mtxProjection;    // Projection matrix

	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;	// Temporary vertex buffer to render
    CObject                 m_pObject[3];       // Objects storing mesh instances
    CTimer                  m_Timer;            // Game timer
   
    HWND                    m_hWnd;             // Main window HWND
	HICON					m_hIcon;
	HMENU					m_hMenu;
   
    bool                    m_bLostDevice;      // Is the 3d device currently lost ?
    bool                    m_bActive;          // Is the application active ?
    bool                    m_bRotation1;       // Object 1 rotation enabled / disabled 
    bool                    m_bRotation2;       // Object 2 rotation enabled / disabled 

	LPDIRECT3D9             m_pD3D;             // Direct3D Object
	LPDIRECT3DDEVICE9       m_pD3DDevice;       // Direct3D Device Object
	D3DPRESENT_PARAMETERS   m_D3DPresentParams; // Direct3D Present Parameters

    ULONG                   m_nViewX;           // X Position of render viewport
    ULONG                   m_nViewY;           // Y Position of render viewport
    ULONG                   m_nViewWidth;       // Width of render viewport
    ULONG                   m_nViewHeight;      // Height of render viewport
};

#endif // _CGAMEAPP_H_