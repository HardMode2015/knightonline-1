//-----------------------------------------------------------------------------
// File: CGameApp.cpp
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#include "..\\Includes\\CGameApp.h"

#pragma warning(disable: 4995)
#include "..\\Includes\\MeshLoader.h"
#pragma warning(default: 4995)

const int TILE_SIZE = 4;
const int PATCH_TILE_SIZE = 8;

unsigned int eleBuffer;
unsigned int verBuffer;

float TileDirU[8][4] = {
	{ 0.0f, 1.0f, 0.0f, 1.0f }, //[down][RT, LT, RB, LB]
{ 0.0f, 0.0f, 1.0f, 1.0f }, //[left][RT, LT, RB, LB]
{ 1.0f, 0.0f, 1.0f, 0.0f }, //[up][RT, LT, RB, LB]
{ 1.0f, 1.0f, 0.0f, 0.0f }, //[right][RT, LT, RB, LB]

{ 1.0f, 0.0f, 1.0f, 0.0f }, //[down][LT, RT, LB, RB]
{ 0.0f, 0.0f, 1.0f, 1.0f }, //[left][LT, RT, LB, RB]
{ 0.0f, 1.0f, 0.0f, 1.0f }, //[up][LT, RT, LB, RB]
{ 1.0f, 1.0f, 0.0f, 0.0f }, //[right][LT, RT, LB, RB]
};

float TileDirV[8][4] = {
	{ 0.0f, 0.0f, 1.0f, 1.0f }, //[down][RT, LT, RB, LB]
{ 1.0f, 0.0f, 1.0f, 0.0f }, //[left][RT, LT, RB, LB]
{ 1.0f, 1.0f, 0.0f, 0.0f }, //[up][RT, LT, RB, LB]
{ 0.0f, 1.0f, 0.0f, 1.0f }, //[right][RT, LT, RB, LB]

{ 0.0f, 0.0f, 1.0f, 1.0f }, //[down][LT, RT, LB, RB]
{ 0.0f, 1.0f, 0.0f, 1.0f }, //[left][LT, RT, LB, RB]
{ 1.0f, 1.0f, 0.0f, 0.0f }, //[up][LT, RT, LB, RB]
{ 1.0f, 0.0f, 1.0f, 0.0f }, //[right][LT, RT, LB, RB]
};

CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd = nullptr;
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	m_hIcon = nullptr;
	m_hMenu = nullptr;
	m_bLostDevice = false;
	m_pVertexBuffer = nullptr;
}

CGameApp::~CGameApp()
{
	ShutDown();
}

bool CGameApp::InitInstance(HANDLE hInstance, LPCTSTR lpCmdLine, int iCmdShow)
{
	if (!CreateDisplay()) { ShutDown(); return false; }
	if (!BuildObjects()) { ShutDown(); return false; }
	SetupGameState();
	SetupRenderStates();
	return true;
}

bool CGameApp::CreateDisplay()
{
	LPTSTR WindowTitle = _T("Initialization");
	USHORT Width = 800;
	USHORT Height = 600;
	RECT   rc;

	// Register the new windows window class.
	WNDCLASS			wc;
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StaticWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WindowTitle;
	RegisterClass(&wc);

	// Create the rendering window
	m_hWnd = CreateWindow(WindowTitle, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, Width, Height, nullptr, LoadMenu(wc.hInstance, MAKEINTRESOURCE(IDR_MENU)),
		wc.hInstance, this);

	// Bail on error
	if (!m_hWnd) return false;

	// Retrieve the final client size of the window
	::GetClientRect(m_hWnd, &rc);
	m_nViewX = rc.left;
	m_nViewY = rc.top;
	m_nViewWidth = rc.right - rc.left;
	m_nViewHeight = rc.bottom - rc.top;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Initialize Direct3D (Simple)
	if (!InitDirect3D()) return false;

	// Success!!
	return true;
}

