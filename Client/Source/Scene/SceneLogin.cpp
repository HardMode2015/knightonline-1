#include "../../Includes/Scene/SceneLogin.hpp"
#include "../../Includes/Elements/CTexture.hpp"

SceneLogin::SceneLogin()
{
	
}

void SceneLogin::Init()
{
	m_tMoon = new CTexture();
	m_tMoon->LoadFromFile("Intro/moon.DXT");
}

void SceneLogin::Render()
{
	D3DVIEWPORT9 vp;
	CBase::s_lpD3DDev->GetViewport(&vp);

	float fMW = (m_tMoon->Width() * vp.Width / 1024.0f)*1.3f;
	float fMH = (m_tMoon->Height() * vp.Height / 768.0f)*1.3f;
	float fX = 100.0f * vp.Width / 1024.0f;
	float fY = 50.0f * vp.Height / 768.0f;

	float fRHW = 1.0f;
	__VertexTransformed vMoon[4];
	vMoon[0].Set(fX, fY, 0, fRHW, 0xffffffff, 0.0f, 0.0f);
	vMoon[1].Set(fX + fMW, fY, 0, fRHW, 0xffffffff, 1.0f, 0.0f);
	vMoon[2].Set(fX + fMW, fY + fMH, 0, fRHW, 0xffffffff, 1.0f, 1.0f);
	vMoon[3].Set(fX, fY + fMH, 0, fRHW, 0xffffffff, 0.0f, 1.0f);

	DWORD dwZWrite;
	CBase::s_lpD3DDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWrite);
	CBase::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CBase::s_lpD3DDev->SetTexture(0, m_tMoon->Get());
	CBase::s_lpD3DDev->SetFVF(FVF_TRANSFORMED);
	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vMoon, sizeof(__VertexTransformed));

	CBase::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
}