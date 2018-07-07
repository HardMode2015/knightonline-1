//-----------------------------------------------------------------------------
// File: CGameApp.cpp
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#include "..\\Includes\\CGameApp.h"

#pragma warning(disable: 4995)
#include "..\\Includes\\MeshLoader.h"
#pragma warning(default: 4995)

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

bool CGameApp::InitInstance( HANDLE hInstance, LPCTSTR lpCmdLine, int iCmdShow )
{
    if (!CreateDisplay()) { ShutDown(); return false; }
    if (!BuildObjects()) { ShutDown(); return false; }
    SetupGameState();
    SetupRenderStates();
	return true;
}

bool CGameApp::CreateDisplay()
{
	LPTSTR WindowTitle  = _T("Initialization");
    USHORT Width        = 800;
    USHORT Height       = 600;
    RECT   rc;

    // Register the new windows window class.
    WNDCLASS			wc;	
	wc.style			= CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= StaticWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= (HINSTANCE)GetModuleHandle(nullptr);
	wc.hIcon			= LoadIcon( wc.hInstance, MAKEINTRESOURCE(IDI_ICON));
	wc.hCursor			= LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH )GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= nullptr;
	wc.lpszClassName	= WindowTitle;
	RegisterClass(&wc);

    // Create the rendering window
	m_hWnd = CreateWindow( WindowTitle, WindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 
                           CW_USEDEFAULT, Width, Height, nullptr, LoadMenu( wc.hInstance, MAKEINTRESOURCE(IDR_MENU) ),
                           wc.hInstance, this );
	
    // Bail on error
    if (!m_hWnd) return false;

    // Retrieve the final client size of the window
    ::GetClientRect( m_hWnd, &rc );
    m_nViewX      = rc.left;
    m_nViewY      = rc.top;
    m_nViewWidth  = rc.right - rc.left;
    m_nViewHeight = rc.bottom - rc.top;

    // Show the window
	ShowWindow(m_hWnd, SW_SHOW);

    // Initialize Direct3D (Simple)
    if (!InitDirect3D( )) return false;

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
    m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if (!m_pD3D) 
    {
        MessageBox( m_hWnd, _T("No compatible Direct3D object could be created."), _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL );
        return false;
    
    } // End if failure

    // Fill out a simple set of present parameters
    ZeroMemory( &PresentParams, sizeof(D3DPRESENT_PARAMETERS) );

    // Select back buffer format etc
	m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &CurrentMode);
	PresentParams.BackBufferFormat = CurrentMode.Format;
        
	// Setup remaining flags
	PresentParams.AutoDepthStencilFormat = FindDepthStencilFormat( D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_HAL );
    PresentParams.SwapEffect			 = D3DSWAPEFFECT_DISCARD;
    PresentParams.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
	PresentParams.Windowed				 = !isFullscreen;
	PresentParams.EnableAutoDepthStencil = true;

	if (isFullscreen) {
		PresentParams.BackBufferWidth = 800;
		PresentParams.BackBufferHeight = 600;
	}
    
	// Set Creation Flags
	unsigned long ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

    // Check if Hardware T&L is available
    ZeroMemory( &Caps, sizeof(D3DCAPS9) );
    m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps );
    if ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        
    // Attempt to create a HAL device
    if( FAILED( hRet = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, ulFlags, &PresentParams, &m_pD3DDevice ) ) ) 
    {
        MessageBox( m_hWnd, _T("Could not create a valid HAL Direct3D device object.\r\n\r\n")
                            _T("The system will now attempt to create a device utilising the 'Reference Rasterizer' (D3DDEVTYPE_REF)"),
                            _T("Fatal Error!"), MB_OK | MB_ICONINFORMATION | MB_APPLMODAL );
        
        // Find REF depth buffer format
        PresentParams.AutoDepthStencilFormat = FindDepthStencilFormat( D3DADAPTER_DEFAULT, CurrentMode, D3DDEVTYPE_REF );

        // Check if Hardware T&L is available
        ZeroMemory( &Caps, sizeof(D3DCAPS9) );
        ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &Caps );
        if ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

        // Attempt to create a REF device
        if( FAILED( hRet = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd, ulFlags, &PresentParams, &m_pD3DDevice ) ) ) 
        {
            MessageBox( m_hWnd, _T("Could not create a valid REF Direct3D device object.\r\n\r\nThe system will now exit."),
                                _T("Fatal Error!"), MB_OK | MB_ICONSTOP | MB_APPLMODAL );

            // Failed
            return false;
        
        } // End if Failure (REF)

    } // End if Failure (HAL)

    // Store the present parameters
    m_D3DPresentParams = PresentParams;

    // Success!!
    return true;
}