bool CGameApp::InitDirect3D()
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
		MessageBox(m_hWnd, _T("No compatible Direct3D object could be created."), _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);
		return false;

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
	if (FAILED(hRet = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, ulFlags, &PresentParams, &m_pD3DDevice)))
	{
		MessageBox(m_hWnd, _T("Could not create a valid HAL Direct3D device object.\r\n\r\n")
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
		if (FAILED(hRet = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, ulFlags, &PresentParams, &m_pD3DDevice)))
		{
			MessageBox(m_hWnd, _T("Could not create a valid REF Direct3D device object.\r\n\r\nThe system will now exit."),
				_T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL);

			// Failed
			return false;

		} // End if Failure (REF)

	} // End if Failure (HAL)

	  // Store the present parameters
	m_D3DPresentParams = PresentParams;

	// Success!!
	return true;
}

D3DFORMAT CGameApp::FindDepthStencilFormat(ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType)
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

void CGameApp::SetupGameState()
{
	D3DXMatrixIdentity(&m_mtxView);
	m_bRotation1 = true;
	m_bRotation2 = true;
	m_bActive = true;
}

void CGameApp::SetupRenderStates()
{
	// Set up new perspective projection matrix
	float fAspect = (float)m_nViewWidth / (float)m_nViewHeight;
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection, D3DXToRadian(60.0f), fAspect, 1.01f, 1000.0f);

	// Setup our D3D Device initial states
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Setup our vertex FVF code
	m_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	m_mtxView._41 = -22;
	m_mtxView._42 = -154;
	m_mtxView._43 = -15;

	// Setup our matrices
	m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
	m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);
}

int CGameApp::BeginGame()
{
	MSG		msg;
	while (1) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			FrameAdvance();
		}
	}
	return 0;
}

bool CGameApp::ShutDown()
{
	if (m_pD3DDevice) m_pD3DDevice->Release();
	if (m_pD3D) m_pD3D->Release();
	m_pD3D = nullptr;
	m_pD3DDevice = nullptr;
	if (m_hWnd) DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
	return true;
}

LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
#pragma warning (push)
#pragma warning (disable : 4244) // inhibit ignorable warning
	if (Message == WM_CREATE) SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);
#pragma warning (pop)
	CGameApp *Destination = (CGameApp*)((LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA));
	if (Destination) return Destination->DisplayWndProc(hWnd, Message, wParam, lParam);
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

