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

	__DXT_HEADER HeaderOrg; // 헤더를 저장해 놓고..
	ReadFile(hFile, &HeaderOrg, sizeof(HeaderOrg), &dwRWC, NULL); // 헤더를 읽는다..
	if ('N' != HeaderOrg.szID[0] || 'T' != HeaderOrg.szID[1] || 'F' != HeaderOrg.szID[2] || 3 != HeaderOrg.szID[3]) // "NTF"3 - Noah Texture File Ver. 3.0
	{
#ifdef _N3GAME
		CLogWriter::Write("N3Texture Warning - Old format DXT file (%s)", m_szFileName.c_str());
#endif
	}

	// DXT Format 을 읽어야 하는데 지원이 되는지 안되는지 보고 지원안되면 대체 포맷을 정한다.
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
	if (fmtNew != HeaderOrg.Format) { iWCreate /= 2; iHCreate /= 2; }// DXT 지원이 안되면 너비 높이를 줄인다.
	if (m_iLOD > 0 && m_iLOD <= 2 && HeaderOrg.nWidth >= 16 && HeaderOrg.nHeight >= 16) // LOD 만큼 작게 생성..
	{
		for (int i = 0; i < m_iLOD; i++)
		{
			iWCreate /= 2;
			iHCreate /= 2;
		}
	}
	else m_iLOD = 0; // LOD 적용이 아니면..

	int iLODPrev = m_iLOD;
	this->Create(iWCreate, iHCreate, fmtNew, HeaderOrg.bMipMap); // 서피스 만들고..
	m_iLOD = iLODPrev;

	if (m_lpTexture == NULL)
	{
		DBOUT("Texture not initialized")
	}

	D3DSURFACE_DESC sd;
	D3DLOCKED_RECT LR;
	int iMMC = m_lpTexture->GetLevelCount(); // 생성한 MipMap 수

											 // 압축 포맷이면..
	if (D3DFMT_DXT1 == HeaderOrg.Format ||
		D3DFMT_DXT2 == HeaderOrg.Format ||
		D3DFMT_DXT3 == HeaderOrg.Format ||
		D3DFMT_DXT4 == HeaderOrg.Format ||
		D3DFMT_DXT5 == HeaderOrg.Format)
	{
		if (TRUE == bDXTSupport) // 압축 텍스처 지원이면..
		{
			if (iMMC > 1)
			{
				if (m_iLOD > 0) // LOD 만큼 건너뛰기...
				{
					int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
					{
						if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; // DXT1 형식은 16비트 포맷에 비해 1/4 로 압축..
						else iSkipSize += iWTmp * iHTmp; // DXT2 ~ DXT5 형식은 16비트 포맷에 비해 1/2 로 압축..
					}
					::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // 건너뛰고.
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

					ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, NULL); // 일렬로 된 데이터를 쓰고..
					m_lpTexture->UnlockRect(i);
				}

				// 텍스처 압축안되는 비디오 카드를 위한 여분의 데이터 건너뛰기.. 
				int iWTmp = HeaderOrg.nWidth / 2, iHTmp = HeaderOrg.nHeight / 2;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2) // 한픽셀에 두바이트가 들어가는 A1R5G5B5 혹은 A4R4G4B4 포맷으로 되어 있다..
					::SetFilePointer(hFile, iWTmp * iHTmp * 2, 0, FILE_CURRENT); // 건너뛰고.
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

				ReadFile(hFile, (BYTE*)LR.pBits, iTexSize, &dwRWC, NULL); // 일렬로 된 데이터를 쓰고..
				m_lpTexture->UnlockRect(0);

				// 텍스처 압축안되는 비디오 카드를 위한 여분의 데이터 건너뛰기.. 
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight / 4, 0, FILE_CURRENT); // 건너뛰고.
				if (HeaderOrg.nWidth >= 1024) SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); // 사이즈가 512 보다 클경우 부두용 데이터 건너뛰기..
			}
		}
		else // DXT 지원이 안되면..
		{
			if (iMMC > 1) // LOD 만큼 건너뛰기...
			{
				// 압축 데이터 건너뛰기..
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (; iWTmp >= 4 && iHTmp >= 4; iWTmp /= 2, iHTmp /= 2)
				{
					if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize += iWTmp * iHTmp / 2; // DXT1 형식은 16비트 포맷에 비해 1/4 로 압축..
					else iSkipSize += iWTmp * iHTmp; // DXT2 ~ DXT5 형식은 16비트 포맷에 비해 1/2 로 압축..
				}
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // 건너뛰고.

																	 // LOD 만큼 건너뛰기..
				iWTmp = HeaderOrg.nWidth / 2; iHTmp = HeaderOrg.nHeight / 2; iSkipSize = 0;
				if (m_iLOD > 0)
				{
					for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2)
						iSkipSize += iWTmp * iHTmp * 2; // 피치에 너비를 나눈게 픽셀의 크기라 생각한다...
				}

				// 비디오 카드 지원 텍스처 크기가 작을경우 건너뛰기..
				for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
					iSkipSize += iWTmp * iHTmp * 2;
				if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // 건너뛰고.

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
				// 압축 데이터 건너뛰기..
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				if (D3DFMT_DXT1 == HeaderOrg.Format) iSkipSize = iWTmp * iHTmp / 2; // DXT1 형식은 16비트 포맷에 비해 1/4 로 압축..
				else iSkipSize = iWTmp * iHTmp; // DXT2 ~ DXT5 형식은 16비트 포맷에 비해 1/2 로 압축..

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
			if (m_iLOD > 0) // LOD 만큼 건너뛰기...
			{
				int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
				for (int i = 0; i < m_iLOD; i++, iWTmp /= 2, iHTmp /= 2) iSkipSize += iWTmp * iHTmp * iPixelSize; // 피치에 너비를 나눈게 픽셀의 크기라 생각한다...
				::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // 건너뛰고.
			}

			// 비디오 카드 지원 텍스처 크기가 작을경우 건너뛰기..
			int iWTmp = HeaderOrg.nWidth, iHTmp = HeaderOrg.nHeight, iSkipSize = 0;
			for (; iWTmp > s_DevCaps.MaxTextureWidth || iHTmp > s_DevCaps.MaxTextureHeight; iWTmp /= 2, iHTmp /= 2)
				iSkipSize += iWTmp * iHTmp * iPixelSize;
			if (iSkipSize) ::SetFilePointer(hFile, iSkipSize, 0, FILE_CURRENT); // 건너뛰고.

																				// 데이터 읽기..
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
			// 비디오 카드 지원 텍스처 크기가 작을경우 건너뛰기..
			if (HeaderOrg.nWidth >= 512 && m_Header.nWidth <= 256)
				::SetFilePointer(hFile, HeaderOrg.nWidth * HeaderOrg.nHeight * iPixelSize, 0, FILE_CURRENT); // 건너뛰고.

			m_lpTexture->GetLevelDesc(0, &sd);
			m_lpTexture->LockRect(0, &LR, NULL, NULL);
			for (int y = 0; y < (int)sd.Height; y++)
				ReadFile(hFile, (BYTE*)LR.pBits + y * LR.Pitch, iPixelSize * sd.Width, &dwRWC, NULL);
			m_lpTexture->UnlockRect(0);

			if (m_Header.nWidth >= 512 && m_Header.nHeight >= 512)
				SetFilePointer(hFile, 256 * 256 * 2, 0, FILE_CURRENT); // 사이즈가 512 보다 클경우 부두용 데이터 건너뛰기..
		}
	}
}

