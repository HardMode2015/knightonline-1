//-----------------------------------------------------------------------------
// File: CTerrainManager.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CTERRAINMANAGER_H_
#define _CTERRAINMANAGER_H_

#include "Main.h"

struct _N3MapData {
	float fHeight;
	unsigned int bIsTileFull : 1;
	unsigned int Tex1Dir : 5;
	unsigned int Tex2Dir : 5;
	unsigned int Tex1Idx : 10;
	unsigned int Tex2Idx : 10;

	_N3MapData(void) {
		bIsTileFull = true;
		fHeight = FLT_MIN;
		Tex1Idx = 1023;
		Tex1Dir = 0;
		Tex2Idx = 1023;
		Tex2Dir = 0;
	}
};

class CTerrainManager
{
public:
	
	CTerrainManager();
	virtual ~CTerrainManager();

	void LoadFromFile(LPTSTR fileName);

private:
	int m_nTileSize = 4;
	int m_nPatchTileSize = 8;

	float m_fTileDirU[8][4] = {
		{ 0.0f,	1.0f, 0.0f,	1.0f },		//[up][LT, RT, LB, RB]
		{ 0.0f,	0.0f, 1.0f, 1.0f },		//[right][ // ]
		{ 1.0f,	0.0f, 1.0f, 0.0f },		//[left][ // ]
		{ 1.0f, 1.0f, 0.0f,	0.0f },		//[bottom][ // ]

		{ 1.0f,	0.0f, 1.0f,	0.0f },		//[up_mirr][LT, RT, LB, RB]
		{ 0.0f,	0.0f, 1.0f, 1.0f },		//[right_mirr][ // ]
		{ 0.0f,	1.0f, 0.0f, 1.0f },		//[left_mirr][ // ]
		{ 1.0f, 1.0f, 0.0f,	0.0f }		//[bottom_mirr][ // ]
	};

	float m_fTileDirV[8][4] = {
		{ 0.0f, 0.0f, 1.0f, 1.0f },		//[up][ // ]
		{ 1.0f, 0.0f, 1.0f, 0.0f },		//[right][ // ]
		{ 1.0f, 1.0f, 0.0f, 0.0f },		//[left][ // ]
		{ 0.0f, 1.0f, 0.0f, 1.0f },		//[bottom][ // ]

		{ 0.0f, 0.0f, 1.0f, 1.0f },		//[up_mirr][ // ]
		{ 0.0f, 1.0f, 0.0f, 1.0f },		//[right_mirr][ // ]
		{ 1.0f, 1.0f, 0.0f, 0.0f },		//[left_mirr][ // ]
		{ 1.0f, 0.0f, 1.0f, 0.0f }		//[bottom_mirr][ // ]
	};
};

#endif