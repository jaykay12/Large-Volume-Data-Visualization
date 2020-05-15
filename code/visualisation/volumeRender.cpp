#include <iostream>
#include <string>
#include <cmath>

using namespace std;

// OpenGL Graphics includes
#include <GL/glew.h>
#if defined (__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <cstdio>
#include <ctime>

// CUDA Runtime, Interop, and includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

// CUDA utilities
#include <helper_cuda.h>
#include <helper_cuda_gl.h>

// Helper functions
#include <helper_cuda.h>
#include <helper_functions.h>
#include <helper_timer.h>

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

// Define the files that are to be save and the reference images for validation
const char *sOriginal[] =
{
    "volume.ppm",
    NULL
};

const char *sReference[] =
{
    "ref_volume.ppm",
    NULL
};

const char *sSDKsample = "CUDA 3D Volume Render";

const char *volumeFilename = "Bucky.raw";
typedef unsigned char VolumeType;
cudaExtent volumeSize = make_cudaExtent(220, 128, 234);// for 1

//uint width = 512, height = 700;
uint width = 612, height = 700;


dim3 blockSize(16, 16);
dim3 gridSize;

float3 viewRotation;
float3 viewTranslation = make_float3(0.0, 0.0, -4.0f);
float invViewMatrix[12];

float density = 0.3f;
float brightness = 1.0f;
float transferOffset = 0.09f;
float transferScale = 1.0f;
bool linearFiltering = true;
float f_size=0.0;

GLuint pbo = 0;     // OpenGL pixel buffer object
GLuint tex = 0;     // OpenGL texture object
struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

StopWatchInterface *timer = 0;

// Auto-Verification Code
const int frameCheckNumber = 2;
int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
int g_Index = 0;
unsigned int frameCount = 0;

int *pArgc;
char **pArgv;

#ifndef MAX
#define MAX(a,b) ((a > b) ? a : b)
#endif

extern "C" void setTextureFilterMode(bool bLinearFilter);
extern "C" void initCuda(void *h_volume, cudaExtent volumeSize);
extern "C" void freeCudaBuffers();
extern "C" void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH,
                              float density, float brightness, float transferOffset, float transferScale,float x1,float y1,float z1,float x2,float y2,float z2);
extern "C" void copyInvViewMatrix(float *invViewMatrix, size_t sizeofMatrix);

void initPixelBuffer();
void loadFile();
string pathIndex="dataset/";
string currentFilePath="root";
string  extension=".raw";

int font=0;
int length=0;

int orientation=1;
int quadSelected=1;


float x1OriginalBox=-1.0f,y1OriginalBox=-1.0f,z1OriginalBox=-1.0f;
float x2OriginalBox=0.75f,y2OriginalBox=1.0f,z2OriginalBox=1.0f;

float x1CurrentBox=-1.0f,y1CurrentBox=-1.0f,z1CurrentBox=-1.0f;
float x2CurrentBox=0.75f,y2CurrentBox=1.0f,z2CurrentBox=1.0f;

float x1RectangleOriginal=0.2f,y1RectangleOriginal=0.2f,x2RectangleOriginal=0.8f,y2RectangleOriginal=0.8f;
float x1RectangleCurrent=0.2f,y1RectangleCurrent=0.2f,x2RectangleCurrent=0.8f,y2RectangleCurrent=0.8f;

int crop=0;

int option=0; //0-Home 1-Instructions 2-Credits

void computeFPS()
{
    frameCount++;
    fpsCount++;

    if (fpsCount == fpsLimit)
    {
        char fps[256];
        float ifps = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
        sprintf(fps, "Volume Render: %3.1f fps  size of file = %.2f MB", ifps,f_size);


        glutSetWindowTitle(fps);
        fpsCount = 0;

        fpsLimit = (int)MAX(1.f, ifps);
        sdkResetTimer(&timer);
    }
}

//::::::::::::::::TEXT FUNCTION:::::::::::::://
void drawBitmapText(const char *string,float x,float y,float z)
{
    const char *c;
    glRasterPos3f(x,y,z);

    for (c=string; *c != '\0'; c++)
    {
      if(font==0)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
      else if(font==1)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
      else if(font==2)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, *c);
      else if(font==3)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
      else if(font==4)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
      else if(font==5)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
      else if(font==6)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

    }
}