bool CTexture::Create(int nWidth, int nHeight, D3DFORMAT Format, BOOL bGenerateMipMap)
{
	if (nWidth != nHeight)

		if (nWidth <= 1 || nHeight <= 1 || D3DFMT_UNKNOWN == Format) return false;

	if (s_dwTextureCaps & TEX_CAPS_POW2) // 2 의 승수만 된다면..
	{
		int nW, nH;
		for (nW = 1; nW <= nWidth; nW *= 2); nW /= 2;
		for (nH = 1; nH <= nHeight; nH *= 2); nH /= 2;

		nWidth = nW;
		nHeight = nH;
	}

	if ((s_dwTextureCaps & TEX_CAPS_SQUAREONLY) && nWidth != nHeight) // 정사각형 텍스처만 되면..
	{
		if (nWidth > nHeight) nWidth = nHeight;
		else nHeight = nWidth;
	}

	// 비디오 카드가 256 이상의 텍스처를 지원 하지 못하면..
	if (nWidth > 256 && s_DevCaps.MaxTextureWidth <= 256) nWidth = s_DevCaps.MaxTextureWidth;
	if (nHeight > 256 && s_DevCaps.MaxTextureHeight <= 256) nHeight = s_DevCaps.MaxTextureHeight;

	// 헤더 세팅..
	memset(&m_Header, 0, sizeof(m_Header));

	// MipMap 단계 결정..
	// 4 X 4 픽셀까지만 MipMap 을 만든다..
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