D3DFORMAT CGameApp::FindDepthStencilFormat( ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType )
{

    // Test for 24 bith depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D32 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32 ))) return D3DFMT_D32;
    
    } // End if 32bpp Available

    // Test for 24 bit depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D24X8 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8 ))) return D3DFMT_D24X8;
    
    } // End if 24bpp Available

    // Test for 16 bit depth buffer
    if (SUCCEEDED( m_pD3D->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D16 )))
    {
        if (SUCCEEDED( m_pD3D->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16 ))) return D3DFMT_D16;
    
    } // End if 16bpp Available

    // No depth buffer supported
    return D3DFMT_UNKNOWN;

}

void CGameApp::SetupGameState()
{
    // Setup Default Matrix Values
    D3DXMatrixIdentity( &m_mtxView );
    
    // Enable rotation
    m_bRotation1 = true;
    m_bRotation2 = true;

    // App is active
    m_bActive    = true;

}

void CGameApp::SetupRenderStates()
{
    // Set up new perspective projection matrix
    float fAspect = (float)m_nViewWidth / (float)m_nViewHeight;
    D3DXMatrixPerspectiveFovLH( &m_mtxProjection, D3DXToRadian( 60.0f ), fAspect, 1.01f, 1000.0f );

    // Setup our D3D Device initial states
    m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pD3DDevice->SetRenderState( D3DRS_DITHERENABLE,  TRUE );
    m_pD3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    m_pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Setup our vertex FVF code
    m_pD3DDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

    // Setup our matrices
    m_pD3DDevice->SetTransform( D3DTS_VIEW, &m_mtxView );
    m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_mtxProjection );
}

int CGameApp::BeginGame()
{
    MSG		msg;

    // Start main loop
	while (1) 
    {
        // Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) ) 
        {
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
        else 
        {
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
    } // Until quit message is receieved

    return 0;
}

bool CGameApp::ShutDown()
{
    // Destroy Direct3D Objects
    if ( m_pD3DDevice ) m_pD3DDevice->Release();
    if ( m_pD3D       ) m_pD3D->Release();
    m_pD3D       = nullptr;
    m_pD3DDevice = nullptr;
    
    // Destroy the render window
    if ( m_hWnd ) DestroyWindow( m_hWnd );
    m_hWnd = nullptr;
    
    // Shutdown Success
    return true;
}

LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    #pragma warning (push)
    #pragma warning (disable : 4244) // inhibit ignorable warning
    if ( Message == WM_CREATE ) SetWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)((CREATESTRUCT FAR *)lParam)->lpCreateParams);
    #pragma warning (pop)
    CGameApp *Destination = (CGameApp*)((LONG_PTR)GetWindowLongPtr( hWnd, GWL_USERDATA ));
    if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
    return DefWindowProc( hWnd, Message, wParam, lParam );
}

LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
    // Determine message type
	switch (Message)
    {
		case WM_CREATE:
            break;
		
        case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
        case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
        case WM_SIZE:

            if ( wParam == SIZE_MINIMIZED )
            {
                // App is inactive
                m_bActive = false;
            
            } // App has been minimized
            else
            {
                // App is active
                m_bActive = true;

                // Store new viewport sizes
                m_nViewWidth  = LOWORD( lParam );
                m_nViewHeight = HIWORD( lParam );
        
                if (m_pD3DDevice) 
                {
                    // Store new sizes
                    m_D3DPresentParams.BackBufferWidth = m_nViewWidth;
                    m_D3DPresentParams.BackBufferHeight = m_nViewHeight;
                    
                    // Reset the device
                    m_pD3DDevice->Reset( &m_D3DPresentParams );
                    SetupRenderStates( );
                
                } // End if
            
            } // End if !Minimized

			break;

        case WM_KEYDOWN:

            // Which key was pressed?
			switch (wParam) 
            {
				case VK_ESCAPE:
					PostQuitMessage(0);
					return 0;
                    
			} // End Switch

			break;

        case WM_COMMAND:

            // Process Menu Items
            switch( LOWORD(wParam) )
            {
                case ID_ANIM_ROTATION1:
                    // Disable / enable rotation
                    m_bRotation1 = !m_bRotation1;
                    ::CheckMenuItem( ::GetMenu( m_hWnd ), ID_ANIM_ROTATION1, 
                                     MF_BYCOMMAND | (m_bRotation1) ? MF_CHECKED :  MF_UNCHECKED );
                    break;

                case ID_ANIM_ROTATION2:
                    // Disable / enable rotation
                    m_bRotation2 = !m_bRotation2;
                    ::CheckMenuItem( ::GetMenu( m_hWnd ), ID_ANIM_ROTATION2, 
                                     MF_BYCOMMAND | (m_bRotation2) ? MF_CHECKED :  MF_UNCHECKED );
                    break;

                case ID_EXIT:
                    // Recieved key/menu command to exit app
                    SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
                    return 0;
            
            } // End Switch

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

    } // End Message Switch
    
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



	// Set both objects matrices so that they are offset slightly
	D3DXMatrixTranslation(&m_pObject[0].m_mtxWorld, 0.0f, 0.0f, 2.0f);
	D3DXMatrixTranslation(&m_pObject[1].m_mtxWorld, 3.5f, -2.0f, 14.0f);

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
    m_Timer.Tick( );
   
    // Skip if app is inactive
    if ( !m_bActive ) return;

    // Recover lost device if required
    if ( m_bLostDevice )
    {
        // Can we reset the device yet ?
        HRESULT hRet = m_pD3DDevice->TestCooperativeLevel();
        if ( hRet == D3DERR_DEVICENOTRESET )
        {
            // Restore the device
            m_pD3DDevice->Reset( &m_D3DPresentParams );
            SetupRenderStates( );
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
    if ( nLastFrameRate != nFrameRate )
    {
        static char FPSBuffer[20];
        m_Timer.GetFrameRate( FPSBuffer );
        nLastFrameRate = nFrameRate;
        SetWindowText( m_hWnd, FPSBuffer );

    } // End if Frame Rate Altered

    // Poll & Process input devices
    ProcessInput();

    // Animate the two objects
    AnimateObjects();

    // Clear the frame & depth buffer ready for drawing
    m_pD3DDevice->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );
    
    // Begin Scene Rendering
    m_pD3DDevice->BeginScene();

    // Loop through each object
    for ( ULONG i = 0; i < 2; i++ )
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
    if ( FAILED(m_pD3DDevice->Present( nullptr, nullptr, nullptr, nullptr)) ) m_bLostDevice = true;

}

