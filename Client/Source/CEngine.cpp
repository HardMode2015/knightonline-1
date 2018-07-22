#include "../Includes/CEngine.hpp"

#include "../Includes/Scene/SceneBase.hpp"
#include "../Includes/Scene/SceneLogin.hpp"

CEngine::CEngine()
{

}

CEngine::~CEngine()
{
	
}

int CEngine::StartGame()
{
	CBase::s_pSceneManager->SetActiveScene((SceneBase *)s_pSceneManager->s_pSceneLogin);

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
	CBase::m_Timer.Tick();

	// Skip if app is inactive
	if (!m_bActive) return;

	// Recover lost device if required
	if (m_bLostDevice)
	{
		// Can we reset the device yet ?
		HRESULT hRet = s_lpD3DDev->TestCooperativeLevel();
		if (hRet == D3DERR_DEVICENOTRESET)
		{
			// Restore the device
			s_lpD3DDev->Reset(&s_DevParam);
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

	s_pSceneManager->TickActive();

	// Clear the frame & depth buffer ready for drawing
	s_lpD3DDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	// Begin Scene Rendering
	s_lpD3DDev->BeginScene();

	// Render Scene
	s_pSceneManager->RenderActive();

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
	s_lpD3DDev->EndScene();

	// Present the buffer
	if (FAILED(s_lpD3DDev->Present(nullptr, nullptr, nullptr, nullptr))) m_bLostDevice = true;
}