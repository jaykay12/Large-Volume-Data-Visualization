#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

GLfloat xRotated=0.0, yRotated=0.0, zRotated=0.0;
GLfloat xTranslate=0.0, yTranslate=0.0, zTranslate=0.0;
GLfloat xScale=1.0, yScale=1.0, zScale=1.0;

int iShape = 1;

void init(void)
{
    glClearColor(0,0,0,0); 
}

void DrawCube(void)
{
 
    glMatrixMode(GL_MODELVIEW);
    
    // clear the drawing buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    glLoadIdentity();
        
    glTranslatef(0.0+xTranslate, 0.0+yTranslate, -10.5+ zTranslate);

    glRotatef(xRotated,1.0,0.0,0.0);
    // rotation about Y axis
    glRotatef(yRotated,0.0,1.0,0.0);
    // rotation about Z axis
    glRotatef(zRotated,0.0,0.0,1.0);

    glScalef(xScale,yScale,zScale);

  glBegin(GL_QUADS);        // Draw The Cube Using quads
    glColor3f(0.0f,1.0f,0.0f);    // Color Blue
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)
    
    glColor3f(1.0f,0.5f,0.0f);    // Color Orange
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)
    
    glColor3f(1.0f,0.0f,0.0f);    // Color Red    
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)
    
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)
    
    glColor3f(1.0f,0.0f,1.0f);    // Color Violet
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)
  glEnd();            // End Drawing The Cube
glFlush();


}


/*void animation(void)
{
 
     yRotated += 0.01;
     xRotated += 0.02;
    DrawCube();
}*/

void ProcessMenu(int value)
{
    iShape = value;

    switch(iShape)
        {
        case 1:
            xRotated += 90.0;
            break;

        case 2:
            yRotated += 90.0;
            break;

        case 3:
            zRotated += 90.0;
            break;

        case 4:
            xTranslate += 2.0;
            break;

        case 5:
            xTranslate += -2.0;
            break;

        case 6:
            yTranslate += 2.0;
            break;

        case 7:
            yTranslate += -2.0;
            break;

        case 8:
            zTranslate += 2.0;
            break;

        case 9:
            zTranslate += -2.0;
            break;

        case 10:
            xScale += 0.5;
            yScale += 0.5;
            zScale += 0.5;
            break;

        case 11:
            xScale -= 0.5;
            yScale -= 0.5;
            zScale -= 0.5;
            break;            

        default:
            xRotated += 0.0;

        }
    
    // Restore transformations
    glPopMatrix();

    // Flush drawing commands
    glutSwapBuffers();

    glutPostRedisplay();
}

void reshape(int x, int y)
{
    if (y == 0 || x == 0) return;  //Nothing is visible then, so return
    //Set a new projection matrix
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();
    //Angle of view:40 degrees
    //Near clipping plane distance: 0.5
    //Far clipping plane distance: 20.0
     
    gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0,0,x,y);  //Use the whole window for rendering
}

int main(int argc, char* argv[])
{
    int nTransformationMenu, nRotateMenu, nTranslateMenu, nScaleMenu;

    glutInit(&argc, argv);
//we initizlilze the glut. functions
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cube");

    nRotateMenu = glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("90 degree along x-axis",1);
    glutAddMenuEntry("90 degree along y-axis",2);
    glutAddMenuEntry("90 degree along z-axis",3);

    nTranslateMenu = glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("2 units along positive x-axis",4);
    glutAddMenuEntry("2 units along negative x-axis",5);
    glutAddMenuEntry("2 units along positive y-axis",6);
    glutAddMenuEntry("2 units along negative y-axis",7);    
    glutAddMenuEntry("2 units along positive z-axis",8);
    glutAddMenuEntry("2 units along negative z-axis",9);


    nScaleMenu = glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Zoom In",10);
    glutAddMenuEntry("Zoom Out",11);

    nTransformationMenu = glutCreateMenu(ProcessMenu);
    glutAddSubMenu("Rotate",nRotateMenu);
    glutAddSubMenu("Translate",nTranslateMenu);
    glutAddSubMenu("Scale",nScaleMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();
    glutDisplayFunc(DrawCube);
    glutReshapeFunc(reshape);
//Set the function for the animation.
    //glutIdleFunc();
    glutMainLoop();
    
    return 0;
} //- See more at: http://www.codemiles.com/c-opengl-examples/draw-3d-cube-using-opengl-t9018.html#sthash.TGrdK6Sq.dpuf