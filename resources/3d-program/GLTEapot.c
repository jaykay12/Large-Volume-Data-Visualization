#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

void RenderScene(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0f,0.0f,0.0f);
  glutWireTeapot(1.0f);
  glFlush();
}

void SetupRC(void)
{
  glClearColor(0.0f,0.0f,1.0f,1.0f);
}

void ChangeSize(GLsizei w, GLsizei h)
{
  if(h==0)
    h=1;

glViewport(0,0,w,h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
if(w<=h)
    glOrtho(0.0f,250.0f,0.0f,250.0f*h/w,1.0,-1.0);
else
    glOrtho(0.0f,250.0f*w/h,0.0f,250.0f,1.0,-1.0);

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

int main(int argc,char* argv[])
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(800,600);
  glutCreateWindow("GLTeaPot");
  glutDisplayFunc(RenderScene);
  glutReshapeFunc(ChangeSize);

  SetupRC();

  glutMainLoop();
}
