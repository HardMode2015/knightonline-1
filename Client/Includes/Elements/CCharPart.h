#ifndef _CCHAR_PART_H_
#define _CCHAR_PART_H_

#include "../CBase.hpp"

class CCharPart : public CBase
{
public:
	CCharPart();
	virtual ~CCharPart();

	boolean LoadFromFile(LPTSTR szFN);

	class CTexture * m_pPartTexture;

	int m_iFaceCount = 0;
	int m_iVertexCount = 0;

	LPTSTR m_szFileName;

	__Material m_pMaterial;
	__VertexXyzNormal * m_pnPartVertexes = nullptr;
	WORD * m_pPartIndexes = nullptr;
	__VertexT1 * m_pPartVertexes = nullptr;
	float * m_pPartUVs = nullptr;
	WORD * m_pPartUVIndexes = nullptr;
protected:

private:

};

#endif