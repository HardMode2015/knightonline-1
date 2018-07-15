#include "../Includes/CBase.hpp"
#include "../Res/resource.h"
#include "../Includes/Scene/SceneBase.hpp"

LPDIRECT3D9 CBase::s_lpD3D = nullptr;
LPDIRECT3DDEVICE9 CBase::s_lpD3DDev = nullptr;
D3DPRESENT_PARAMETERS CBase::s_DevParam;
D3DDEVTYPE CBase::s_DevType;
D3DCAPS9 CBase::s_DevCaps;
D3DFORMAT CBase::s_BackBufferFormat;
DWORD CBase::s_dwTextureCaps;
SceneBase * CBase::s_pSceneManager;

CBase::CBase()
{

}

CBase::~CBase()
{
	if (s_lpD3DDev) s_lpD3DDev->Release();
	if (s_lpD3D) s_lpD3D->Release();
	s_lpD3D = nullptr;
	s_lpD3DDev = nullptr;
	if (m_hMainWindow) DestroyWindow(m_hMainWindow);
	m_hMainWindow = nullptr;
}

void CBase::InitEngine(HINSTANCE hWndInstance, LPTSTR sCmdLine)
{
	m_hCursorDefault = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_DEFAULT));
	m_hCursorDefaultClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_DEFAULT_CLICKED));
	m_hCursorBlue = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_BLUE));
	m_hCursorBlueClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_BLUE_CLICKED));
	m_hCursorRepair = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_REPAIR));
	m_hCursorRepairClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_REPAIR_CLICKED));
	m_hCursorAttack = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_ATTACK));
	m_hCursorWrite = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_WRITE));

	s_dwTextureCaps = 0;

	CreateMainWindow(hWndInstance, sCmdLine);

	InitD3D();

	s_pSceneManager = new SceneBase();
	s_pSceneManager->InitStatic();

	D3DDISPLAYMODE dm;
	s_lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
	s_DevParam.BackBufferCount = 1;
	s_BackBufferFormat = dm.Format;

	if (D3D_OK == s_lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, s_DevType, s_DevParam.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1)) s_dwTextureCaps |= TEX_CAPS_DXT1;
	if (D3D_OK == s_lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, s_DevType, s_DevParam.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT2)) s_dwTextureCaps |= TEX_CAPS_DXT2;
	if (D3D_OK == s_lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, s_DevType, s_DevParam.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT3)) s_dwTextureCaps |= TEX_CAPS_DXT3;
	if (D3D_OK == s_lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, s_DevType, s_DevParam.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT4)) s_dwTextureCaps |= TEX_CAPS_DXT4;
	if (D3D_OK == s_lpD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, s_DevType, s_DevParam.BackBufferFormat, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT5)) s_dwTextureCaps |= TEX_CAPS_DXT5;
	if (s_DevCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) s_dwTextureCaps |= TEX_CAPS_SQUAREONLY;
	if (s_DevCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) s_dwTextureCaps |= TEX_CAPS_MIPMAP;
	if (s_DevCaps.TextureCaps & D3DPTEXTURECAPS_POW2) s_dwTextureCaps |= TEX_CAPS_POW2;
}

void CBase::CreateMainWindow(HINSTANCE hWndInstance, LPTSTR sCmdLine)
{
	LPTSTR WindowTitle = _T("Initialization");
	USHORT Width = 800;
	USHORT Height = 600;
	RECT   rc;

	// Register the new windows window class.
	WNDCLASS			wc;
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StaticWindowMainProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hWndInstance;
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(WIDI_ICON));
	wc.hCursor = m_hCursorDefault;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WindowTitle;
	RegisterClass(&wc);

	// Create the rendering window
	m_hMainWindow = CreateWindow(WindowTitle, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, Width, Height, nullptr, nullptr,
		wc.hInstance, this);

	// Bail on error
	if (!m_hMainWindow) {
		MessageBox(0, _T("Can not create window."), _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);
		exit(-1);
	}

	// Retrieve the final client size of the window
	::GetClientRect(m_hMainWindow, &rc);
	m_nViewX = rc.left;
	m_nViewY = rc.top;
	m_nViewWidth = rc.right - rc.left;
	m_nViewHeight = rc.bottom - rc.top;

	ShowWindow(m_hMainWindow, SW_SHOW);
}

LRESULT CALLBACK CBase::StaticWindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
#pragma warning (push)
#pragma warning (disable : 4244) // inhibit ignorable warning
	if (Message == WM_CREATE) SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);
#pragma warning (pop)
	CBase *Destination = (CBase*)((LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA));
	if (Destination) return Destination->WindowMainProc(hWnd, Message, wParam, lParam);
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