LRESULT CGameApp::DisplayWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
			if (m_pD3DDevice) {
				m_D3DPresentParams.BackBufferWidth = m_nViewWidth;
				m_D3DPresentParams.BackBufferHeight = m_nViewHeight;
				m_pD3DDevice->Reset(&m_D3DPresentParams);
				SetupRenderStates();
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

bool CGameApp::BuildObjects()
{
	HRESULT hRet;
	CVertex* pVertex = nullptr;
	ULONG ulUsage = D3DUSAGE_WRITEONLY;

	// If there is any object initialized, release them
	ReleaseObjects();

	objl::Loader objLoader;
	int nVertexCount = 0;

	bool loadout = objLoader.LoadFile("Dagger\\a119102294.obj");

	if (loadout) {
		for (size_t i = 0; i < objLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = objLoader.LoadedMeshes[i];
			nVertexCount = curMesh.Vertices.size();
		}
	}

	// Get settings here for software vertex processing
	hRet = m_pD3DDevice->CreateVertexBuffer(
		sizeof(CVertex) * nVertexCount, ulUsage,
		D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED,
		&m_pVertexBuffer, nullptr
	);
	if (FAILED(hRet)) return false;

	hRet = m_pVertexBuffer->Lock(0, sizeof(CVertex) * nVertexCount, (void**)&pVertex, 0);
	if (FAILED(hRet)) return false;

	if (loadout) {
		for (size_t i = 0; i < objLoader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = objLoader.LoadedMeshes[i];

			//  position, normal, and texture coordinate
			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				*pVertex++ = CVertex(
					curMesh.Vertices[j].Position.X,
					curMesh.Vertices[j].Position.Y,
					curMesh.Vertices[j].Position.Z,
					RANDOM_COLOR
				);
			}
		}
	}

	m_pVertexBuffer->Unlock();

	// Our two objects should reference this vertex buffer
	m_pObject[0].SetVertexBuffer(m_pVertexBuffer);
	m_pObject[0].m_nVertexCount = nVertexCount;
	m_pObject[1].SetVertexBuffer(m_pVertexBuffer);
	m_pObject[1].m_nVertexCount = nVertexCount;

	D3DXMatrixTranslation(&m_pObject[0].m_mtxWorld, 0.0f, 0.0f, 2.0f);
	D3DXMatrixTranslation(&m_pObject[1].m_mtxWorld, 3.5f, -2.0f, 14.0f);

	FILE* fpMap = fopen("karus_start.gtd", "rb");
	if (fpMap == NULL) {
		printf("ER: File not found!\n\n");
		system("pause");
		return false;
	}

	// NOTE: get the size of the map
	m_ti_MapSize = 0;
	fread(&m_ti_MapSize, sizeof(int), 1, fpMap);
	printf("DB: m_ti_MapSize = %d\n\n", m_ti_MapSize);

	// NOTE: the number of patches which exist within the map
	m_pat_MapSize = (m_ti_MapSize - 1) / PATCH_TILE_SIZE;
	printf("DB: m_pat_MapSize = %d\n\n", m_pat_MapSize);

	// NOTE: read in the mapdata
	m_pMapData = new _N3MapData[m_ti_MapSize*m_ti_MapSize];
	fread(m_pMapData, sizeof(_N3MapData), m_ti_MapSize*m_ti_MapSize, fpMap);
	printf("DB: m_pMapData[0] = {\n");
	printf("DB:     fHeight     = %f\n", m_pMapData[0].fHeight);
	printf("DB:     bIsTileFull = %d\n", m_pMapData[0].bIsTileFull);
	printf("DB:     Tex1Dir     = %d\n", m_pMapData[0].Tex1Dir);
	printf("DB:     Tex2Dir     = %d\n", m_pMapData[0].Tex2Dir);
	printf("DB:     Tex1Idx     = %d\n", m_pMapData[0].Tex1Idx);
	printf("DB:     Tex2Idx     = %d\n", m_pMapData[0].Tex2Idx);
	printf("DB: }\n\n");

	// NOTE: read in the middle Y and radius for each patch
	float** m_ppPatchMiddleY = new float*[m_pat_MapSize];
	float** m_ppPatchRadius = new float*[m_pat_MapSize];

	for (int x = 0; x<m_pat_MapSize; x++) {
		m_ppPatchMiddleY[x] = new float[m_pat_MapSize];
		m_ppPatchRadius[x] = new float[m_pat_MapSize];

		for (int z = 0; z<m_pat_MapSize; z++) {
			fread(&(m_ppPatchMiddleY[x][z]), sizeof(float), 1, fpMap);
			fread(&(m_ppPatchRadius[x][z]), sizeof(float), 1, fpMap);
		}
	}
	printf(
		"DB: m_ppPatchMiddleY[0][0] = %f\n"\
		"DB: m_ppPatchRadius[0][0] = %f\n\n",
		m_ppPatchMiddleY[0][0],
		m_ppPatchRadius[0][0]
	);

	// NOTE: read in the grass attributes
	unsigned char* m_pGrassAttr = new unsigned char[m_ti_MapSize*m_ti_MapSize];
	fread(m_pGrassAttr, sizeof(unsigned char), m_ti_MapSize*m_ti_MapSize, fpMap);
	printf("DB: m_pGrassAttr[0] = %d\n", m_pGrassAttr[0]);

	// NOTE: "set" the grass number
	unsigned char* m_pGrassNum = new unsigned char[m_ti_MapSize*m_ti_MapSize];
	memset(m_pGrassNum, 5, sizeof(unsigned char)*m_ti_MapSize*m_ti_MapSize);
	printf("DB: m_pGrassNum[0] = %d\n\n", m_pGrassNum[0]);

	// NOTE: read in the grass file name
	char* m_pGrassFileName = new char[MAX_PATH];
	fread(m_pGrassFileName, sizeof(char), MAX_PATH, fpMap);
	printf("DB: m_pGrassFileName = \"%s\"\n\n", m_pGrassFileName);

	// NOTE: if there isn't a grass filename then zero out the grass attr
	if (strcmp(m_pGrassFileName, "") == 0) {
		memset(m_pGrassAttr, 0, sizeof(unsigned char)*m_ti_MapSize*m_ti_MapSize);
	}
	else {
		/* CN3Terrain::LoadGrassInfo(void) */
	}

	int m_NumTileTex;
	fread(&m_NumTileTex, sizeof(int), 1, fpMap);
	printf("DB: m_NumTileTex = %d\n", m_NumTileTex);

	int NumTileTexSrc = 0;
	fread(&NumTileTexSrc, sizeof(int), 1, fpMap);

	// NOTE: load in all the texture file names
	char** SrcName = new char*[NumTileTexSrc];
	for (int i = 0; i<NumTileTexSrc; i++) {
		SrcName[i] = new char[MAX_PATH];
		fread(SrcName[i], MAX_PATH, 1, fpMap);
	}

	short SrcIdx, TileIdx;

	for (int i = 0; i < m_NumTileTex; i++) {
		fread(&SrcIdx, sizeof(short), 1, fpMap);
		fread(&TileIdx, sizeof(short), 1, fpMap);
	}

	int NumLightMap;
	fread(&NumLightMap, sizeof(int32_t), 1, fpMap);
	printf("DB: NumLightMap = %d\n\n", NumLightMap);

	if (NumLightMap>0) {
		printf("ER: NumLightMap > 0! Need to implement this.\n\n");
		exit(-1);
	}

	// Load River

	int m_iRiverCount;
	fread(&m_iRiverCount, sizeof(int32_t), 1, fpMap);
	printf("DB: m_iRiverCount = %d\n\n", m_iRiverCount);

	if (m_iRiverCount>0) {
		printf("ER: m_iRiverCount > 0! Need to implement this.\n\n");
		exit(-1);
	}

	// Load Pond

	int m_iPondMeshNum;
	fread(&m_iPondMeshNum, sizeof(int32_t), 1, fpMap);
	printf("DB: m_iPondMeshNum = %d\n\n", m_iPondMeshNum);

	if (m_iPondMeshNum>0) {
		printf("WR: m_iPondMeshNum > 0! Ignoring this...\n\n");
	}

	gl_buf_offsets = (int*)malloc((m_ti_MapSize * m_ti_MapSize) * sizeof(int));

	int buffer_len = 28 * (m_ti_MapSize * m_ti_MapSize);
	float* vertex_info = (float*)malloc(buffer_len * sizeof(float));

	int gl_os = 0;
	int offset = 0;
	int meshOffset = 0;

	m_pMeshBuffer = new CMesh[100];

	for (int pX = 1; pX<11 /*(m_ti_MapSize - 1)*/; ++pX) {
		for (int pZ = 1; pZ<11 /*(m_ti_MapSize - 1)*/; ++pZ) {
			if (pX<1 || pX>(m_ti_MapSize - 1)) continue;
			if (pZ<1 || pZ>(m_ti_MapSize - 1)) continue;
			meshOffset = (pX - 1) * 10 + (pZ - 1);

			_N3MapData MapData = m_pMapData[(pX*m_ti_MapSize) + pZ];

			float u10, u11, u12, u13;
			float v10, v11, v12, v13;
			float u20, u21, u22, u23;
			float v20, v21, v22, v23;

			u10 = TileDirU[MapData.Tex1Dir][2];
			u11 = TileDirU[MapData.Tex1Dir][0];
			u12 = TileDirU[MapData.Tex1Dir][1];
			u13 = TileDirU[MapData.Tex1Dir][3];

			v10 = TileDirV[MapData.Tex1Dir][2];
			v11 = TileDirV[MapData.Tex1Dir][0];
			v12 = TileDirV[MapData.Tex1Dir][1];
			v13 = TileDirV[MapData.Tex1Dir][3];

			u20 = TileDirU[MapData.Tex2Dir][2];
			u21 = TileDirU[MapData.Tex2Dir][0];
			u22 = TileDirU[MapData.Tex2Dir][1];
			u23 = TileDirU[MapData.Tex2Dir][3];

			v20 = TileDirV[MapData.Tex2Dir][2];
			v21 = TileDirV[MapData.Tex2Dir][0];
			v22 = TileDirV[MapData.Tex2Dir][1];
			v23 = TileDirV[MapData.Tex2Dir][3];

			float x0, y0, z0;
			float x1, y1, z1;
			float x2, y2, z2;
			float x3, y3, z3;

			x0 = (float)(pX*TILE_SIZE);
			y0 = m_pMapData[(pX*m_ti_MapSize) + pZ].fHeight;
			z0 = (float)(pZ*TILE_SIZE);

			x1 = (float)(pX*TILE_SIZE);
			y1 = m_pMapData[(pX*m_ti_MapSize) + pZ + 1].fHeight;
			z1 = (float)((pZ + 1)*TILE_SIZE);

			x2 = (float)((pX + 1)*TILE_SIZE);
			y2 = m_pMapData[((pX + 1)*m_ti_MapSize) + pZ + 1].fHeight;
			z2 = (float)((pZ + 1)*TILE_SIZE);

			x3 = (float)((pX + 1)*TILE_SIZE);
			y3 = m_pMapData[((pX + 1)*m_ti_MapSize) + pZ].fHeight;
			z3 = (float)(pZ*TILE_SIZE);

			m_pMeshBuffer[meshOffset].AddVertex(4);
			m_pMeshBuffer[meshOffset].AddIndex(6);
			m_pMeshBuffer[meshOffset].m_pVertex[0] = CVertex(x0, y0, z0, RANDOM_COLOR);
			m_pMeshBuffer[meshOffset].m_pVertex[1] = CVertex(x1, y1, z1, RANDOM_COLOR);
			m_pMeshBuffer[meshOffset].m_pVertex[2] = CVertex(x2, y2, z2, RANDOM_COLOR);
			m_pMeshBuffer[meshOffset].m_pVertex[3] = CVertex(x3, y3, z3, RANDOM_COLOR);
			m_pMeshBuffer[meshOffset].m_pIndex[0] = 0;
			m_pMeshBuffer[meshOffset].m_pIndex[1] = 1;
			m_pMeshBuffer[meshOffset].m_pIndex[2] = 2;
			m_pMeshBuffer[meshOffset].m_pIndex[3] = 2;
			m_pMeshBuffer[meshOffset].m_pIndex[4] = 3;
			m_pMeshBuffer[meshOffset].m_pIndex[5] = 0;
			m_pMeshBuffer[meshOffset].BuildBuffers(m_pD3DDevice, true);
		}
	}

	return true;
}

