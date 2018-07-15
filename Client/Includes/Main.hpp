//-----------------------------------------------------------------------------
// File: Main.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _MAIN_H_
#define _MAIN_H_

#include "..\\Res\\resource.h"
#include <windows.h>
#include <tchar.h>
#include <D3DX9.h>
#include <iostream>
#include <sstream>

#define RANDOM_COLOR 0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX)

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

#endif // _MAIN_H_