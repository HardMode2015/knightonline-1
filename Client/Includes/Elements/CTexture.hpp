#ifndef _CTEXTURE_H_
#define _CTEXTURE_H_

#include "../CBase.hpp"

class CTexture : public CBase
{
public:
	typedef struct __DXT_HEADER
	{
		char szID[4]; // Texture file version
		int nWidth;
		int nHeight;
		D3DFORMAT Format; // 0 - 압축 안함 1 ~ 5 : D3DFMT_DXT1 ~ D3DFMT_DXT5
		BOOL bMipMap; // Mip Map ??
	} __DxtHeader;

	int m_iLOD;

	CTexture();
	virtual ~CTexture();

	int Width() { return m_Header.nWidth; };
	int Height() { return m_Header.nHeight; };
	D3DFORMAT PixelFormat() { return m_Header.Format; };
	int MipMapCount() { if (m_lpTexture) return m_lpTexture->GetLevelCount(); else return 0; };

	bool Create(int nWidth, int nHeight, D3DFORMAT Format, BOOL bGenerateMipMap);

	LPDIRECT3DTEXTURE9	Get() { return m_lpTexture; }
	operator LPDIRECT3DTEXTURE9 () { return m_lpTexture; }

	void LoadFromFile(LPTSTR filePath);

	__DXT_HEADER m_Header;
	LPDIRECT3DTEXTURE9 m_lpTexture = nullptr;

protected:

private:

};

#endif