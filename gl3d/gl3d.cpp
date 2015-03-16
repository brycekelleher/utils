#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "GL/freeglut.h"

#define PI 3.14159265358979323846f

static int framenum;

// Input
typedef struct inputstate_s
{
	int mousepos[2];
	int moused[2];
	bool lbuttondown;
	bool rbuttondown;
	bool keys[256];

} inputstate_t;

static inputstate_t input;

// ==============================================
// memory allocation

#define MEM_ALLOC_SIZE	32 * 1024 * 1024

typedef struct memstack_s
{
	unsigned char mem[MEM_ALLOC_SIZE];
	int allocated;

} memstack_t;

static memstack_t memstack;

void *Mem_Alloc(int numbytes)
{
	unsigned char *mem;
	
	if(memstack.allocated + numbytes > MEM_ALLOC_SIZE)
	{
		printf("Error: Mem: no free space available\n");
		abort();
	}

	mem = memstack.mem + memstack.allocated;
	memstack.allocated += numbytes;

	return mem;
}

void Mem_FreeStack()
{
	memstack.allocated = 0;
}

// ==============================================
// errors and warnings

static void Error(const char *error, ...)
{
	va_list valist;
	char buffer[2048];

	va_start(valist, error);
	vsprintf(buffer, error, valist);
	va_end(valist);

	printf("Error: %s", buffer);
	exit(1);
}

static void Warning(const char *warning, ...)
{
	va_list valist;
	char buffer[2048];

	va_start(valist, warning);
	vsprintf(buffer, warning, valist);
	va_end(valist);

	fprintf(stdout, "Warning: %s", buffer);
}

// ==============================================
// Misc crap

static float Vector_Dot(float a[3], float b[3])
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static void Vector_Copy(float *a, float *b)
{
	a[0] = b[0];
	a[1] = b[1];
	a[2] = b[2];
}

static void Vector_Cross(float *c, float *a, float *b)
{
	c[0] = (a[1] * b[2]) - (a[2] * b[1]); 
	c[1] = (a[2] * b[0]) - (a[0] * b[2]); 
	c[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

static void Vector_Normalize(float *v)
{
	float len, invlen;

	len = sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	invlen = 1.0f / len;

	v[0] *= invlen;
	v[1] *= invlen;
	v[2] *= invlen;
}

static void Vector_Lerp(float *result, float *from, float *to, float t)
{
	result[0] = ((1 - t) * from[0]) + (t * to[0]);
	result[1] = ((1 - t) * from[1]) + (t * to[1]);
	result[2] = ((1 - t) * from[2]) + (t * to[2]);
}

static void MatrixTranspose(float out[4][4], const float in[4][4])
{
	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < 4; j++ )
		{
			out[j][i] = in[i][j];
		}
	}
}

//==============================================
// simulation code

typedef struct viewstate_s
{
	float	angles[2];
	float	pos[3];
	float	vectors[3][3];

	float	znear;
	float	zfar;
	float	fov;

} viewstate_t;

static viewstate_t viewstate;

typedef struct tickcmd_s
{
	float	forwardmove;
	float	sidemove;
	float	anglemove[2];

} tickcmd_t;

static tickcmd_t gcmd;

static void VectorsFromSphericalAngles(float vectors[3][3], float angles[2])
{
	float cx, sx, cy, sy, cz, sz;

	cx = 1.0f;
	sx = 0.0f;
	cy = cosf(angles[0]);
	sy = sinf(angles[0]);
	cz = cosf(angles[1]);
	sz = sinf(angles[1]);

	vectors[0][0] = cy * cz;
	vectors[0][1] = sz;
	vectors[0][2] = -sy * cz;

	vectors[1][0] = (-cx * cy * sz) + (sx * sy);
	vectors[1][1] = cx * cz;
	vectors[1][2] = (cx * sy * sz) + (sx * cy);

	vectors[2][0] = (sx * cy * sz) + (cx * sy);
	vectors[2][1] = (-sx * cz);
	vectors[2][2] = (-sx * sy * sz) + (cx * cy);
}

// build a current command from the input state
static void BuildTickCmd()
{
	tickcmd_t *cmd = &gcmd;
	float scale;
	
	// move forward ~512 units each second (60 * 4.2)
	scale = 4.2f;

	cmd->forwardmove = 0.0f;
	cmd->sidemove = 0.0f;
	cmd->anglemove[0] = 0.0f;
	cmd->anglemove[1] = 0.0f;

	if(input.keys['w'])
	{
		cmd->forwardmove += scale;
	}

	if(input.keys['s'])
	{
		cmd->forwardmove -= scale;
	}

	if(input.keys['d'])
	{
		cmd->sidemove += scale;
	}

	if(input.keys['a'])
	{
		cmd->sidemove -= scale;
	}

	// Handle mouse movement
	if(input.lbuttondown)
	{
		cmd->anglemove[0] = -0.01f * (float)input.moused[0];
		cmd->anglemove[1] = -0.01f * (float)input.moused[1];
	}
}

