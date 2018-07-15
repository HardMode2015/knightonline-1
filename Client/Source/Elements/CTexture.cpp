#include "../../Includes/Elements/CTexture.hpp"
#include <stdio.h>

CTexture::CTexture()
{
	memset(&m_Header, 0, sizeof(__DXT_HEADER));
	m_lpTexture = nullptr;
}

CTexture::~CTexture()
{
	if (m_lpTexture)
	{
		int nRefCount = m_lpTexture->Release();
		if (nRefCount == 0) m_lpTexture = nullptr;
	}
}

void CTexture::LoadFromFile(LPTSTR filePath)
{
	HANDLE hFile = ::CreateFile(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	LPTSTR m_szName = "";

	DWORD dwRWC = 0;
	int nL = 0;
	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
	if (nL > 0)
	{
		//std::vector<char> buffer(nL + 1, NULL);
		//ReadFile(hFile, &buffer[0], nL, &dwRWC, NULL);
		//m_szName = &buffer[0];
	}

	dwRWC = 0;

	__DXT_HEADER HeaderOrg; // ����� ������ ����..
	ReadFile(hFile, &HeaderOrg, sizeof(HeaderOrg), &dwRWC, NULL); // ����� �д´�..
	if ('N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3]) // "NTF"3 - Noah Texture File Ver. 3.0
	{
#ifdef _N3GAME
		CLogWriter::Write("N3Texture Warning - Old format DXT file (%s)", m_szFileName.c_str());
#endif
	}

	// DXT Format �� �о�� �ϴµ� ������ �Ǵ��� �ȵǴ��� ���� �����ȵǸ� ��ü ������ ���Ѵ�.
	bool bDXTSupport = FALSE;
	D3DFORMAT fmtNew = HeaderOrg.Format;
	if (D3DFMT_DXT1 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT1) bDXTSupport = true;
		else fmtNew = D3DFMT_A1R5G5B5;
	}
	else if (D3DFMT_DXT2 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT2) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT3 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT3) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT4 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT4) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}
	else if (D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if (s_dwTextureCaps & TEX_CAPS_DXT5) bDXTSupport = true;
		else fmtNew = D3DFMT_A4R4G4B4;
	}

	int iWCreate = HeaderOrg.nWidth, iHCreate = HeaderOrg.nHeight;
	if (fmtNew != HeaderOrg.Format) { iWCreate /= 2; iHCreate /= 2; }// DXT ������ �ȵǸ� �ʺ� ���̸� ���δ�.
	if (m_iLOD > 0 && m_iLOD <= 2 && HeaderOrg.nWidth >= 16 && HeaderOrg.nHeight >= 16) // LOD ��ŭ �۰� ����..
	{
		for (int i = 0; i < m_iLOD; i++)
		{
			iWCreate /= 2;
			iHCreate /= 2;
		}
	}
	else m_iLOD = 0; // LOD ������ �ƴϸ�..

	int iLODPrev = m_iLOD;
	this->Create(iWCreate, iHCreate, fmtNew, HeaderOrg.bMipMap); // ���ǽ� �����..
	m_iLOD = iLODPrev;

	if (m_lpTexture == NULL)
	{
		DBOUT("Texture not initialized")
	}

	D3DSURFACE_DESC sd;
	D3DLOCKED_RECT LR;
	int iMMC = m_lpTexture->GetLevelCount(); // ������ MipMap ��

											 // ���� �����̸�..
	if (D3DFMT_DXT1 == HeaderOrg.Format ||
		D3DFMT_DXT2 == HeaderOrg.Format ||
		D3DFMT_DXT3 == HeaderOrg.Format ||
		D3DFMT_DXT4 == HeaderOrg.Format ||
		D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if (TRUE == bDXTSupport) // ���� �ؽ�ó �����̸�..
		{
			if (iMMC > 1)
			{
				if (m_iLOD > 0) // LOD ��ŭ �ǳʶٱ�...
				{
					int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
					{
						if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; // DXT1 ������ 16��Ʈ ���˿� ���� 1/4 �� ����..
						else iSkipSize += iWTmp * iHTmp; // DXT2 ~ DXT5 ������ 16��Ʈ ���˿� ���� 1/2 �� ����..
					}
					::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // �ǳʶٰ�.
				}

				for (int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);
					m_lpTexture->LockRect(i, &LR, NULL, NULL);

					int iTexSize = 0;
					switch (HeaderOrg.Format) {
					case D3DFMT_DXT1: {
						iTexSize = (sd.Width*sd.Height / 2);
					} break;
					case D3DFMT_DXT2: {
						exit(-1);
					} break;
					case D3DFMT_DXT3: {
						iTexSize = (sd.Width*sd.Height);
					} break;
					case D3DFMT_DXT4: {
						exit(-1);
					} break;
					case D3DFMT_DXT5: {
						iTexSize = (sd.Width*sd.Height * 2);
						exit(-1);
					} break;
					}

					ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, NULL); // �Ϸķ� �� �����͸� ����..
					m_lpTexture->UnlockRect(i);
				}

				// �ؽ�ó ����ȵǴ� ���� ī�带 ���� ������ ������ �ǳʶٱ�.. 
				int iWTmp = HeaderOrg.nWidth / 2, iHTmp = HeaderOrg.nHeight / 2;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2) // ���ȼ��� �ι���Ʈ�� ���� A1R5G5B5 Ȥ�� A4R4G4B4 �������� �Ǿ� �ִ�..
					::SetFilePointer(hFile, iWTmp * iHTmp * 2, 0, FILE_CURRENT); // �ǳʶٰ�.
			}
			else // pair of if(iMMC > 1)
			{
				m_lpTexture->GetLevelDesc(0, &sd);
				m_lpTexture->LockRect(0, &LR, NULL, NULL);

				int iTexSize = 0;
				switch (HeaderOrg.Format) {
				case D3DFMT_DXT1: {
					iTexSize = (sd.Width*sd.Height / 2);
				} break;
				case D3DFMT_DXT2: {
					exit(-1);
				} break;
				case D3DFMT_DXT3: {
					iTexSize = (sd.Width*sd.Height);
				} break;
				case D3DFMT_DXT4: {
					exit(-1);
				} break;
				case D3DFMT_DXT5: {
					iTexSize = (sd.Width*sd.Height * 2);
					exit(-1);
				} break;
				}

				ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, NULL); // �Ϸķ� �� �����͸� ����..
				m_lpTexture->UnlockRect(0);

				// �ؽ�ó ����ȵǴ� ���� ī�带 ���� ������ ������ �ǳʶٱ�.. 
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight / 4, 0, FILE_CURRENT); // �ǳʶٰ�.
				if (HeaderOrg.nWidth >= 1024) SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); // ����� 512 ���� Ŭ��� �εο� ������ �ǳʶٱ�..
			}
		}
		else // DXT ������ �ȵǸ�..
		{
			if (iMMC > 1) // LOD ��ŭ �ǳʶٱ�...
			{
				// ���� ������ �ǳʶٱ�..
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				{
					if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; // DXT1 ������ 16��Ʈ ���˿� ���� 1/4 �� ����..
					else iSkipSize += iWTmp * iHTmp; // DXT2 ~ DXT5 ������ 16��Ʈ ���˿� ���� 1/2 �� ����..
				}
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // �ǳʶٰ�.

																	 // LOD ��ŭ �ǳʶٱ�..
				iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2; iSkipSize = 0;
				if (m_iLOD > 0)
				{
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
						iSkipSize += iWTmp * iHTmp * 2; // ��ġ�� �ʺ� ������ �ȼ��� ũ��� �����Ѵ�...
				}

				// ���� ī�� ���� �ؽ�ó ũ�Ⱑ ������� �ǳʶٱ�..
				for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
					iSkipSize += iWTmp * iHTmp * 2;
				if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // �ǳʶٰ�.

				for (int i = 0; i < iMMC; i++)
				{
					m_lpTexture->GetLevelDesc(i, &sd);
					m_lpTexture->LockRect(i, &LR, NULL, NULL);
					int nH = sd.Height;
					for (int y = 0; y < nH; y++)
						ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, 2 * sd.Width, &dwRWC, NULL);
					m_lpTexture->UnlockRect(i);
				}
			}
			else // pair of if(iMMC > 1)
			{
				// ���� ������ �ǳʶٱ�..
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize = iWTmp * iHTmp / 2; // DXT1 ������ 16��Ʈ ���˿� ���� 1/4 �� ����..
				else iSkipSize = iWTmp * iHTmp; // DXT2 ~ DXT5 ������ 16��Ʈ ���˿� ���� 1/2 �� ����..

			}
		}
	}
	else
	{
		int iPixelSize = 0;
		if (fmtNew == D3DFMT_A1R5G5B5 || fmtNew == D3DFMT_A4R4G4B4) iPixelSize = 2;
		else if (fmtNew == D3DFMT_R8G8B8) iPixelSize = 3;
		else if (fmtNew == D3DFMT_A8R8G8B8 || fmtNew == D3DFMT_X8R8G8B8) iPixelSize = 4;
		else
		{
			__ASSERT(0, "Not supported texture format");
		}

		if (iMMC > 1)
		{
			if (m_iLOD > 0) // LOD ��ŭ �ǳʶٱ�...
			{
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2) iSkipSize += iWTmp * iHTmp * iPixelSize; // ��ġ�� �ʺ� ������ �ȼ��� ũ��� �����Ѵ�...
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // �ǳʶٰ�.
			}

			// ���� ī�� ���� �ؽ�ó ũ�Ⱑ ������� �ǳʶٱ�..
			int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
			for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
			if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // �ǳʶٰ�.

																				// ������ �б�..
			for (int i = 0; i < iMMC; i++)
			{
				m_lpTexture->GetLevelDesc(i, &sd);
				m_lpTexture->LockRect(i, &LR, NULL, NULL);
				for (int y = 0; y < (int)sd.Height; y++)
					ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, iPixelSize * sd.Width, &dwRWC, NULL);
				m_lpTexture->UnlockRect(i);
			}
		}
		else // pair of if(iMMC > 1)
		{
			// ���� ī�� ���� �ؽ�ó ũ�Ⱑ ������� �ǳʶٱ�..
			if (HeaderOrg.nWidth >= 512 && m_Header.nWidth <= 256)
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight * iPixelSize, 0, FILE_CURRENT); // �ǳʶٰ�.

			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->LockRect(0, &LR, NULL, NULL);
			for (int y = 0; y < (int)sd.Height; y++)
				ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, iPixelSize * sd.Width, &dwRWC, NULL);
			m_lpTexture->UnlockRect(0);

			if (m_Header.nWidth >= 512 && m_Header.nHeight >= 512)
				SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); // ����� 512 ���� Ŭ��� �εο� ������ �ǳʶٱ�..
		}
	}
}