void loadMenu()
{
	//glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.52,0.80,1.0);
	glRectf(0.15,0.92,0.30,0.87);
	glRectf(0.70,0.92,0.85,0.87);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(x1RectangleCurrent,y1RectangleCurrent,x2RectangleCurrent,y2RectangleCurrent);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glRectf(0.05,0.05,0.23,0.10);
	glRectf(0.80,0.05,0.90,0.10);

	glColor3f(1.0,0.5,0.0);
	if(crop==1)
		glRectf(0.42,0.10,0.58,0.15);

	glColor3f(0.0,0.0,0.0);
    font=4;
    drawBitmapText("BACK",0.18,0.885,0);
    drawBitmapText("RESET",0.73,0.885,0);
    drawBitmapText("CREDITS!",0.065,0.065,0);
    drawBitmapText("EXIT",0.82,0.065,0);
    font=4;
    drawBitmapText("RENDER",0.445,0.115,0);

    glColor3f(1.0,1.0,1.0);
    drawBitmapText("LARGE VOLUME DATA VISUALIZATION",0.22,0.95,0);

    glFlush();
    glColor3f(1.0,1.0,1.0);

}


// render image using CUDA
void render()
{
    copyInvViewMatrix(invViewMatrix, sizeof(float4)*3);

    // map PBO to get CUDA device pointer
    uint *d_output;
    // map PBO to get CUDA device pointer
    checkCudaErrors(cudaGraphicsMapResources(1, &cuda_pbo_resource, 0));
    size_t num_bytes;
    checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&d_output, &num_bytes,
                                                         cuda_pbo_resource));
    //printf("CUDA mapped PBO: May access %ld bytes\n", num_bytes);

    // clear image
    checkCudaErrors(cudaMemset(d_output, 0, width*height*4));

    // call CUDA kernel, writing results to PBO
    render_kernel(gridSize, blockSize, d_output, width, height, density, brightness, transferOffset, transferScale,x1CurrentBox,y1CurrentBox,z1CurrentBox,x2CurrentBox,y2CurrentBox,z2CurrentBox);

    //getLastCudaError("kernel failed");

    checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));
}



