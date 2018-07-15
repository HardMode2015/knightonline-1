//-----------------------------------------------------------------------------
// File: CGameApp.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CBASE_H_
#define _CBASE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "DirectXExtensions.hpp"
#include "CTimer.hpp"

const DWORD TEX_CAPS_DXT1 = 0x00000001;
const DWORD TEX_CAPS_DXT2 = 0x00000002;
const DWORD TEX_CAPS_DXT3 = 0x00000004;
const DWORD TEX_CAPS_DXT4 = 0x00000008;
const DWORD TEX_CAPS_DXT5 = 0x00000010;
const DWORD TEX_CAPS_SQUAREONLY = 0x00000020;
const DWORD TEX_CAPS_MIPMAP = 0x00000040;
const DWORD TEX_CAPS_POW2 = 0x00000080;

const float CAMERA_RADIUS_UNIT = 2.0f;
const int MAX_CAMERA_RADIUS = 512;

enum VERTEXPROCESSING_TYPE
{
	SOFTWARE_VP = 1,
	MIXED_VP = 2,
	HARDWARE_VP = 3,
	PURE_HARDWARE_VP = 4
};

struct __CameraData
{
	__CameraData()
	{
		Release();
	}

	void Release()
	{
		memset(this, 0, sizeof(__CameraData));
		mtxView.Identity();
		mtxViewInverse.Identity();
		mtxProjection.Identity();
	}

	__Vector3		vEye;
	__Vector3		vAt;
	__Vector3		vUp;

	float			fFOV;
	float			fAspect;
	float			fNP;
	float			fFP;
	D3DVIEWPORT9	vp;
	__Matrix44		mtxView;
	__Matrix44		mtxViewInverse;
	__Matrix44		mtxProjection;

	float			fFrustum [6][4];

	BOOL IsOutOfFrustum(const __Vector3& vPosition, float fRadius)
	{
		if((vEye - vPosition).Magnitude() > fFP + fRadius) 
			return TRUE;

		int p;
		for( p = 0; p < 6; p++ )
		{
			if( fFrustum[p][0] * vPosition.x + fFrustum[p][1] * vPosition.y +
				fFrustum[p][2] * vPosition.z + fFrustum[p][3] <= -fRadius )
				return TRUE;
		}
		return FALSE;
	}
};

struct __RenderInfo
{
	int nShape;
	int nShape_Part;
	int nShape_Polygon;

	int nChr;
	int nChr_Part;
	int nChr_Polygon;
	int nChr_Plug;
	int	nChr_Plug_Polygon;

	int nTerrain_Polygon;
	int nTerrain_Tile_Polygon;

	int nAlpha_Polygon;

	int nTexture_32X32;
	int nTexture_64X64;
	int nTexture_128X128;
	int nTexture_256X256;
	int nTexture_512X512;
	int nTexture_Huge;
	int nTexture_OtherSize;
};

struct __ResrcInfo
{
	int nTexture_Loaded_32X32;
	int nTexture_Loaded_64X64;
	int nTexture_Loaded_128X128;
	int nTexture_Loaded_256X256;
	int nTexture_Loaded_512X512;
	int nTexture_Loaded_Huge;
	int nTexture_Loaded_OtherSize;
};

struct __Options
{
	int iUseShadow;
	int iTexLOD_Chr;
	int iTexLOD_Shape;
	int iTexLOD_Terrain;
	int iViewWidth;
	int iViewHeight;
	int iViewColorDepth;
	int iViewDist;
	int iEffectSndDist;

	bool bSndEnable;
	bool bSndDuplicated;
	bool bWindowCursor;

	void InitDefault()
	{
		iUseShadow = true;
		iTexLOD_Chr = 0;
		iTexLOD_Shape = 0;
		iTexLOD_Terrain = 0;
		iViewColorDepth = 16;
		iViewWidth = 1024;
		iViewHeight = 768;
		iViewDist = 512;
		iEffectSndDist = 48;
		bSndEnable = 1;
		bSndDuplicated = false;
		bWindowCursor = true;
	}
	__Options() { InitDefault(); }
};

class CBase
{
public:
	CBase();
	virtual ~CBase();

	void InitEngine(HINSTANCE hWndInstance, LPTSTR sCmdLine);
	void CreateMainWindow(HINSTANCE hWndInstance, LPTSTR sCmdLine);
	void InitD3D();

	static void			RenderLines(const __Vector3 *pvLines, int nCount, D3DCOLOR color);
	static void			RenderLines(const RECT& rc, D3DCOLOR color);

	static LPDIRECT3D9 s_lpD3D;
	static LPDIRECT3DDEVICE9 s_lpD3DDev;
	static D3DPRESENT_PARAMETERS s_DevParam;
	static D3DDEVTYPE s_DevType;
	static D3DCAPS9 s_DevCaps;
	static D3DFORMAT s_BackBufferFormat;
	static DWORD s_dwTextureCaps;
	static HWND s_hWndBase;
	static HWND s_hWndPresent;

	static __CameraData s_CameraData;
	static __ResrcInfo s_ResrcInfo;
	static __Options s_Options;
	static __RenderInfo s_RenderInfo;

	CTimer m_Timer;

	HWND m_hMainWindow = nullptr;
	ULONG m_nViewX;
	ULONG m_nViewY;
	ULONG m_nViewWidth;
	ULONG m_nViewHeight;

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

	static class SceneBase * s_pSceneManager;

	//static CN3SndMgr				s_SndMgr;
	//static CN3AlphaPrimitiveManager	s_AlphaMgr; // Alpha blend 할 폴리곤들을 관리.. 추가했다가.. 카메라 거리에 맟추어 정렬하고 한꺼번에 그린다..

	//static CN3Mng<class CN3Texture>		s_MngTex; // Texture Manager
	//static CN3Mng<class CN3Mesh>		s_MngMesh; // Normal Mesh Manager
	//static CN3Mng<class CN3VMesh>		s_MngVMesh; // 단순히 폴리곤만 갖고 있는 메시 - 주로 충돌 체크에 쓴다..
	//static CN3Mng<class CN3PMesh>		s_MngPMesh; // Progressive Mesh Manager
	//static CN3Mng<class CN3Joint>		s_MngJoint; // Joint Manager
	//static CN3Mng<class CN3CPartSkins>	s_MngSkins; // Character Part Skin Manager
	//static CN3Mng<class CN3AnimControl>	s_MngAniCtrl; // Animation Manager
	//static CN3Mng<class CN3FXPMesh>		s_MngFXPMesh; // FX에서 쓰는 PMesh - 파일은 일반 PMesh를 쓰지만 속은 다르다.
	//static CN3Mng<class CN3FXShape>		s_MngFXShape;

private:

	D3DFORMAT FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType);
	static LRESULT CALLBACK StaticWindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

};

#endif