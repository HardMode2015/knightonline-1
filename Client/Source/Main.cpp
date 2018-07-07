//-----------------------------------------------------------------------------
// File: Main.cpp
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------

#include "..\\Includes\\Main.h"
#include "..\\Includes\\CGameApp.h"

CGameApp    g_App;      // Core game application processing engine

int WINAPI _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int iCmdShow )
{
	// Seed the random on start.
	srand(timeGetTime());

    int retCode;
	if (!g_App.InitInstance( hInstance, lpCmdLine, iCmdShow )) return 0;
    retCode = g_App.BeginGame();
    if ( !g_App.ShutDown() )  MessageBox( 0, _T("Failed to shut system down correctly, please check file named 'debug.txt'.\r\n\r\nIf the problem persists, please contact technical support."), _T("Non-Fatal Error"), MB_OK | MB_ICONEXCLAMATION );
    return retCode;
}