// display results using OpenGL (called by GLUT)
void display()
{
	if(option==0)
	{
		    sdkStartTimer(&timer);
			glClear(GL_COLOR_BUFFER_BIT);
		    
		    // use OpenGL to build view matrix  
		    GLfloat modelView[16];
		    glMatrixMode(GL_MODELVIEW);
		    glPushMatrix();
		    glLoadIdentity();
		    glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
		    glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
		    glTranslatef(-viewTranslation.x, -viewTranslation.y, -viewTranslation.z);
		    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
		    glPopMatrix();

		    invViewMatrix[0] = modelView[0];
		    invViewMatrix[1] = modelView[4];
		    invViewMatrix[2] = modelView[8];
		    invViewMatrix[3] = modelView[12];
		    invViewMatrix[4] = modelView[1];
		    invViewMatrix[5] = modelView[5];
		    invViewMatrix[6] = modelView[9];
		    invViewMatrix[7] = modelView[13];
		    invViewMatrix[8] = modelView[2];
		    invViewMatrix[9] = modelView[6];
		    invViewMatrix[10] = modelView[10];
		    invViewMatrix[11] = modelView[14];

		    
		    render();

		  // draw image from PBO
		    glDisable(GL_DEPTH_TEST);

		    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		#if 0
		    // draw using glDrawPixels (slower)
		    glRasterPos2i(0, 0);
		    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
		    glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		#else
		    // draw using texture

		    // copy from pbo to texture
		    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
		    glBindTexture(GL_TEXTURE_2D, tex);
		    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

		    // draw textured quad
		    glEnable(GL_TEXTURE_2D);
		    glBegin(GL_QUADS);
		    glTexCoord2f(0.2, 0.2);
		    glVertex2f(0.2, 0.2);
		    glTexCoord2f(0.8, 0.2);
		    glVertex2f(0.8, 0.2);
		    glTexCoord2f(0.8, 0.8);
		    glVertex2f(0.8, 0.8);
		    glTexCoord2f(0.2, 0.8);
		    glVertex2f(0.2, 0.8);
		    glEnd();
		    glDisable(GL_TEXTURE_2D);
		    glBindTexture(GL_TEXTURE_2D, 0);
		#endif

		    loadMenu();
		    glutSwapBuffers();

		    glutReportErrors();

		    sdkStopTimer(&timer);

		    computeFPS();
	}
	else if(option==2)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0,0.75,0.14);
      	glRectf(0.0,0.0,1.0,1.0);

        glColor3f(0.59,0.43,1.0);
        glRectf(0.43,0.05,0.57,0.10);
        glColor3f(0.0,0.0,0.0);
        font=4;
        drawBitmapText("HOME",0.455,0.065,0);

        font=1;
        drawBitmapText("LARGE VOLUME DATA VISUALIZATION",0.13,0.90,0);
        font=5;
        drawBitmapText("Multimodal Visualization of large Volumetric Datasets with Ray Casting ",0.05,0.85,0);
        drawBitmapText("& Octree Traversal using Cuda C & OpenGL",0.24,0.83,0);
        font=1;
        drawBitmapText("Developed  By: ",0.40,0.70,0);
        font=4;
        drawBitmapText("Jalaz Kumar",0.16,0.60,0);
        drawBitmapText("Aman Agrawal",0.65,0.60,0);
        font=3;
        drawBitmapText("Department of Computer Science & Engineering",0.04,0.57,0);
        drawBitmapText("National Institute of Technology, Hamirpur(H.P.)",0.04,0.54,0);

        drawBitmapText("Department of Information Technology",0.60,0.57,0);
        drawBitmapText("Harcourt Butler Technical University, Kanpur(U.P.)",0.53,0.54,0);

        font=4;
        drawBitmapText("SUMMER RESEARCH INTERNSHIP PROGRAMME, 2017 ",0.14,0.45,0);

        font=1;
        drawBitmapText("Supervised By: ",0.40,0.30,0);
        font=4;
        drawBitmapText("Dr. Anupam Agrawal",0.38,0.24,0);
        font=3;
        drawBitmapText("Department of Information Technology",0.34,0.21,0);
        drawBitmapText("Indian Institute of Information Technology, Allahabad(U.P.)",0.25,0.18,0);        

    	glFinish();
    	glutSwapBuffers();
	}
}

void idle()
{
    glutPostRedisplay();
}

void cleanup()
{
    sdkDeleteTimer(&timer);

    freeCudaBuffers();

    if (pbo)
    {
        cudaGraphicsUnregisterResource(cuda_pbo_resource);
        glDeleteBuffersARB(1, &pbo);
        glDeleteTextures(1, &tex);
    }
}

	FILE *fpp1 ;
	void *data111;
	size_t siz2=6605433;
	size_t read1;

