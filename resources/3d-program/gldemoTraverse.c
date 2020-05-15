#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265f
#endif

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

/* Define "point" datatype (using rect. coords) */
typedef struct rpt {float x; float y; float z; } rpoint;

/* Conversion from sph. to rect. coordinates */

rpoint S2R(float r, float t, float p)
{
	rpoint rect;
	rect.x = r * sin(t) * cos(p);
	rect.y = r * sin(t) * sin(p);
	rect.z = r * cos(t);
	return rect;
}

/* Field equation (infinitesimal dipole) */
/* Modify this to plot other patterns */
float field(float t, float p)
{
	return sin(t);
} 

void display(void)
{
	float r, t, p;
	rpoint rp;

	/* Clear the window */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (0.0f, 0.0f, 0.0f);
	glLoadIdentity();

	/* Set the camera angle */
	glTranslatef(0.0f, 0.0f, -2.0f);
	glRotatef (xRot, 1.0f, 0.0f, 0.0f);
	glRotatef (yRot, 0.0f, 1.0f, 0.0f);

	/* Plot phi-oriented lines */
	for (t = 0.0f; t < M_PI; t += M_PI/30.0f)
	{
		/* Create curve made up of many short lines */
		glBegin(GL_LINE_STRIP) ;
		for (p = 0.0f; p < 2.0f* M_PI; p += M_PI/15.0f)
			{
				/* Get magnitude of field */
				r = field(t, p);
			
				/* Convert from spherical to rectangular */
				rp = S2R(r, t, p);
			
				/* Add a point to the line strip */
				glVertex3f (rp.x, rp.y, rp.z);
			}
		glEnd();
	}

	/*Plot theta-oriented lines */
	for(p = 0.0f; p<2.0f * M_PI; p += M_PI / 15.0f)
	{
	glBegin(GL_LINE_STRIP);
		for (t = 0.0f; t < M_PI; t += M_PI/30.0f)
		{
			r = field(t, p);
			rp = S2R(r, t, p);
			glVertex3f (rp.x, rp.y, rp.z);
		}
	glEnd() ;
	}

	/* Swap draw buffer w/ display buffer */
	glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
    {
    if(key == GLUT_KEY_UP)
        xRot-= 5.0f;

    if(key == GLUT_KEY_DOWN)
        xRot += 5.0f;

    if(key == GLUT_KEY_LEFT)
        yRot -= 5.0f;

    if(key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    if(key > 356.0f)
        xRot = 0.0f;

    if(key < -1.0f)
        xRot = 355.0f;

    if(key > 356.0f)
        yRot = 0.0f;

    if(key < -1.0f)
        yRot = 355.0f;

    // Refresh the Window
    glutPostRedisplay();
    }

/* Miscellaneous initialization */
int main(int argc, char** argv)
{
	glutInit (&argc, argv) ;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (400,400);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("GLDemo Traversal");
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0f, 1.0f, -1.0f , 1.0f , 1.0f , 20.0f);
	glutSpecialFunc(SpecialKeys);
	glMatrixMode(GL_MODELVIEW);
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
} 