//-----------------------------------------------------------------------------
// File: CObject.cpp
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#include "../Includes/CMesh.h"

CMesh::CMesh()
{
	m_pVertex = nullptr;
	m_pIndex = nullptr;
	m_nVertexCount = 0;
	m_nIndexCount = 0;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

CMesh::CMesh(ULONG nVertexCount, ULONG nIndexCount) : CMesh()
{
	if (nVertexCount > 0) AddVertex(nVertexCount);
	if (nIndexCount > 0) AddIndex(nIndexCount);
}

CMesh::~CMesh()
{
	if (m_pVertex) delete[] m_pVertex;
	if (m_pIndex) delete[] m_pIndex;

	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pIndexBuffer) m_pIndexBuffer->Release();

	m_pVertex = nullptr;
	m_pIndex = nullptr;
	m_nVertexCount = 0;
	m_nIndexCount = 0;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

long CMesh::AddVertex(ULONG nCount)
{
	CVertex* pVertex;

	if (!(pVertex = new CVertex[m_nVertexCount + nCount])) return -1;

	if (m_pVertex) {
		memcpy(pVertex, m_pVertex, m_nVertexCount * sizeof(CVertex));
		delete[] m_pVertex;
	} // End if

	m_pVertex = pVertex;
	m_nVertexCount += nCount;

	return m_nVertexCount - nCount;
}

long CMesh::AddIndex(ULONG nCount)
{
	USHORT* pIndex;

	if (!(pIndex = new USHORT[m_nIndexCount + nCount])) return -1;

	if (m_pIndex) {
		memcpy(pIndex, m_pIndex, m_nIndexCount * sizeof(USHORT));
		delete[] m_pIndex;
	} // End if

	m_pIndex = pIndex;
	m_nIndexCount += nCount;

	return m_nIndexCount - nCount;
}

HRESULT CMesh::BuildBuffers(LPDIRECT3DDEVICE9 pD3DDevice, bool HardwareTnL, bool ReleaseOriginals)
{
	HRESULT hRet = S_OK;
	CVertex* pVertex = nullptr;
	USHORT* pIndex = nullptr;
	ULONG ulUsage = D3DUSAGE_WRITEONLY;

	// Should we use software vertex processing ?
	if (!HardwareTnL) ulUsage |= D3DUSAGE_SOFTWAREPROCESSING;

	// Release any previously allocated vertex / index buffers
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pIndexBuffer) m_pIndexBuffer->Release();
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;

	// Create our vertex buffer
	hRet = pD3DDevice->CreateVertexBuffer(sizeof(CVertex) * m_nVertexCount, ulUsage, D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
	if (FAILED(hRet)) return hRet;

	// Lock the vertex buffer ready to fill data
	hRet = m_pVertexBuffer->Lock(0, sizeof(CVertex) * m_nVertexCount, (void**)&pVertex, 0);
	if (FAILED(hRet)) return hRet;

	// Copy over the vertex data
	memcpy(pVertex, m_pVertex, sizeof(CVertex) * m_nVertexCount);

	// We are finished with the vertex buffer
	m_pVertexBuffer->Unlock();

	// Create our index buffer
	hRet = pD3DDevice->CreateIndexBuffer(sizeof(USHORT) * m_nIndexCount, ulUsage, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &m_pIndexBuffer, NULL);
	if (FAILED(hRet)) return hRet;

	// Lock the index buffer ready to fill data
	hRet = m_pIndexBuffer->Lock(0, sizeof(USHORT) * m_nIndexCount, (void**)&pIndex, 0);
	if (FAILED(hRet)) return hRet;

	// Copy over the index data
	memcpy(pIndex, m_pIndex, sizeof(USHORT) * m_nIndexCount);

	// We are finished with the indexbuffer
	m_pIndexBuffer->Unlock();

	// Calculate the mesh bounding box extents
	m_BoundsMin = D3DXVECTOR3(999999.0f, 999999.0f, 999999.0f);
	m_BoundsMax = D3DXVECTOR3(-999999.0f, -999999.0f, -999999.0f);
	for (ULONG i = 0; i < m_nVertexCount; ++i)
	{
		D3DXVECTOR3 * Pos = (D3DXVECTOR3*)&m_pVertex[i];
		if (Pos->x < m_BoundsMin.x) m_BoundsMin.x = Pos->x;
		if (Pos->y < m_BoundsMin.y) m_BoundsMin.y = Pos->y;
		if (Pos->z < m_BoundsMin.z) m_BoundsMin.z = Pos->z;
		if (Pos->x > m_BoundsMax.x) m_BoundsMax.x = Pos->x;
		if (Pos->y > m_BoundsMax.y) m_BoundsMax.y = Pos->y;
		if (Pos->z > m_BoundsMax.z) m_BoundsMax.z = Pos->z;

	} // Next Vertex

	  // Release old data if requested
	if (ReleaseOriginals)
	{
		// Release our mesh components
		if (m_pVertex) delete[]m_pVertex;
		if (m_pIndex) delete[]m_pIndex;

		// Clear variables
		m_pVertex = NULL;
		m_pIndex = NULL;
		m_nVertexCount = 0;
		m_nIndexCount = 0;

	} // End if ReleaseOriginals

	return S_OK;
}