void keyboard(unsigned char key, int x, int y)
{
	
    switch (key)
    {
        case 'q':
		case 'Q':
            exit(EXIT_SUCCESS);
            break;

        case 'f':
            linearFiltering = !linearFiltering;
            setTextureFilterMode(linearFiltering);
            break;

        case '+':
            density += 0.01f;
            break;

        case '-':
            density -= 0.01f;
            break;

        case ']':
            brightness += 0.1f;
            break;

        case '[':
            brightness -= 0.1f;
            break;

        case ';':
            transferOffset += 0.01f;
            break;

        case '\'':
            transferOffset -= 0.01f;
            break;

        case '.':
            transferScale += 0.01f;
            break;

        case ',':
            transferScale -= 0.01f;
            break;

		case '0':
			if(currentFilePath=="root")
                currentFilePath="0";
            else
                currentFilePath=currentFilePath+"0";
            loadFile();
            break;

		case '1':
			if(currentFilePath=="root")
                currentFilePath="1";
            else
                currentFilePath=currentFilePath+"1";
            loadFile();
            break;
        case '2':
            if(currentFilePath=="root")
                currentFilePath="2";
            else
                currentFilePath=currentFilePath+"2";
            loadFile();
            break;
        case '3':
            if(currentFilePath=="root")
                currentFilePath="3";
            else
                currentFilePath=currentFilePath+"3";
            loadFile();
            break;
        case '4':
            if(currentFilePath=="root")
                currentFilePath="4";
            else
                currentFilePath=currentFilePath+"4";
            loadFile();
            break;
        case '5':
            if(currentFilePath=="root")
                currentFilePath="5";
            else
                currentFilePath=currentFilePath+"5";
            loadFile();
            break;
        case '6':
            if(currentFilePath=="root")
                currentFilePath="6";
            else
                currentFilePath=currentFilePath+"6";
            loadFile();
            break;
        case '7':
            if(currentFilePath=="root")
                currentFilePath="7";
            else
                currentFilePath=currentFilePath+"7";
            loadFile();
            break;
        case 'b':
             length=currentFilePath.length();
           if(length==1)
                currentFilePath="root";
            else if(length==2||length==3)
                currentFilePath=currentFilePath.substr(0,length-1);

            loadFile();
            break;
        case 'h':
            currentFilePath="root";
            loadFile();
            break;

        default:
            break;
    }

    printf("density = %.2f, brightness = %.2f, transferOffset = %.2f, transferScale = %.2f\n", density, brightness, transferOffset, transferScale);
    glutPostRedisplay();
}

