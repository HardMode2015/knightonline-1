#include "../../Includes/Scene/SceneLogin.hpp"
#include "../../Includes/Elements/CTexture.hpp"

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

	__Vector3 m_vPos;
	__Quaternion m_qRot;
	__Vector3 m_vScale;

	fread(&m_vPos, sizeof(__Vector3), 1, hOgre);
	fread(&m_qRot, sizeof(__Quaternion), 1, hOgre);
	fread(&m_vScale, sizeof(__Vector3), 1, hOgre);

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


	int iPC = 0;
	fread(&iPC, sizeof(iPC), 1, hOgre);
	if (iPC > 0) {
		// part allocation

		for (int i = 0; i < iPC; i++)
		{
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

	// todo: fx plug bytes

	DBOUT("test");
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

	__VertexTransformed vLogin[4];
	vLogin[0].Set((float)510 / 979 * vp.Width,	(float)vp.Height - 53	, 0, fRHW, 0xffffffff, 0.005859f, 0.115234f);
	vLogin[1].Set(vp.Width,				(float)vp.Height - 53	, 0, fRHW, 0xffffffff, 0.921875f, 0.115234f);
	vLogin[2].Set(vp.Width,				vp.Height						, 0, fRHW, 0xffffffff, 0.921875f, 0.218750f);
	vLogin[3].Set((float)510 / 979 * vp.Width,	vp.Height						, 0, fRHW, 0xffffffff, 0.005859f, 0.218750f);

	CBase::s_lpD3DDev->SetTexture(0, m_ptLogin->Get());
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CBase::s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vLogin, sizeof(__VertexTransformed));

	vLogin[0].Set(0, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.005859f, 0.005859f);
	vLogin[1].Set((float)510 / 979 * vp.Width, (float)vp.Height - 53, 0, fRHW, 0xffffffff, 0.921875f, 0.005859f);
	vLogin[2].Set((float)510 / 979 * vp.Width, vp.Height, 0, fRHW, 0xffffffff, 0.921875f, 0.109375f);
	vLogin[3].Set(0, vp.Height, 0, fRHW, 0xffffffff, 0.005859f, 0.109375f);

	CBase::s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vLogin, sizeof(__VertexTransformed));


	CBase::s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWrite);
}