#include <GL/freeglut.h>

static void ReshapeFunc(int w, int h)
{}

static void DisplayFunc()
{
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutCreateWindow("test window");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);

	glutMainLoop();

	return 0;
}

