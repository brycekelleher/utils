#ifdef __APPLE__
#include <sys/time.h>
#include <unistd.h>

static unsigned int GetMilliseconds()
{
	struct timeval	tp;

	gettimeofday(&tp, NULL);

	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); 
}

unsigned int Sys_Milliseconds (void)
{
	static unsigned int basetime;
	static unsigned int curtime;

	if (!basetime)
	{
		basetime = GetMilliseconds();
	}

	curtime = GetMilliseconds() - basetime;

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
	static unsigned int basetime;
	static unsigned int curtime;

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

