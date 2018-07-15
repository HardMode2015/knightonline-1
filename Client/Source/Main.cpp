//-----------------------------------------------------------------------------
// File: Main.cpp
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------

#include "../Includes/Main.hpp"
#include "../Includes/CEngine.hpp"

CEngine m_iEngine;

int WINAPI _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int iCmdShow )
{
	// Init game engine.
	m_iEngine.InitEngine(hInstance, lpCmdLine);

    int retCode;

	// Start game and tick till user quits
	retCode = m_iEngine.StartGame();
    
    return retCode;
}