int ox, oy;
int buttonState = 0;

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        buttonState  |= 1<<button;
    }
    else if (state == GLUT_UP)
    {
        buttonState = 0;
    }
    float tempx,tempy;
    tempx=x/(float)width;
    tempy=y/(float)height;

    float vRx=(int)viewRotation.x % 360;
    float vRy=(int)viewRotation.y % 360;
    if(vRx<0)
    	vRx+=360;
    if(vRy<0)
    	vRy+=360;


    if(buttonState==1)
    {
    	if(option==0&&tempx>85/(float)612 && tempx<215/(float)612&& tempy>50/(float)700 && tempy<95/(float)700)
    	{
    		length=currentFilePath.length();
           if(length==1)
                currentFilePath="root";
            else if(length==2||length==3)
                currentFilePath=currentFilePath.substr(0,length-1);

            loadFile();
    	}

    	if(option==0&&tempx>395/(float)612 && tempx<525/(float)612 && tempy>50/(float)700 && tempy<95/(float)700)
    	{
    		currentFilePath="root";
    		crop=0;
    		x2CurrentBox=x2OriginalBox;
    		y2CurrentBox=y2OriginalBox;
    		z2CurrentBox=z2OriginalBox;
    		x1RectangleCurrent=x1RectangleOriginal,y1RectangleCurrent=y1RectangleOriginal,x2RectangleCurrent=x2RectangleOriginal,y2RectangleCurrent=y2RectangleOriginal;
            loadFile();

    	}
    	if(option==0&&tempx>30/(float)612 && tempx<140/(float)612 &&tempy>630/(float)700 && tempy<668/(float)700)
    	{
    		option=2;
    	}
    	if(option==2&&tempx>265/(float)612 && tempx<350/(float)612 &&tempy>630/(float)700 && tempy<668/(float)700)
    	{
    		option=0;
    	}
    	if(option==0&&tempx>490/(float)612 && tempx<583/(float)612 &&tempy>630/(float)700 && tempy<668/(float)700)
    	{
    		exit(EXIT_SUCCESS);
    	}
    
    	if(option==0&&tempx>255/(float)612 && tempx<355/(float)612 &&tempy>595/(float)700 && tempy<630/(float)700)
    	{
	    	if(vRx>70.0 && vRx<120.0 && vRy>135.0 && vRy<225.0)
	    		orientation=1;
	    	if(vRx>70.0 && vRx<120.0 && (vRy>315.0 || vRy<45.0))
	    		orientation=2;
	    	if(vRx>70.0 && vRx<120.0 && vRy>225.0 && vRy<315.0)
	    		orientation=3;
	    	if(vRx>70.0 && vRx<120.0 && vRy>45.0 && vRy<135.0)
	    		orientation=4;


    	float temp1,temp2;
    	temp1=1.0-y1RectangleCurrent;
    	temp2=1.0-y2RectangleCurrent;

    	float minX,minY;
    	if(abs(x1RectangleCurrent-305/(float)612)>abs(x2RectangleCurrent-305/(float)612))
    	{
    		if(abs(temp1-348/(float)700)>abs(temp2-348/(float)700))
    			quadSelected=4;
    		else
    			quadSelected=1;
    	}
    	else
    	{	
    		if(abs(temp1-348/(float)700)>abs(temp2-348/(float)700))
    			quadSelected=3;
    		else
    			quadSelected=2;
    	}

    	cout<<quadSelected;

    	crop=0;
    	x1RectangleCurrent=x1RectangleOriginal;
    	y1RectangleCurrent=y1RectangleOriginal;
    	x2RectangleCurrent=x2RectangleOriginal;
    	y2RectangleCurrent=y2RectangleOriginal;
    	
    	if(orientation==1 && quadSelected==1)
    	{
			if(currentFilePath=="root")
                currentFilePath="1";
            else
                currentFilePath=currentFilePath+"1";
            loadFile();
    	}
    	if(orientation==1 && quadSelected==2)
    	{
			if(currentFilePath=="root")
                currentFilePath="0";
            else
                currentFilePath=currentFilePath+"0";
            loadFile();    		
    	}
    	if(orientation==1 && quadSelected==3)
    	{
            if(currentFilePath=="root")
                currentFilePath="4";
            else
                currentFilePath=currentFilePath+"4";
            loadFile();    		
    	}
    	if(orientation==1 && quadSelected==4)
    	{
            if(currentFilePath=="root")
                currentFilePath="5";
            else
                currentFilePath=currentFilePath+"5";
            loadFile();    		
    	}
    	if(orientation==2 && quadSelected==1)
    	{
            if(currentFilePath=="root")
                currentFilePath="2";
            else
                currentFilePath=currentFilePath+"2";
            loadFile();

    	}
    	if(orientation==2 && quadSelected==2)
    	{
            if(currentFilePath=="root")
                currentFilePath="3";
            else
                currentFilePath=currentFilePath+"3";
            loadFile();
    		
    	}
    	if(orientation==2 && quadSelected==3)
    	{
            if(currentFilePath=="root")
                currentFilePath="7";
            else
                currentFilePath=currentFilePath+"7";
            loadFile();
    		
    	}
    	if(orientation==2 && quadSelected==4)
    	{
            if(currentFilePath=="root")
                currentFilePath="6";
            else
                currentFilePath=currentFilePath+"6";
            loadFile();
    		
    	}
    	if(orientation==3 && quadSelected==1)
    	{
            if(currentFilePath=="root")
                currentFilePath="3";
            else
                currentFilePath=currentFilePath+"3";
            loadFile();
    	}
    	if(orientation==3 && quadSelected==2)
    	{
			if(currentFilePath=="root")
                currentFilePath="1";
            else
                currentFilePath=currentFilePath+"1";
            loadFile();    		
    	}
    	if(orientation==3 && quadSelected==3)
    	{
            if(currentFilePath=="root")
                currentFilePath="5";
            else
                currentFilePath=currentFilePath+"5";
            loadFile();       		
    	}
    	if(orientation==3 && quadSelected==4)
    	{
            if(currentFilePath=="root")
                currentFilePath="7";
            else
                currentFilePath=currentFilePath+"7";
            loadFile();    		
    	}
    	if(orientation==4 && quadSelected==1)
    	{
			if(currentFilePath=="root")
                currentFilePath="0";
            else
                currentFilePath=currentFilePath+"0";
            loadFile();    		

    	}
    	if(orientation==4 && quadSelected==2)
    	{
            if(currentFilePath=="root")
                currentFilePath="2";
            else
                currentFilePath=currentFilePath+"2";
            loadFile();    		
    	}
    	if(orientation==4 && quadSelected==3)
    	{
            if(currentFilePath=="root")
                currentFilePath="6";
            else
                currentFilePath=currentFilePath+"6";
            loadFile();
    		
    	}
    	if(orientation==4 && quadSelected==4)
    	{
            if(currentFilePath=="root")
                currentFilePath="4";
            else
                currentFilePath=currentFilePath+"4";
            loadFile();    		    		
    	}

    	cout<<"\n"<<quadSelected<<endl;
    	}

    }

    ox = x;
    oy = y;
    //cout<<x<<" "<<y<<endl;
    glutPostRedisplay();
}

