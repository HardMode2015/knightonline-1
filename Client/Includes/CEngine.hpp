//-----------------------------------------------------------------------------
// File: CGameApp.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CENGINE_H_
#define _CENGINE_H_

#include "CBase.hpp"

class CEngine : public CBase
{
public:

	CEngine();
	virtual ~CEngine();

	int StartGame();
	void AdvanceFrame();

private:

	

};

#endif