bool CGameApp::ReleaseObjects()
{
	if (m_pD3DDevice) m_pD3DDevice->SetStreamSource(0, nullptr, 0, 0);
	for (ULONG i = 0; i < 2; i++) m_pObject[i].SetVertexBuffer(nullptr);
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;
	return true;
}

void CGameApp::FrameAdvance()
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
			SetupRenderStates();
			m_bLostDevice = false;

		} // End if can reset
		else
		{
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
		SetWindowText(m_hWnd, FPSBuffer);

	} // End if Frame Rate Altered

	  // Poll & Process input devices
	ProcessInput();

	// Clear the frame & depth buffer ready for drawing
	m_pD3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// Begin Scene Rendering
	m_pD3DDevice->BeginScene();

	CMesh * currentMeshPointer = m_pMeshBuffer;

	for (int mi = 0; mi < 100; mi++)
	{
		// Set vertex buffer stream source
		m_pD3DDevice->SetStreamSource(0, currentMeshPointer->m_pVertexBuffer, 0, sizeof(CVertex));
		m_pD3DDevice->SetIndices(currentMeshPointer->m_pIndexBuffer);

		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		currentMeshPointer++;
	}

	// Loop through each object
	for (ULONG i = 0; i < 2; i++)
	{
		// Set objects world matris
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &m_pObject[i].m_mtxWorld);

		// Set vertex buffer stream source
		m_pD3DDevice->SetStreamSource(0, m_pObject[i].m_pVertexBuffer, 0, sizeof(CVertex));

		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_pObject[i].m_nVertexCount);
	} // Next Object

	  // End Scene Rendering
	m_pD3DDevice->EndScene();

	// Present the buffer
	if (FAILED(m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr))) m_bLostDevice = true;

}

