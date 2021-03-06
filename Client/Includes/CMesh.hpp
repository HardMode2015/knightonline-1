//-----------------------------------------------------------------------------
// File: CObject.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CMESH_H_
#define _CMESH_H_

#include "Main.hpp"

class CVertex
{
public:
    CVertex(float fX, float fY, float fZ, ULONG ulDiffuse = 0xFF000000) 
        { x = fX; y = fY; z = fZ; Diffuse = ulDiffuse; }
    
    CVertex() 
        { x = 0.0f; y = 0.0f; z = 0.0f; Diffuse = 0xFF000000; }

    float       x;          // Vertex Position X Component
    float       y;          // Vertex Position Y Component
    float       z;          // Vertex Position Z Component
    ULONG       Diffuse;    // Diffuse vertex colour component
    
};

class CMesh
{
public:
	CMesh(ULONG nVertexCount, ULONG nIndexCount);
	CMesh();
	virtual ~CMesh();

	long AddVertex(ULONG nCount = 1);
	long AddIndex(ULONG nCount = 1);

	HRESULT BuildBuffers(LPDIRECT3DDEVICE9 pD3DDevice, bool hardwareTnL, bool ReleaseOriginals = true);

	ULONG m_nVertexCount;
	CVertex* m_pVertex;
	ULONG m_nIndexCount;
	USHORT* m_pIndex;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;

	D3DXVECTOR3 m_BoundsMin;
	D3DXVECTOR3 m_BoundsMax;
};

#endif // !_COBJECT_H_