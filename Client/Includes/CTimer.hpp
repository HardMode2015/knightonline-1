//-----------------------------------------------------------------------------
// File: CTimer.h
// Author: Osein <osein.wtr@gmail.com>
//-----------------------------------------------------------------------------
#ifndef _CTIMER_H_
#define _CTIMER_H_

#include "Main.hpp"

const ULONG MAX_SAMPLE_COUNT = 50; // Maximum frame time sample count

class CTimer
{
public:
	CTimer();

	void	        Tick( float fLockFPS = 0.0f );
    unsigned long   GetFrameRate( LPTSTR lpszString = nullptr ) const;
    float           GetTimeElapsed() const;

private:
    bool            m_PerfHardware;             // Has Performance Counter
	float           m_TimeScale;                // Amount to scale counter
	float           m_TimeElapsed;              // Time elapsed since previous frame
    __int64         m_CurrentTime;              // Current Performance Counter
    __int64         m_LastTime;                 // Performance Counter last frame
	__int64         m_PerfFreq;                 // Performance Frequency

    float           m_FrameTime[MAX_SAMPLE_COUNT];
    ULONG           m_SampleCount;

    unsigned long   m_FrameRate;                // Stores current framerate
	unsigned long   m_FPSFrameCount;            // Elapsed frames in any given second
	float           m_FPSTimeElapsed;           // How much time has passed during FPS sample
};

#endif // _CTIMER_H_