void motion(int x, int y)
{

    float dx, dy;
    dx = (float)(x - ox);
    dy = (float)(y - oy);
    float tempx,tempy;
    tempx=x/(float)width;
    tempy=y/(float)height;
    if (buttonState == 4)
    {
        // right = zoom
        viewTranslation.z += dy / 100.0f;
    }
    else if (buttonState == 2)
    {
       // middle = translate
        //viewTranslation.x += dx / 100.0f;
        //viewTranslation.y -= dy / 100.0f;

    	tempy=1.0-tempy;

    	if(tempx>x1RectangleCurrent-0.02&&tempx<x1RectangleCurrent+0.02)
    		x1RectangleCurrent+=dx/width;
    	if(tempx>x2RectangleCurrent-0.02&&tempx<x2RectangleCurrent+0.02)
    		x2RectangleCurrent+=dx/width;

    	if(tempy>y1RectangleCurrent-0.02&&tempy<y1RectangleCurrent+0.02)
    		y1RectangleCurrent-=dy/height;
    	if(tempy>y2RectangleCurrent-0.02&&tempy<y2RectangleCurrent+0.02)
    		y2RectangleCurrent-=dy/height;

    	if(tempx>x1RectangleCurrent+0.05 && tempx <x2RectangleCurrent-0.05 && tempy>y1RectangleCurrent+0.05 && tempy<y2RectangleCurrent-0.05)
    	{
    		x1RectangleCurrent+=dx/width;
    		x2RectangleCurrent+=dx/width;
    		y1RectangleCurrent-=dy/height;
    		y2RectangleCurrent-=dy/height;
    	}


    	if(x1RectangleCurrent<x1RectangleOriginal)
    		x1RectangleCurrent=x1RectangleOriginal;
    	if(x2RectangleCurrent>x2RectangleOriginal)
    		x2RectangleCurrent=x2RectangleOriginal;

    	if(x1RectangleCurrent>x2RectangleCurrent-0.1)
    		x1RectangleCurrent=x2RectangleCurrent-0.1;
    	if(x2RectangleCurrent<x1RectangleCurrent+0.1)
    		x2RectangleCurrent=x1RectangleCurrent+0.1;
    	if(x2RectangleCurrent<x1RectangleOriginal+0.1)
    		x2RectangleCurrent=x1RectangleOriginal+0.1;
    	if(x1RectangleCurrent>x2RectangleOriginal-0.1)
    		x1RectangleCurrent=x2RectangleOriginal-0.1;


    	if(y1RectangleCurrent<y1RectangleOriginal)
    		y1RectangleCurrent=y1RectangleOriginal;
    	if(y2RectangleCurrent>y2RectangleOriginal)
    		y2RectangleCurrent=y2RectangleOriginal;

    	if(y1RectangleCurrent>y2RectangleCurrent-0.1)
    		y1RectangleCurrent=y2RectangleCurrent-0.1;
    	if(y2RectangleCurrent<y1RectangleCurrent+0.1)
    		y2RectangleCurrent=y1RectangleCurrent+0.1;
    	if(y2RectangleCurrent<y1RectangleOriginal+0.1)
    		y2RectangleCurrent=y1RectangleOriginal+0.1;
    	if(y1RectangleCurrent>y2RectangleOriginal-0.1)
    		y1RectangleCurrent=y2RectangleOriginal-0.1;

    	crop=1;


    }

    else if (buttonState == 1)
    {
        // left = rotate
        viewRotation.x += dy / 5.0f;
        viewRotation.y += dx / 5.0f;

    }

    ox = x;
    oy = y;

    glutPostRedisplay();
}

int iDivUp(int a, int b)
{
    return (a % b != 0) ? (a / b + 1) : (a / b);
}

