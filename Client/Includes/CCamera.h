//-----------------------------------------------------------------------------
// File: CGameApp.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include "Main.h"

class CCamera
{
public:
	enum CAMERA_MODE
	{
		MODE_FPS = 1,
		MODE_THIRDPERSON = 2,
		MODE_SPACECRAFT = 3,

		MODE_FORCE_32BIT = 0x7FFFFFFF
	};

	CCamera();
	CCamera(const CCamera* pCamera);
	virtual ~CCamera();

protected:
	void CalculateFrustumPlanes();

	
};

#endif