void CGameApp::ProcessInput( )
{
    // Simple strafing
    if ( GetKeyState( VK_LEFT  ) & 0xFF00 ) m_mtxView._41 += 25.0f * m_Timer.GetTimeElapsed();
    if ( GetKeyState( VK_RIGHT ) & 0xFF00 ) m_mtxView._41 -= 25.0f * m_Timer.GetTimeElapsed();
        
    // Update the device matrix
    if (m_pD3DDevice) m_pD3DDevice->SetTransform( D3DTS_VIEW, &m_mtxView );

}

void CGameApp::AnimateObjects()
{
    // Note : Expanded for the purposes of this example only.
    D3DXMATRIX mtxYaw, mtxPitch, mtxRoll, mtxRotate;
    float RotationYaw, RotationPitch, RotationRoll;

    // Rotate Object 1 by small amount
    if ( m_bRotation1 )
    {
        // Calculate rotation values for object 0
        RotationYaw   = D3DXToRadian( 75.0f * m_Timer.GetTimeElapsed() );
        RotationPitch = D3DXToRadian( 50.0f * m_Timer.GetTimeElapsed() );
        RotationRoll  = D3DXToRadian( 25.0f * m_Timer.GetTimeElapsed() );

        // Build rotation matrices 
        D3DXMatrixIdentity( &mtxRotate );
        D3DXMatrixRotationY( &mtxYaw, RotationYaw);
        D3DXMatrixRotationX( &mtxPitch,RotationPitch);
        D3DXMatrixRotationZ( &mtxRoll, RotationRoll);
        
        // Concatenate the rotation matrices
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxYaw );
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxPitch );
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxRoll );
            
        // Apply the rotation to our object's matrix
        D3DXMatrixMultiply( &m_pObject[ 0 ].m_mtxWorld, &mtxRotate, &m_pObject[ 0 ].m_mtxWorld );

    } // End if Rotation Enabled

    // Rotate Object 2 by small amount
    if ( m_bRotation2 )
    {
        // Calculate rotation values for object 1
        RotationYaw   = D3DXToRadian( -25.0f * m_Timer.GetTimeElapsed() );
        RotationPitch = D3DXToRadian(  50.0f * m_Timer.GetTimeElapsed() );
        RotationRoll  = D3DXToRadian( -75.0f * m_Timer.GetTimeElapsed() );

        // Build rotation matrices 
        D3DXMatrixIdentity( &mtxRotate );
        D3DXMatrixRotationY( &mtxYaw, RotationYaw);
        D3DXMatrixRotationX( &mtxPitch,RotationPitch);
        D3DXMatrixRotationZ( &mtxRoll, RotationRoll);
        
        // Concatenate the rotation matrices
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxYaw );
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxPitch );
        D3DXMatrixMultiply( &mtxRotate, &mtxRotate, &mtxRoll );
            
        // Apply the rotation to our object's matrix
        D3DXMatrixMultiply( &m_pObject[ 1 ].m_mtxWorld, &mtxRotate, &m_pObject[ 1 ].m_mtxWorld );

    } // End if rotation enabled

}
