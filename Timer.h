
#ifndef _TIMER_HXX
#define _TIMER_HXX

class Timer
{
	public:
		/*	Constructor. */
		Timer();

		/*	Destructor. */
		virtual ~Timer();

		/*	Reset the timer to zero. */
		virtual void reset() = 0;

		/*	Retrieve the time in milliseconds since the timer started. */
		virtual double getTime() const = 0;

		/*	Return a new timer. IMPLEMENT THIS IN THE FILE FOR THE DERIVED CLASS. */
		static Timer *getNewTimer();

		/*	Pause this timer. */
		virtual void pause() = 0;

		/*	Unpause this timer. */
		virtual void unpause() = 0;

		/*	Pause all timers. */
		static void pauseAll();

		/*	Unpause all timers. */
		static void unpauseAll();

	private:
		struct TimerLink
		{
			Timer *m_pTimer;
			TimerLink *m_pNext;
		};

		static TimerLink *m_pTimerList;
};

#endif	// PORTALIB3D_TIMER_HXX