void CGameApp::ProcessInput()
{
	// Simple strafing
	if (GetKeyState(VK_LEFT) & 0xFF00) m_mtxView._41 += 50.0f * m_Timer.GetTimeElapsed();
	if (GetKeyState(VK_RIGHT) & 0xFF00) m_mtxView._41 -= 50.0f * m_Timer.GetTimeElapsed();
	if (GetKeyState(VK_UP) & 0xFF00) m_mtxView._43 -= 50.0f * m_Timer.GetTimeElapsed();
	if (GetKeyState(VK_DOWN) & 0xFF00) m_mtxView._43 += 50.0f * m_Timer.GetTimeElapsed();
	if (GetKeyState(VK_SPACE) & 0xFF00) m_mtxView._42 += 50.0f * m_Timer.GetTimeElapsed();
	if (GetKeyState(VK_TAB) & 0xFF00) m_mtxView._42 -= 50.0f * m_Timer.GetTimeElapsed();

	// Update the device matrix
	if (m_pD3DDevice) m_pD3DDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

}

void CGameApp::AnimateObjects()
{
	// Note : Expanded for the purposes of this example only.
	D3DXMATRIX mtxYaw, mtxPitch, mtxRoll, mtxRotate;
	float RotationYaw, RotationPitch, RotationRoll;

	// Rotate Object 1 by small amount
	if (m_bRotation1)
	{
		// Calculate rotation values for object 0
		RotationYaw = D3DXToRadian(75.0f * m_Timer.GetTimeElapsed());
		RotationPitch = D3DXToRadian(50.0f * m_Timer.GetTimeElapsed());
		RotationRoll = D3DXToRadian(25.0f * m_Timer.GetTimeElapsed());

		// Build rotation matrices 
		D3DXMatrixIdentity(&mtxRotate);
		D3DXMatrixRotationY(&mtxYaw, RotationYaw);
		D3DXMatrixRotationX(&mtxPitch, RotationPitch);
		D3DXMatrixRotationZ(&mtxRoll, RotationRoll);

		// Concatenate the rotation matrices
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxYaw);
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxPitch);
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxRoll);

		// Apply the rotation to our object's matrix
		D3DXMatrixMultiply(&m_pObject[0].m_mtxWorld, &mtxRotate, &m_pObject[0].m_mtxWorld);

	} // End if Rotation Enabled

	  // Rotate Object 2 by small amount
	if (m_bRotation2)
	{
		// Calculate rotation values for object 1
		RotationYaw = D3DXToRadian(-25.0f * m_Timer.GetTimeElapsed());
		RotationPitch = D3DXToRadian(50.0f * m_Timer.GetTimeElapsed());
		RotationRoll = D3DXToRadian(-75.0f * m_Timer.GetTimeElapsed());

		// Build rotation matrices 
		D3DXMatrixIdentity(&mtxRotate);
		D3DXMatrixRotationY(&mtxYaw, RotationYaw);
		D3DXMatrixRotationX(&mtxPitch, RotationPitch);
		D3DXMatrixRotationZ(&mtxRoll, RotationRoll);

		// Concatenate the rotation matrices
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxYaw);
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxPitch);
		D3DXMatrixMultiply(&mtxRotate, &mtxRotate, &mtxRoll);

		// Apply the rotation to our object's matrix
		D3DXMatrixMultiply(&m_pObject[1].m_mtxWorld, &mtxRotate, &m_pObject[1].m_mtxWorld);

	} // End if rotation enabled

}
