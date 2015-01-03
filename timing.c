#ifdef __APPLE__
#include <sys/time.h>
#include <unistd.h>

unsigned int Sys_Milliseconds (void)
{
	struct timeval	tp;
	static int		secbase;
	static int		curtime;

	gettimeofday(&tp, NULL);

	if (!secbase)
	{
		secbase = tp.tv_sec;
	}

	curtime = (tp.tv_sec - secbase) * 1000 + tp.tv_usec / 1000;

	return curtime;
}

void Sys_Sleep(unsigned int msecs)
{
	usleep(msecs * 1000);
}

#endif

#ifdef WIN32
#include <windows.h>

unsigned int Sys_Milliseconds(void)
{
	static int basetime;
	static int curtime;

	if(!basetime)
	{
		basetime = timeGetTime();
	}

	curtime = timeGetTime() - basetime;

	return curtime;
}

void Sys_Sleep(unsigned int msecs)
{
	Sleep(msecs);
}
#endif

