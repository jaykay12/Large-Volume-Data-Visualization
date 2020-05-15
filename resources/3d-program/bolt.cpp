#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

void changeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 100.0f;
	if(h==0)
		h=1;

	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(w <= h)
		glOrtho(-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange*2.0f, nRange*2.0f);
	else
		glOrtho(-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange*2.0f, nRange*2.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SetupRC()
{
	GLfloat ambientLight[] = {0.4f,0.4f,0.4f,1.0f};
	GLfloat diffuseLight[] = {0.7f,0.7f,0.7f,1.0f};
	GLfloat specular[] = {0.9f,0.9f,0.9f,1.0f};
	GLfloat lightPos[] = {-50.0f,200.0f,200.0f,1.0f};
	GLfloat specref[] = {0.6f,0.6f,0.6f,1.0f};

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	glLightModefv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS,64);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void RenderHead(void)
{
	float x,y,angle;
	
}

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glRotatef(xRot, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot,0.0f,1.0f,0.0f);
		glTranslatef(0.0f,0.0f,55.0f);
		RenderHead();
	glPopMatrix();

	glPushMatrix();
		glRotatef(xRot,1.0f,0.0f,0.0f);
		glRotatef(yRot,0.0f,1.0f,0.0f);
		glTranslatef(0.0f,0.0f,40.0f);

		RenderShaft();
		RenderThread();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}




























int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
//we initizlilze the glut. functions
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cube");
init();
    glutDisplayFunc(DrawCube);
    glutReshapeFunc(reshape);
//Set the function for the animation.
    glutIdleFunc(animation);
    glutMainLoop();
    return 0;
} //- See more at: http://www.codemiles.com/c-opengl-examples/draw-3d-cube-using-opengl-t9018.html#sthash.TGrdK6Sq.dpuf