// apply the tick command to the viewstate
static void DoMove()
{
	tickcmd_t *cmd = &gcmd;

	VectorsFromSphericalAngles(viewstate.vectors, viewstate.angles);

	viewstate.pos[0] += cmd->forwardmove * viewstate.vectors[0][0];
	viewstate.pos[1] += cmd->forwardmove * viewstate.vectors[0][1];
	viewstate.pos[2] += cmd->forwardmove * viewstate.vectors[0][2];

	viewstate.pos[0] += cmd->sidemove * viewstate.vectors[2][0];
	viewstate.pos[1] += cmd->sidemove * viewstate.vectors[2][1];
	viewstate.pos[2] += cmd->sidemove * viewstate.vectors[2][2];

	viewstate.angles[0] += cmd->anglemove[0];
	viewstate.angles[1] += cmd->anglemove[1];

	if(viewstate.angles[1] >= PI / 2.0f)
		viewstate.angles[1] = (PI / 2.0f) - 0.001f;
	if(viewstate.angles[1] <= -PI/ 2.0f)
		viewstate.angles[1] = (-PI / 2.0f) + 0.001f;
}

static void SetupDefaultViewState()
{
	// look down negative z
	viewstate.angles[0]	= PI / 2.0f;
	viewstate.angles[1]	= 0.0f;
	
	viewstate.pos[0]	= 0.0f;
	viewstate.pos[1]	= 0.0f;
	viewstate.pos[2]	= 256.0f;

	viewstate.znear		= 3.0f;
	viewstate.zfar		= 4096.0f;
	viewstate.fov		= 90.0f;
}

//==============================================
// OpenGL rendering code
//
// this stuff touches some of the simulation state (viewvectors, viewpos etc)
// guess it should really have an interface to extract that data?

static int renderwidth;
static int renderheight;

static void GL_LoadMatrix(float m[4][4])
{
	glLoadMatrixf((float*)m);
}

static void GL_LoadMatrixTranspose(float m[4][4])
{
	float t[4][4];

	MatrixTranspose(t, m);
	glLoadMatrixf((float*)t);
}

static void GL_MultMatrix(float m[4][4])
{
	glMultMatrixf((float*)m);
}

static void GL_MultMatrixTranspose(float m[4][4])
{
	float t[4][4];

	MatrixTranspose(t, m);
	glMultMatrixf((float*)t);
}

static void DrawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(32, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 32, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 32);
	glEnd();
}

static void DrawVector(float *origin, float *dir)
{
	float s = 2.0f;

	glBegin(GL_LINES);
	{
		glVertex3f(origin[0], origin[1], origin[2]);
		glVertex3f(s * dir[0] + origin[0], s * dir[1] + origin[1], s * dir[2] + origin[2]);
	}
	glEnd();
}

static void DrawMatrix(float matrix[4][4])
{
	float vectors[4][3] =
	{
		{ matrix[0][0], matrix[1][0], matrix[2][0] },
		{ matrix[0][1], matrix[1][1], matrix[2][1] },
		{ matrix[0][2], matrix[1][2], matrix[2][2] },
		{ matrix[0][3], matrix[1][3], matrix[2][3] },
	};

	glColor3f(1, 0, 0);
	DrawVector(vectors[3], vectors[0]);
	glColor3f(0, 1, 0);
	DrawVector(vectors[3], vectors[1]);
	glColor3f(0, 0, 1);
	DrawVector(vectors[3], vectors[2]);
}