void reshape(int w, int h)
{
    width = w;
    height = h;
    initPixelBuffer();

    // calculate new grid size
    gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}



void initGL(int *argc, char **argv)
{
    // initialize GLUT callback functions
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("CUDA volume rendering");

    glewInit();

    if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object"))
    {
        printf("Required OpenGL extensions missing.");
        exit(EXIT_SUCCESS);
    }
}

void initPixelBuffer()
{
    if (pbo)
    {
        // unregister this buffer object from CUDA C
        checkCudaErrors(cudaGraphicsUnregisterResource(cuda_pbo_resource));

        // delete old buffer
        glDeleteBuffersARB(1, &pbo);
        glDeleteTextures(1, &tex);
    }

    // create pixel buffer object for display
    glGenBuffersARB(1, &pbo);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, width*height*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    // register this buffer object with CUDA
    checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard));

    // create texture for display
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// General initialization call for CUDA Device
int chooseCudaDevice(int argc, const char **argv, bool bUseOpenGL)
{
    int result = 0;

    if (bUseOpenGL)
    {
        result = findCudaGLDevice(argc, argv);
    }
    else
    {
        result = findCudaDevice(argc, argv);
    }

    return result;
}

void runSingleTest(const char *ref_file, const char *exec_path)
{
    bool bTestResult = true;

    uint *d_output;
    checkCudaErrors(cudaMalloc((void **)&d_output, width*height*sizeof(uint)));
    checkCudaErrors(cudaMemset(d_output, 0, width*height*sizeof(uint)));

    float modelView[16] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 4.0f, 1.0f
    };

    invViewMatrix[0] = modelView[0];
    invViewMatrix[1] = modelView[4];
    invViewMatrix[2] = modelView[8];
    invViewMatrix[3] = modelView[12];
    invViewMatrix[4] = modelView[1];
    invViewMatrix[5] = modelView[5];
    invViewMatrix[6] = modelView[9];
    invViewMatrix[7] = modelView[13];
    invViewMatrix[8] = modelView[2];
    invViewMatrix[9] = modelView[6];
    invViewMatrix[10] = modelView[10];
    invViewMatrix[11] = modelView[14];

    // call CUDA kernel, writing results to PBO
    copyInvViewMatrix(invViewMatrix, sizeof(float4)*3);

    // Start timer 0 and process n loops on the GPU
    int nIter = 10;

    for (int i = -1; i < nIter; i++)
    {
        if (i == 0)
        {
            cudaDeviceSynchronize();
            sdkStartTimer(&timer);
        }

        render_kernel(gridSize, blockSize, d_output, width, height, density, brightness, transferOffset, transferScale,x1CurrentBox,y1CurrentBox,z1CurrentBox,x2CurrentBox,y2CurrentBox,z2CurrentBox);
    }

    cudaDeviceSynchronize();
    sdkStopTimer(&timer);
    // Get elapsed time and throughput, then log to sample and master logs
    double dAvgTime = sdkGetTimerValue(&timer)/(nIter * 1000.0);
    printf("volumeRender, Throughput = %.4f MTexels/s, Time = %.5f s, Size = %u Texels, NumDevsUsed = %u, Workgroup = %u\n",
           (1.0e-6 * width * height)/dAvgTime, dAvgTime, (width * height), 1, blockSize.x * blockSize.y);


    getLastCudaError("Error: render_kernel() execution FAILED");
    checkCudaErrors(cudaDeviceSynchronize());

    unsigned char *h_output = (unsigned char *)malloc(width*height*4);
    checkCudaErrors(cudaMemcpy(h_output, d_output, width*height*4, cudaMemcpyDeviceToHost));

    sdkSavePPM4ub("volume.ppm", h_output, width, height);
    bTestResult = sdkComparePPM("volume.ppm", sdkFindFilePath(ref_file, exec_path), MAX_EPSILON_ERROR, THRESHOLD, true);

    cudaFree(d_output);
    free(h_output);
    cleanup();

    exit(bTestResult ? EXIT_SUCCESS : EXIT_FAILURE);
}

