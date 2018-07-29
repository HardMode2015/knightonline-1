#include "../../Includes/Elements/CCharPart.h"
#include "../../Includes/Elements/CTexture.hpp"

CCharPart::CCharPart()
{

}

CCharPart::~CCharPart()
{

}

boolean CCharPart::LoadFromFile(LPTSTR szFN)
{
	m_szFileName = new char[strlen(szFN)];
	strcpy(m_szFileName, szFN);
	FILE * partFile = fopen(szFN, "rb");
	int nL;

	fread(&nL, sizeof(nL), 1, partFile);
	if (nL > 0)
	{
		fread(szFN, nL, 1, partFile);
		szFN[nL] = NULL;
	}

	fread(&nL, sizeof(nL), 1, partFile);
	fread(&m_pMaterial, sizeof(m_pMaterial), 1, partFile);

	fread(&nL, sizeof(nL), 1, partFile);
	if (nL > 0)
	{
		// texture name
		fread(szFN, nL, 1, partFile);
		szFN[nL] = NULL;

		m_pPartTexture = new CTexture();
		m_pPartTexture->LoadFromFile(szFN);
	}

	fread(&nL, sizeof(nL), 1, partFile);
	if (nL > 0)
	{
		// part model name
		fread(szFN, nL, 1, partFile);
		szFN[nL] = NULL;
		//s_MngSkins.Delete(&m_pSkinsRef);
		//m_pSkinsRef = s_MngSkins.Get(szFN);

		FILE * skinFile = fopen(szFN, "rb");

		fread(&nL, sizeof(nL), 1, skinFile);
		if (nL > 0)
		{
			// n3skin name
			fread(szFN, nL, 1, skinFile);
			szFN[nL] = NULL;
		}

		fread(&nL, sizeof(nL), 1, skinFile);
		if (nL > 0)
		{
			// n3skin name
			fread(szFN, nL, 1, skinFile);
			szFN[nL] = NULL;
		}

		int nUVC = 0;

		fread(&m_iFaceCount, sizeof(m_iFaceCount), 1, skinFile);
		fread(&m_iVertexCount, sizeof(m_iVertexCount), 1, skinFile);
		fread(&nUVC, sizeof(nUVC), 1, skinFile);

		if (m_iFaceCount > 0 && m_iVertexCount > 0)
		{

			m_pnPartVertexes = new __VertexXyzNormal[m_iVertexCount];
			m_pPartIndexes = new WORD[m_iFaceCount * 3];

			fread(m_pnPartVertexes, sizeof(__VertexXyzNormal), m_iVertexCount, skinFile);
			fread(m_pPartIndexes, 2 * 3, m_iFaceCount, skinFile);

			m_pPartVertexes = new __VertexT1[m_iFaceCount * 3];
		}

		if (nUVC > 0)
		{
			m_pPartUVs = new float[nUVC * 2];
			m_pPartUVIndexes = new WORD[m_iFaceCount * 3];

			fread(m_pPartUVs, sizeof(float), nUVC * 2, skinFile);
			fread(m_pPartUVIndexes, 2 * 3, m_iFaceCount, skinFile);
		}

		int n = 0, nVI = 0, nUVI = 0;
		if (nUVC > 0)
		{
			for (int i = 0; i < m_iFaceCount; i++)
			{
				n = i * 3 + 0; nVI = m_pPartIndexes[n]; nUVI = m_pPartUVIndexes[n];
				m_pPartVertexes[n].Set(m_pnPartVertexes[nVI].x, m_pnPartVertexes[nVI].y, m_pnPartVertexes[nVI].z,
					m_pnPartVertexes[nVI].n.x, m_pnPartVertexes[nVI].n.y, m_pnPartVertexes[nVI].n.z,
					m_pPartUVs[nUVI * 2], m_pPartUVs[nUVI * 2 + 1]);

				n = i * 3 + 1; nVI = m_pPartIndexes[n]; nUVI = m_pPartUVIndexes[n];
					m_pPartVertexes[n].Set(m_pnPartVertexes[nVI].x, m_pnPartVertexes[nVI].y, m_pnPartVertexes[nVI].z,
					m_pnPartVertexes[nVI].n.x, m_pnPartVertexes[nVI].n.y, m_pnPartVertexes[nVI].n.z,
					m_pPartUVs[nUVI * 2], m_pPartUVs[nUVI * 2 + 1]);

				n = i * 3 + 2; nVI = m_pPartIndexes[n]; nUVI = m_pPartUVIndexes[n];
				m_pPartVertexes[n].Set(m_pnPartVertexes[nVI].x, m_pnPartVertexes[nVI].y, m_pnPartVertexes[nVI].z,
					m_pnPartVertexes[nVI].n.x, m_pnPartVertexes[nVI].n.y, m_pnPartVertexes[nVI].n.z,
					m_pPartUVs[nUVI * 2], m_pPartUVs[nUVI * 2 + 1]);
			}
		}
		else
		{
			for (int i = 0; i < m_iFaceCount; i++)
			{
				n = i * 3 + 0; nVI = m_pPartIndexes[n];
				m_pPartVertexes[n].Set(m_pnPartVertexes[nVI], m_pnPartVertexes[nVI].n, 0, 0);

				n = i * 3 + 1; nVI = m_pPartIndexes[n];
				m_pPartVertexes[n].Set(m_pnPartVertexes[nVI], m_pnPartVertexes[nVI].n, 0, 0);

				n = i * 3 + 2; nVI = m_pPartIndexes[n];
				m_pPartVertexes[n].Set(m_pnPartVertexes[nVI], m_pnPartVertexes[nVI].n, 0, 0);
			}
		}
	}

	fclose(partFile);

	return true;
}