static void SetModelViewMatrix()
{
	// matrix to transform from look down x to looking down -z
	static float yrotate[4][4] =
	{
		{ 0, 0, 1, 0 },
		{ 0, 1, 0, 0 },
		{ -1, 0, 0, 0 },
		{ 0, 0, 0, 1 }
	};

	float matrix[4][4];
	matrix[0][0]	= viewstate.vectors[0][0];
	matrix[0][1]	= viewstate.vectors[0][1];
	matrix[0][2]	= viewstate.vectors[0][2];
	matrix[0][3]	= -(viewstate.vectors[0][0] * viewstate.pos[0]) - (viewstate.vectors[0][1] * viewstate.pos[1]) - (viewstate.vectors[0][2] * viewstate.pos[2]);

	matrix[1][0]	= viewstate.vectors[1][0];
	matrix[1][1]	= viewstate.vectors[1][1];
	matrix[1][2]	= viewstate.vectors[1][2];
	matrix[1][3]	= -(viewstate.vectors[1][0] * viewstate.pos[0]) - (viewstate.vectors[1][1] * viewstate.pos[1]) - (viewstate.vectors[1][2] * viewstate.pos[2]);

	matrix[2][0]	= viewstate.vectors[2][0];
	matrix[2][1]	= viewstate.vectors[2][1];
	matrix[2][2]	= viewstate.vectors[2][2];
	matrix[2][3]	= -(viewstate.vectors[2][0] * viewstate.pos[0]) - (viewstate.vectors[2][1] * viewstate.pos[1]) - (viewstate.vectors[2][2] * viewstate.pos[2]);

	matrix[3][0]	= 0.0f;
	matrix[3][1]	= 0.0f;
	matrix[3][2]	= 0.0f;
	matrix[3][3]	= 1.0f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GL_MultMatrixTranspose(yrotate);
	GL_MultMatrixTranspose(matrix);
}

static void SetPerspectiveMatrix()
{
	float r, l, t, b;
	float fovx, fovy;
	float m[4][4];

	fovx = viewstate.fov * (PI / 360.0f);
	float x = (renderwidth / 2.0f) / atan(fovx);
	fovy = atan2(renderheight / 2.0f, x);

	// Calcuate right, left, top and bottom values
	r = viewstate.znear * fovx;
	l = -r;

	t = viewstate.znear * fovy;
	b = -t;

	m[0][0] = (2.0f * viewstate.znear) / (r - l);
	m[1][0] = 0;
	m[2][0] = (r + l) / (r - l);
	m[3][0] = 0;

	m[0][1] = 0;
	m[1][1] = (2.0f * viewstate.znear) / (t - b);
	m[2][1] = (t + b) / (t - b);
	m[3][1] = 0;

	m[0][2] = 0;
	m[1][2] = 0;
	m[2][2] = -(viewstate.zfar + viewstate.znear) / (viewstate.zfar - viewstate.znear);
	m[3][2] = -2.0f * viewstate.zfar * viewstate.znear / (viewstate.zfar - viewstate.znear);

	m[0][3] = 0;
	m[1][3] = 0;
	m[2][3] = -1;
	m[3][3] = 0;

	glMatrixMode(GL_PROJECTION);
	GL_LoadMatrix(m);
}

static void BeginFrame()
{
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	SetModelViewMatrix();

	SetPerspectiveMatrix();

	glViewport(0, 0, renderwidth, renderheight);
}

static void EndFrame()
{}

static void Draw()
{
	BeginFrame();

	DrawAxis();

	EndFrame();
}

static void Tick()
{
	BuildTickCmd();
	
	DoMove();
}

//==============================================
// GLUT OS windowing code

static int mousepos[2];

static void ProcessInput()
{
	// mousepos has current "frame" mouse pos
	input.moused[0] = mousepos[0] - input.mousepos[0];
	input.moused[1] = mousepos[1] - input.mousepos[1];
	input.mousepos[0] = mousepos[0];
	input.mousepos[1] = mousepos[1];
}

static void DisplayFunc()
{
	Draw();

	glutSwapBuffers();
}

static void KeyboardDownFunc(unsigned char key, int x, int y)
{
	input.keys[key] = true;
}

static void KeyboardUpFunc(unsigned char key, int x, int y)
{
	input.keys[key] = false;
}

static void ReshapeFunc(int w, int h)
{
	renderwidth = w;
	renderheight = h;
}

static void MouseFunc(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
		input.lbuttondown = (state == GLUT_DOWN);
	if(button == GLUT_RIGHT_BUTTON)
		input.rbuttondown = (state == GLUT_DOWN);
}

static void MouseMoveFunc(int x, int y)
{
	mousepos[0] = x;
	mousepos[1] = y;
}

static void TimerFunc(int value)
{
	ProcessInput();

	Tick();

	glutTimerFunc(16, TimerFunc, value);

	// kick a redraw
	glutPostRedisplay();

	framenum++;
}

static void IdleFunc()
{}

static void PrintUsage()
{}

static void ProcessCommandLine(int argc, char *argv[])
{}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	SetupDefaultViewState();
	
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(400, 400);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("test");

	ProcessCommandLine(argc, argv);

	glutReshapeFunc(ReshapeFunc);
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardDownFunc);
	glutKeyboardUpFunc(KeyboardUpFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MouseMoveFunc);
	glutPassiveMotionFunc(MouseMoveFunc);
	glutTimerFunc(16, TimerFunc, 0);
	glutIdleFunc(IdleFunc);

	glutMainLoop();

	return 0;
}