void loadFile()
{

        size_t siz;
        siz = 6605433;
        cout << siz << endl;
        siz = siz * sizeof(VolumeType);

        size_t bsiz;
        bsiz = siz;
        bsiz = 6605433;

        VolumeType *ss;

        cudaMalloc((VolumeType**)&ss, siz); //buffer
        
        string temp1=pathIndex+currentFilePath;
        string temp=temp1+extension;
        cout<<"\n\n---------------------------------------"<<temp<<endl;
        FILE *fpp = fopen(temp.c_str(), "rb");
        cout<<"Calling malloc() to allocate memory"<<endl;
        void *data11 = malloc(siz);
        size_t read = fread(data11, 1, bsiz, fpp);
        cout << read;

        initCuda(data11, volumeSize);
        fclose(fpp); 
        free(data11);
}
int main(int argc, char **argv)
{
	data111 = (void *)malloc(siz2);
    pArgc = &argc;
    pArgv = argv;

    char *ref_file = NULL;
	int po;
	
    //start logs
    printf("%s Starting...\n\n", sSDKsample);

    if (checkCmdLineFlag(argc, (const char **)argv, "file"))
    {
        getCmdLineArgumentString(argc, (const char **)argv, "file", &ref_file);
        fpsLimit = frameCheckNumber;
    }

    cout<<"\n    -------- First if crossed-------\n";

    if (ref_file)
    {
        // use command-line specified CUDA device, otherwise use device with highest Gflops/s
        chooseCudaDevice(argc, (const char **)argv, false);
    }
    else
    {
        // First initialize OpenGL context, so we can properly set the GL for CUDA.
        // This is necessary in order to achieve optimal performance with OpenGL/CUDA interop.
        initGL(&argc, argv);

        // use command-line specified CUDA device, otherwise use device with highest Gflops/s
        chooseCudaDevice(argc, (const char **)argv, true);
    }


    // parse arguments
    char *filename;
	
    if (getCmdLineArgumentString(argc, (const char **) argv, "volume", &filename))
    {
        volumeFilename = filename;
    }

    int n;

    if (checkCmdLineFlag(argc, (const char **) argv, "size"))
    {
        n = getCmdLineArgumentInt(argc, (const char **) argv, "size");
        volumeSize.width = volumeSize.height = volumeSize.depth = n;
    }

    if (checkCmdLineFlag(argc, (const char **) argv, "xsize"))
    {
        n = getCmdLineArgumentInt(argc, (const char **) argv, "xsize");
        volumeSize.width = n;
    }

    if (checkCmdLineFlag(argc, (const char **) argv, "ysize"))
    {
        n = getCmdLineArgumentInt(argc, (const char **) argv, "ysize");
        volumeSize.height = n;
    }

    if (checkCmdLineFlag(argc, (const char **) argv, "zsize"))
    {
        n= getCmdLineArgumentInt(argc, (const char **) argv, "zsize");
        volumeSize.depth = n;
    }
	
	int div = 640;
	int nu, rest;
	nu = volumeSize.depth/div;
	rest = volumeSize.depth%div;
	size_t size;
	//volumeSize.depth = 626;
	//376

    // load volume data
    char *path = sdkFindFilePath(volumeFilename, argv[0]);

    if (path == 0)
    {
        printf("Error finding file '%s'\n", volumeFilename);
        exit(EXIT_FAILURE);
    }
    loadFile();
	
    sdkCreateTimer(&timer);

    printf("Press '+' and '-' to change density (0.01 increments)\n"
           "      ']' and '[' to change brightness\n"
           "      ';' and ''' to modify transfer function offset\n"
           "      '.' and ',' to modify transfer function scale\n\n");

    // calculate new grid size
    gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

    if (ref_file)
    {
        runSingleTest(ref_file, argv[0]);
    }
    else
    {
        // This is the normal rendering path for VolumeRender
        glutDisplayFunc(display);
        glutKeyboardFunc(keyboard);
        glutMouseFunc(mouse);
        glutMotionFunc(motion);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);

        initPixelBuffer();

        atexit(cleanup);
        glutMainLoop();
    }

    cudaDeviceReset();
    exit(EXIT_SUCCESS);
}
