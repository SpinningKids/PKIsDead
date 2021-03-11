/*
	Win32Timer.cpp

		Timer class under Microsoft Windows.

	Author:	Brett Porter
	Email: brettporter@yahoo.com
	Website: http://rsn.gamedev.net/pl3d
	Copyright (C)2000, 2001, Brett Porter. All Rights Reserved.
	This source code is released under the LGPL. See license.txt for details.

	Created: 18 July 2000
	Last Edited: 6 Febraury 2001

	Please see the file ChangeLog.html for a revision history.
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Assert.h>
#include "Win32Timer.h"

#ifndef _WIN32
#	error	This file should only be used in a Microsoft Windows build.
#endif

// Disable warning about non-standard extension in Microsoft's header files
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( default : 4201 )
#pragma comment( lib, "winmm.lib" )

Timer *Timer::getNewTimer()
{
	return new Win32Timer();
}

Win32Timer::Win32Timer()
{
/*	if ( QueryPerformanceFrequency(( LARGE_INTEGER* )&m_frequency ) == false )
	{
		// No performance counter available
		m_usePerformanceCounter = false;

		m_mmTimerStart = timeGetTime();
	}
	else
	{
		// Performance counter is available, use it instead of the multimedia timer
		m_usePerformanceCounter = true;

		// Get the current time and store it in pcTimerStart
		QueryPerformanceCounter(( LARGE_INTEGER* )&m_pcTimerStart );

		// Calculate the timer resolution using the timer frequency
		m_resolution = ( float )( 1.0/( double )m_frequency )*1000.0f;
	}
*/
	m_pauseCount = 0;
	m_pauseTime = 0;
}

void Win32Timer::reset()
{
	if ( m_usePerformanceCounter == true )
		QueryPerformanceCounter(( LARGE_INTEGER* )&m_pcTimerStart );
	else
		m_mmTimerStart = timeGetTime();

	m_pauseTime = getTime();
}

double Win32Timer::getTime() const
{
	if ( m_pauseCount > 0 )
		return m_pauseTime;

	__int64 timeElapsed;

	if ( m_usePerformanceCounter == true )
	{
		QueryPerformanceCounter(( LARGE_INTEGER* )&timeElapsed );
		timeElapsed -= m_pcTimerStart;
		return timeElapsed*m_resolution;
	}
	else
	{
		timeElapsed = timeGetTime()-m_mmTimerStart;
		return ( double )timeElapsed;
	}
}

void Win32Timer::pause()
{
	if ( m_pauseCount == 0 )
		m_pauseTime = getTime();

	m_pauseCount++;
}

void Win32Timer::unpause()
{
	assert( m_pauseCount > 0 );
	m_pauseCount--;

	if ( m_pauseCount == 0 )
	{
		if ( m_usePerformanceCounter == true )
		{
			__int64 time;
			QueryPerformanceCounter(( LARGE_INTEGER* )&time );
			m_pcTimerStart = time - ( __int64 )( m_pauseTime/m_resolution );
		}
		else
		{
			m_mmTimerStart = ( unsigned )( timeGetTime() - m_pauseTime );
		}
	}
}


void Win32Timer::setTime(float t)
{
	__int64 timeElapsed;
	__int64 newtime = (__int64)t;

	if ( m_usePerformanceCounter == true )
	{
		QueryPerformanceCounter(( LARGE_INTEGER* )&timeElapsed );
		//timeElapsed -= m_pcTimerStart;
		__int64 adesso = timeElapsed * m_resolution;
		__int64 inizio = m_pcTimerStart * m_resolution;
		__int64 trascorso = adesso - inizio;
		m_pcTimerStart = (__int64)(-(newtime / m_resolution) + timeElapsed); // * m_resolution);

/*		adesso = (timeElapsed * m_resolution);
		inizio = (m_pcTimerStart * m_resolution);
		trascorso = adesso - inizio;

		QueryPerformanceCounter(( LARGE_INTEGER* )&timeElapsed );
		timeElapsed -= m_pcTimerStart;
		timeElapsed *= m_resolution;
*/		
		//return timeElapsed*m_resolution;
	}
	else
	{
		timeElapsed = timeGetTime()-m_mmTimerStart;
		m_mmTimerStart += timeElapsed - t;
		//return ( double )timeElapsed;
	}
}

void Win32Timer::startTimer()
{
	if ( QueryPerformanceFrequency(( LARGE_INTEGER* )&m_frequency ) == false )
	{
		// No performance counter available
		m_usePerformanceCounter = false;

		m_mmTimerStart = timeGetTime();
	}
	else
	{
		// Performance counter is available, use it instead of the multimedia timer
		m_usePerformanceCounter = true;

		// Get the current time and store it in pcTimerStart
		QueryPerformanceCounter(( LARGE_INTEGER* )&m_pcTimerStart );

		// Calculate the timer resolution using the timer frequency
		m_resolution = ( float )( 1.0/( double )m_frequency )*1000.0f;
	}

	m_pauseCount = 0;
	m_pauseTime = 0;

}
