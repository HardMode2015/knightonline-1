#include "../Includes/CEngine.hpp"

CEngine::CEngine()
{

}

CEngine::~CEngine()
{
	if (m_pD3DDevice) m_pD3DDevice->Release();
	if (m_pD3D) m_pD3D->Release();
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	if (m_hMainWindow) DestroyWindow(m_hMainWindow);
	m_hMainWindow = nullptr;
}

void CEngine::InitEngine(HINSTANCE hWndInstance, LPTSTR sCmdLine)
{
	m_hCursorDefault = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_DEFAULT));
	m_hCursorDefaultClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_DEFAULT_CLICKED));
	m_hCursorBlue = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_BLUE));
	m_hCursorBlueClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_BLUE_CLICKED));
	m_hCursorRepair = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_REPAIR));
	m_hCursorRepairClicked = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_REPAIR_CLICKED));
	m_hCursorAttack = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_ATTACK));
	m_hCursorWrite = LoadCursor(hWndInstance, MAKEINTRESOURCE(WIDC_WRITE));

	CreateMainWindow(hWndInstance, sCmdLine);

	InitD3D();
}

void CEngine::CreateMainWindow(HINSTANCE hWndInstance, LPTSTR sCmdLine)
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

LRESULT CALLBACK CEngine::StaticWindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	#pragma warning (push)
	#pragma warning (disable : 4244) // inhibit ignorable warning
	if (Message == WM_CREATE) SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);
	#pragma warning (pop)
	CEngine *Destination = (CEngine*)((LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA));
	if (Destination) return Destination->WindowMainProc(hWnd, Message, wParam, lParam);
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

LRESULT CEngine::WindowMainProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
		} else {
			m_bActive = true;
			m_nViewWidth = LOWORD(lParam);
			m_nViewHeight = HIWORD(lParam);
			if (m_pD3DDevice) {
				m_D3DPresentParams.BackBufferWidth = m_nViewWidth;
				m_D3DPresentParams.BackBufferHeight = m_nViewHeight;
				m_pD3DDevice->Reset(&m_D3DPresentParams);
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

void CEngine::InitD3D()
{
	boolean				  isFullscreen = false;
	HRESULT               hRet;
	D3DPRESENT_PARAMETERS PresentParams;
	D3DCAPS9              Caps;
	D3DDISPLAYMODE        CurrentMode;

	// First of all create our D3D Object
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3D)
	{
		MessageBox(m_hMainWindow, _T("No compatible Direct3D object could be created."), _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);
		exit(-1);
	} // End if failure

	  // Fill out a simple set of present parameters
	ZeroMemory(&PresentParams, sizeof(D3DPRESENT_PARAMETERS));

	// Select back buffer format etc
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &CurrentMode);
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
	ZeroMemory(&Caps, sizeof(D3DCAPS9));
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps);
	if (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Attempt to create a HAL device
	if (FAILED(hRet = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hMainWindow, ulFlags, &PresentParams, &m_pD3DDevice)))
	{
		MessageBox(m_hMainWindow, _T("Could not create a valid HAL Direct3D device object.\r\n\r\n")
			_T("The system will now attempt to create a device utilising the 'Reference Rasterizer' (D3DDEVTYPE_REF)"),
			_T("Fatal Error!"), MB_OK | MB_ICONINFORMATION | MB_APPLMODAL);

		// Find REF depth buffer format
		PresentParams.AutoDepthStencilFormat = FindDepthStencilFormat(D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_REF);

		// Check if Hardware T&L is available
		ZeroMemory(&Caps, sizeof(D3DCAPS9));
		ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &Caps);
		if (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

		// Attempt to create a REF device
		if (FAILED(hRet = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hMainWindow, ulFlags, &PresentParams, &m_pD3DDevice)))
		{
			MessageBox(m_hMainWindow, _T("Could not create a valid REF Direct3D device object.\r\n\r\nThe system will now exit."),
				_T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);

			// Failed
			exit(-1);

		} // End if Failure (REF)

	} // End if Failure (HAL)

	  // Store the present parameters
	m_D3DPresentParams = PresentParams;
}

D3DFORMAT CEngine::FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType)
{
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32))) {
		if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32))) return D3DFMT_D32;
	}
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8))) {
		if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8))) return D3DFMT_D24X8;
	}
	if (SUCCEEDED(m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16))) {
		if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch(AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16))) return D3DFMT_D16;
	}
	return D3DFMT_UNKNOWN;
}

int CEngine::StartGame()
{
	MSG msg;
	while (1) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			AdvanceFrame();
		}
	}
	return 0;
}

void CEngine::AdvanceFrame()
{
	// Advance the timer
	m_Timer.Tick();

	// Skip if app is inactive
	if (!m_bActive) return;

	// Recover lost device if required
	if (m_bLostDevice)
	{
		// Can we reset the device yet ?
		HRESULT hRet = m_pD3DDevice->TestCooperativeLevel();
		if (hRet == D3DERR_DEVICENOTRESET)
		{
			// Restore the device
			m_pD3DDevice->Reset(&m_D3DPresentParams);
			m_bLostDevice = false;
		} else {
			return;
		} // End if cannot reset
	} // End if Device Lost

	  // Get / Display the framerate
	int nFrameRate = m_Timer.GetFrameRate();
	static int nLastFrameRate = 0;
	if (nLastFrameRate != nFrameRate)
	{
		static char FPSBuffer[20];
		m_Timer.GetFrameRate(FPSBuffer);
		nLastFrameRate = nFrameRate;
		SetWindowText(m_hMainWindow, FPSBuffer);

	} // End if Frame Rate Altered

	// Poll & Process input devices
	//ProcessInput();

	m_cCurrentScene.Tick();

	// Clear the frame & depth buffer ready for drawing
	m_pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// Begin Scene Rendering
	m_pD3DDevice->BeginScene();

	// Render Scene
	m_cCurrentScene.Render();

	//CMesh * currentMeshPointer = m_pMeshBuffer;

	//for (int mi = 0; mi < 100; mi++)
	//{
		// Set vertex buffer stream source
		//m_pD3DDevice->SetStreamSource(0, currentMeshPointer->m_pVertexBuffer, 0, sizeof(CVertex));
		//m_pD3DDevice->SetIndices(currentMeshPointer->m_pIndexBuffer);

		//m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		//currentMeshPointer++;
	//}

	// Loop through each object
	//for (ULONG i = 0; i < 2; i++)
	//{
		// Set objects world matris
		//m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_pObject[i].m_mtxWorld);

		// Set vertex buffer stream source
		//m_pD3DDevice->SetStreamSource(0, m_pObject[i].m_pVertexBuffer, 0, sizeof(CVertex));

		//m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_pObject[i].m_nVertexCount);
	//} // Next Object

	// End Scene Rendering
	m_pD3DDevice->EndScene();

	// Present the buffer
	if (FAILED(m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr))) m_bLostDevice = true;
}