LRESULT CBase::WindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message) {
	case WM_CREATE:
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
			m_bActive = false;
		}
		else {
			m_bActive = true;
			m_nViewWidth = LOWORD(lParam);
			m_nViewHeight = HIWORD(lParam);
			if (s_lpD3DDev) {
				s_DevParam.BackBufferWidth = m_nViewWidth;
				s_DevParam.BackBufferHeight = m_nViewHeight;
				s_lpD3DDev->Reset(&s_DevParam);
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	return 0;
}

void CBase::InitD3D()
{
	boolean				  isFullscreen = false;
	HRESULT               hRet;
	D3DPRESENT_PARAMETERS PresentParams;
	D3DDISPLAYMODE        CurrentMode;

	// First of all create our D3D Object
	s_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!s_lpD3D)
	{
		MessageBox(m_hMainWindow, _T("No compatible Direct3D object could be created."), _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);
		exit(-1);
	} // End if failure

	  // Fill out a simple set of present parameters
	ZeroMemory(&PresentParams, sizeof(D3DPRESENT_PARAMETERS));

	// Select back buffer format etc
	s_lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &CurrentMode);
	PresentParams.BackBufferFormat = CurrentMode.Format;

	// Setup remaining flags
	PresentParams.AutoDepthStencilFormat = FindDepthStencilFormat(D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_HAL);
	PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	PresentParams.Windowed = !isFullscreen;
	PresentParams.EnableAutoDepthStencil = true;

	if (isFullscreen) {
		PresentParams.BackBufferWidth = 800;
		PresentParams.BackBufferHeight = 600;
	}

	// Set Creation Flags
	unsigned long ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// Check if Hardware T&L is available
	ZeroMemory(&s_DevCaps, sizeof(D3DCAPS9));
	s_lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &s_DevCaps);
	if (s_DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Attempt to create a HAL device
	if (FAILED(hRet = s_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hMainWindow, ulFlags, &PresentParams, &s_lpD3DDev)))
	{
		MessageBox(m_hMainWindow, _T("Could not create a valid HAL Direct3D device object.\r\n\r\n")
			_T("The system will now attempt to create a device utilising the 'Reference Rasterizer' (D3DDEVTYPE_REF)"),
			_T("Fatal Error!"), MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);

		// Find REF depth buffer format
		PresentParams.AutoDepthStencilFormat = FindDepthStencilFormat(D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_REF);

		// Check if Hardware T&L is available
		ZeroMemory(&s_DevCaps, sizeof(D3DCAPS9));
		ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		s_lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &s_DevCaps);
		if (s_DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		// Attempt to create a REF device
		if (FAILED(hRet = s_lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hMainWindow, ulFlags, &PresentParams, &s_lpD3DDev)))
		{
			MessageBox(m_hMainWindow, _T("Could not create a valid REF Direct3D device object.\r\n\r\nThe system will now exit."),
				_T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);

			// Failed
			exit(-1);

		} // End if Failure (REF)
		else {
			s_DevType = D3DDEVTYPE_REF;
		}

	} // End if Failure (HAL)
	else {
		s_DevType = D3DDEVTYPE_HAL;
	}

	// Store the present parameters
	s_DevParam = PresentParams;
}

D3DFORMAT CBase::FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType)
{
	if (SUCCEEDED(s_lpD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32))) {
		if (SUCCEEDED(s_lpD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32))) return D3DFMT_D32;
	}
	if (SUCCEEDED(s_lpD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8))) {
		if (SUCCEEDED(s_lpD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8))) return D3DFMT_D24X8;
	}
	if (SUCCEEDED(s_lpD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16))) {
		if (SUCCEEDED(s_lpD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16))) return D3DFMT_D16;
	}
	return D3DFMT_UNKNOWN;
}

void CBase::RenderLines(const __Vector3 *pvLines, int nCount, D3DCOLOR color)
{
	DWORD dwAlpha, dwFog, dwLight;
	s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);

	if (dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	if (dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if (dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	static __Material smtl;
	static bool bInit = false;
	if (false == bInit)
	{
		smtl.Init();
		bInit = true;
	}

	s_lpD3DDev->SetTexture(0, NULL);

	static __VertexColor svLines[512];

	s_lpD3DDev->SetFVF(FVF_CV);

	int nRepeat = nCount / 512;
	for (int i = 0; i < nRepeat; i++)
	{
		for (int j = 0; j < 512; j++) svLines[j].Set(pvLines[i * 512 + j].x, pvLines[i * 512 + j].y, pvLines[i * 512 + j].z, color);

		s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 511, svLines, sizeof(__VertexColor));
	}
	int nPC = nCount % 512;
	for (int j = 0; j < nPC + 1; j++) svLines[j].Set(pvLines[nRepeat * 512 + j].x, pvLines[nRepeat * 512 + j].y, pvLines[nRepeat * 512 + j].z, color);
	s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, nPC, svLines, sizeof(__VertexColor)); // Y

	if (dwFog) s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	if (dwAlpha) s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	if (dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
}

void CBase::RenderLines(const RECT& rc, D3DCOLOR color)
{
	static __VertexTransformedColor vLines[5];

	// There will be no error. There is still no resolution that exceeds a floats limit..
	#pragma warning(disable : 4244)
	vLines[0].Set(rc.left, rc.top, 0.9f, 1.0f, color);
	vLines[1].Set(rc.right, rc.top, 0.9f, 1.0f, color);
	vLines[2].Set(rc.right, rc.bottom, 0.9f, 1.0f, color);
	vLines[3].Set(rc.left, rc.bottom, 0.9f, 1.0f, color);
	vLines[4] = vLines[0];
	#pragma warning(default : 4244)

	DWORD dwZ, dwFog, dwAlpha, dwCOP, dwCA1, dwSrcBlend, dwDestBlend, dwVertexShader, dwAOP, dwAA1;
	CBase::s_lpD3DDev->GetRenderState(D3DRS_ZENABLE, &dwZ);
	CBase::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CBase::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CBase::s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	CBase::s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	CBase::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwCOP);
	CBase::s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwCA1);
	CBase::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAOP);
	CBase::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAA1);
	CBase::s_lpD3DDev->GetFVF(&dwVertexShader);

	CBase::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	CBase::s_lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vLines, sizeof(__VertexTransformedColor));

	CBase::s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZ);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwCOP);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwCA1);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAOP);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAA1);
	CBase::s_lpD3DDev->SetFVF(dwVertexShader);
}