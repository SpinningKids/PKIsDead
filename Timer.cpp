/*
	Timer.cpp

		Timer abstract base class.

	Author:	Brett Porter
	Email: brettporter@yahoo.com
	Website: http://rsn.gamedev.net/pl3d
	Copyright (C)2000, 2001, Brett Porter. All Rights Reserved.
	This source code is released under the LGPL. See license.txt for details.

	Created: 6 February 2001
	Last Edited: 6 February 2001

	Please see the file ChangeLog.html for a revision history.
*/

#include "Timer.h"
#include <assert.h>
#include <stdlib.h>

Timer::TimerLink *Timer::m_pTimerList = NULL;

Timer::Timer()
{
	TimerLink *pLink = new TimerLink;
	pLink->m_pTimer = this;
	pLink->m_pNext = m_pTimerList;
	m_pTimerList = pLink;
}

Timer::~Timer()
{
	TimerLink *pLink = m_pTimerList, *pPrev = NULL;
	while ( pLink != NULL )
	{
		if ( pLink->m_pTimer == this )
		{
			if ( pPrev == NULL )
				m_pTimerList = pLink->m_pNext;
			else
				pPrev->m_pNext = pLink->m_pNext;

			delete pLink;
			break;
		}
		pPrev = pLink;
		pLink = pLink->m_pNext;
	}
	assert( pLink != NULL );
}

void Timer::pauseAll()
{
	TimerLink *pLink = m_pTimerList;
	while ( pLink != NULL )
	{
		pLink->m_pTimer->pause();
		pLink = pLink->m_pNext;
	}
}

void Timer::unpauseAll()
{
	TimerLink *pLink = m_pTimerList;
	while ( pLink != NULL )
	{
		pLink->m_pTimer->unpause();
		pLink = pLink->m_pNext;
	}
}