bool CTexture::Create(int nWidth, int nHeight, D3DFORMAT Format, BOOL bGenerateMipMap)
{
	if (nWidth != nHeight)

		if (nWidth <= 1 || nHeight <= 1 || D3DFMT_UNKNOWN == Format) return false;

	if (s_dwTextureCaps & TEX_CAPS_POW2) // 2 �� �¼��� �ȴٸ�..
	{
		int nW, nH;
		for (nW = 1; nW <= nWidth; nW *= 2); nW /= 2;
		for (nH = 1; nH <= nHeight; nH *= 2); nH /= 2;

		nWidth = nW;
		nHeight = nH;
	}

	if ((s_dwTextureCaps & TEX_CAPS_SQUAREONLY) && nWidth != nHeight) // ���簢�� �ؽ�ó�� �Ǹ�..
	{
		if (nWidth > nHeight) nWidth = nHeight;
		else nHeight = nWidth;
	}

	// ���� ī�尡 256 �̻��� �ؽ�ó�� ���� ���� ���ϸ�..
	if (nWidth > 256 && s_DevCaps.MaxTextureWidth <= 256) nWidth = s_DevCaps.MaxTextureWidth;
	if (nHeight > 256 && s_DevCaps.MaxTextureHeight <= 256) nHeight = s_DevCaps.MaxTextureHeight;

	// ��� ����..
	memset(&m_Header, 0, sizeof(m_Header));

	// MipMap �ܰ� ����..
	// 4 X 4 �ȼ������� MipMap �� �����..
	int nMMC = 1;
	if (bGenerateMipMap)
	{
		nMMC = 0;
		for (int nW = nWidth, nH = nHeight; nW >= 4 && nH >= 4; nW /= 2, nH /= 2) nMMC++;
	}

	HRESULT rval = s_lpD3DDev->CreateTexture(nWidth, nHeight, nMMC, 0, Format, D3DPOOL_MANAGED, &m_lpTexture, NULL);

	if (rval == D3DERR_INVALIDCALL)
	{
		DBOUT("N3Texture: createtexture err D3DERR_INVALIDCALL()");
		return false;
	}
	if (rval == D3DERR_OUTOFVIDEOMEMORY)
	{
		DBOUT("N3Texture: createtexture err D3DERR_OUTOFVIDEOMEMORY(%s)");
		return false;
	}
	if (rval == E_OUTOFMEMORY)
	{
		DBOUT("N3Texture: createtexture err E_OUTOFMEMORY(m_szFileName.c_str())");
		return false;
	}

	if (nullptr == m_lpTexture)
	{
		DBOUT("Texture lp null");
		return false;
	}

	m_Header.nWidth = nWidth;
	m_Header.nHeight = nHeight;
	m_Header.Format = Format;
	m_Header.bMipMap = bGenerateMipMap;

	return true;
}