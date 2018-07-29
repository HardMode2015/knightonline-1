#include "../../Includes/Scene/SceneLogin.hpp"
#include "../../Includes/Elements/CTexture.hpp"
#include "../../Includes/Elements/CCharPart.h"

SceneLogin::SceneLogin()
{
	
}

void SceneLogin::Init()
{
	m_tMoon = new CTexture();
	m_tMoon->LoadFromFile("Intro/moon.DXT");

	m_ptLogin = new CTexture();
	m_ptLogin->LoadFromFile("Ui_en/ui_login_en.DXT");

	FILE * hOgre = fopen("Intro/intro.n3chr", "rb");

	int nL = 0;
	fread(&nL, sizeof(nL), 1, hOgre);
	LPTSTR m_szName = new char[nL];
	if (nL > 0)
	{
		fread(&m_szName[0], sizeof(char), nL, hOgre);
		DBOUT("Intro char file name: " << m_szName);
	}

	m_mOgreWorld.Identity();

	__Vector3 m_vPos;
	__Quaternion m_qRot;
	__Vector3 m_vScale;

	fread(&m_vPos, sizeof(__Vector3), 1, hOgre);
	fread(&m_qRot, sizeof(__Quaternion), 1, hOgre);
	fread(&m_vScale, sizeof(__Vector3), 1, hOgre);

	m_mOgreWorld.Scale(m_vScale);
	if (m_qRot.w != 0)
	{
		static __Matrix44 mtxRot;
		D3DXMatrixRotationQuaternion(&mtxRot, &m_qRot);
		m_mOgreWorld *= mtxRot;
	}
	m_mOgreWorld.PosSet(m_vPos);

	int m_iAnimKeyPos;
	int m_iAnimKeyRot;
	int m_iAnimKeyScale;

	fread(&m_iAnimKeyPos, sizeof(m_iAnimKeyPos), 1, hOgre);
	fread(&m_iAnimKeyRot, sizeof(m_iAnimKeyRot), 1, hOgre);
	fread(&m_iAnimKeyScale, sizeof(m_iAnimKeyScale), 1, hOgre);
	/*if (m_nCount > 0)
	{
		ReadFile(hFile, &m_eType, 4, &dwRWC, NULL); // Key Type
		ReadFile(hFile, &m_fSamplingRate, 4, &dwRWC, NULL); // Sampling Rate

		this->Alloc(m_nCount, m_fSamplingRate, m_eType);
		if (KEY_VECTOR3 == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Vector3)*m_nCount, &dwRWC, NULL);
			__Vector3* pKeys = (__Vector3*)m_pDatas; // 끝에 하나더 복사해준다.
			pKeys[m_nCount] = pKeys[m_nCount - 1];
		}
		else if (KEY_QUATERNION == m_eType)
		{
			ReadFile(hFile, m_pDatas, sizeof(__Quaternion)*m_nCount, &dwRWC, NULL);
			__Quaternion* pKeys = (__Quaternion*)m_pDatas; // 끝에 하나더 복사해준다.
			pKeys[m_nCount] = pKeys[m_nCount - 1];
		}
	}*/

	nL = 0;
	char szFN[512] = "";

	fread(&nL, sizeof(nL), 1, hOgre);
	if (nL > 0)
	{
		//ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL; // 메시 파일 이름..
		//m_pMeshCollision = s_MngVMesh.Get(szFN);
	}

	fread(&nL, sizeof(nL), 1, hOgre);
	if (nL > 0)
	{
		//ReadFile(hFile, szFN, nL, &dwRWC, NULL); szFN[nL] = NULL; // 메시 파일 이름..
		//m_pMeshClimb = s_MngVMesh.Get(szFN);
	}

	fread(&nL, sizeof(nL), 1, hOgre);
	if (nL > 0)
	{
		fread(szFN, nL, 1, hOgre);
		szFN[nL] = NULL;
	}

	fread(&m_iOgrePartCount, sizeof(m_iOgrePartCount), 1, hOgre);
	if (m_iOgrePartCount > 0) {
		m_pOgreParts = new CCharPart[m_iOgrePartCount];

		for (int i = 0; i < m_iOgrePartCount; i++)
		{
			nL = 0;
			fread(&nL, sizeof(nL), 1, hOgre);
			if (nL > 0)
			{
				fread(&szFN, nL, 1, hOgre);
				szFN[nL] = NULL;
				m_pOgreParts[i].LoadFromFile(szFN);
			}
		}
	}

	/*
	fread(&iPC, sizeof(iPC), 1, hOgre);
	if (iPC > 0) {
		// plug allocation

		// axe n3plug

		for (int i = 0; i < iPC; i++) {
			nL = 0;
			fread(&nL, sizeof(nL), 1, hOgre);
			if (nL > 0)
			{
				fread(&szFN, nL, 1, hOgre);
				szFN[nL] = NULL;
				//m_Parts[i]->LoadFromFile(szFN);
			}
		}
	}

	int * MAX_CHAR_ANI_PARTS_START = new int[2];
	int * MAX_CHAR_ANI_PARTS_END = new int[2];

	fread(&nL, sizeof(nL), 1, hOgre);
	if (nL > 0)
	{
		// read animation control set
		fread(&szFN, nL, 1, hOgre);
		szFN[nL] = NULL;
	}
	*/
	fclose(hOgre);

	// todo: fx plug bytes

	DBOUT("test");

	ZeroMemory(&m_iLight1, sizeof(D3DLIGHT9));
	ZeroMemory(&m_iLight2, sizeof(D3DLIGHT9));
	ZeroMemory(&m_iLight3, sizeof(D3DLIGHT9));

	m_iLight1.Type = D3DLIGHT_POINT;
	m_iLight1.Diffuse = D3DXCOLOR(0.909f, 0.886f, 0.843f, 1.0f);
	m_iLight1.Position = D3DXVECTOR3(-500.0f, 100.0f, -50.0f);
	m_iLight1.Direction = D3DXVECTOR3(-1.0f, -1.0f, 0.5f);
	m_iLight1.Range = 1000000.00;
	m_iLight1.Attenuation0 = 0.5;

	m_iLight2.Type = D3DLIGHT_POINT;
	m_iLight2.Diffuse = D3DXCOLOR(0.258f, 0.266f, 0.658f, 0.0f);
	m_iLight2.Position = D3DXVECTOR3(1000.0f, -300.0f, 50.0f);
	m_iLight2.Direction = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_iLight2.Range = 1000000.00;
	m_iLight2.Attenuation0 = 1;

	m_iLight3.Type = D3DLIGHT_POINT;
	m_iLight3.Diffuse = D3DXCOLOR(0.486f, 0.176f, 0.121f, 0.0f);
	m_iLight3.Position = D3DXVECTOR3(-200.0f, -800.0f, 350.0f);
	m_iLight3.Direction = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_iLight3.Range = 1000000.00;
	m_iLight3.Attenuation0 = 1;
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

	s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);

	// OGRE RENDER PART
	__Matrix44 matProjection;
	::D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian(45.0f), (float)600 / 800, 0.1f, 256.0f);
	s_lpD3DDev->SetTransform(D3DTS_PROJECTION, &matProjection);

	__Matrix44 matView;
	D3DXMatrixLookAtLH(&matView, &__Vector3(0.22f, 0.91f, -1.63f), &__Vector3(-0.19f, 1.1f, 0.09f), &__Vector3(0, 1, 0));
	s_lpD3DDev->SetTransform(D3DTS_VIEW, &matView);

	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_mOgreWorld);
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, true);

	s_lpD3DDev->SetLight(0, &m_iLight1);
	s_lpD3DDev->SetLight(1, &m_iLight2);
	s_lpD3DDev->SetLight(2, &m_iLight3);
	s_lpD3DDev->LightEnable(0, true);
	s_lpD3DDev->LightEnable(1, true);
	s_lpD3DDev->LightEnable(2, true);

	for (int i = 0; i < m_iOgrePartCount; i++) {
		CBase::s_lpD3DDev->SetFVF(FVF_VNT1);
		CBase::s_lpD3DDev->SetMaterial(&m_pOgreParts[i].m_pMaterial);

		CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE	);
		CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		CBase::s_lpD3DDev->SetTexture(0, m_pOgreParts[i].m_pPartTexture->Get());
		CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_pOgreParts[i].m_iFaceCount, m_pOgreParts[i].m_pPartVertexes, sizeof(__VertexT1));
	}

	s_lpD3DDev->LightEnable(0, FALSE);
	s_lpD3DDev->LightEnable(1, FALSE);
	s_lpD3DDev->LightEnable(2, FALSE);
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, false);
	// OGRE RENDER END

	// LOGIN BOTTOM UI START
	CBase::s_lpD3DDev->SetTexture(0, m_ptLogin->Get());
	CBase::s_lpD3DDev->SetFVF(FVF_TRANSFORMED);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	__VertexTransformed vLogin[4];
	vLogin[0].Set((float)510 / 979 * vp.Width, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.005859f, 0.115234f);
	vLogin[1].Set(vp.Width, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.921875f, 0.115234f);
	vLogin[2].Set(vp.Width, vp.Height, 0, fRHW, 0xffffffff, 0.921875f, 0.218750f);
	vLogin[3].Set((float)510 / 979 * vp.Width, vp.Height, 0, fRHW, 0xffffffff, 0.005859f, 0.218750f);
	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vLogin, sizeof(__VertexTransformed));

	vLogin[0].Set(0, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.005859f, 0.005859f);
	vLogin[1].Set((float)510 / 979 * vp.Width, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.921875f, 0.005859f);
	vLogin[2].Set((float)510 / 979 * vp.Width, vp.Height, 0, fRHW, 0xffffffff, 0.921875f, 0.109375f);
	vLogin[3].Set(0, vp.Height, 0, fRHW, 0xffffffff, 0.005859f, 0.109375f);
	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vLogin, sizeof(__VertexTransformed));
	// LOGIN BOTTOM UI END
}