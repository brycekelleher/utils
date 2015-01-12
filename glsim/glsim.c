#include <sys/time.h>
#include <unistd.h>
#include <GL/freeglut.h>

// simulation timestep in msecs
#define SIM_TIMESTEP	100

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

static unsigned int realtime;

static unsigned int simframe;
static unsigned int simtime;

float objx, objy;
float prevx, prevy;
float dirx = 0.1f;
float diry = 0.0f;
float renderx;
float rendery;

static void RunObject()
{
	prevx = objx;
	prevy = objy;

	objx = objx + dirx;
	objy = objy + diry;

	if(objx > 1.0f || objx < -1.0f)
	{
		dirx = -dirx;
	}
}

static void DrawBox()
{
	int i;

	static float verts[4][2] =
	{
		{ -0.1f, -0.1f },
		{  0.1f, -0.1f },
		{  0.1f,  0.1f },
		{ -0.1f,  0.1f }
	};

	float lerp = (float)(simtime - realtime) / SIM_TIMESTEP;

	if(lerp > 1.0f) lerp = 1.0f;
	if(lerp < 0.0f) lerp = 0.0f;

	renderx = ((1.0f - lerp) * objx) + (lerp * prevx);
	rendery = ((1.0f - lerp) * objy) + (lerp * prevy);

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
	for(i = 0; i < 4; i++)
	{
		glVertex2f(verts[i][0] + renderx, verts[i][1] + rendery);
	}
	glEnd();
}

static void ReshapeFunc(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1, 1, -1, 1, -1, 1);

	glViewport(0, 0, w, h);
}

static void DisplayFunc()
{
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawBox();

	glutSwapBuffers();
}

static void SimRunFrame()
{
	simframe++;
	simtime = simframe * SIM_TIMESTEP;

	RunObject();
}

static void MainLoopFunc()
{
	unsigned int newtime = Sys_Milliseconds();

	// yield the thread if no time has advanced
	if(newtime == realtime)
	{
		Sys_Sleep(0);
		return;
	}

	realtime = newtime;

	// run the simulation code
	if(simtime < realtime)
	{
		SimRunFrame();
	}

	// run the rendering code
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutCreateWindow("test window");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutIdleFunc(MainLoopFunc);
	glutMainLoop();

	return 0;
}

