#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void SetupRC(void)
{
	glClearColor(0.0f,0.0f,1.0f,1.0f);
}

int main(int argc,char *argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Simple");
	glutDisplayFunc(RenderScene);

	SetupRC();

	glutMainLoop();
}
