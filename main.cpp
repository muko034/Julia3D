//#include <GLTools.h>
//#include <GLShaderManager.h>
//#define FREEGLUT_STATIC

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <iostream>

#include <ft2build.h> 
#include FT_FREETYPE_H

#include "scene_julia3d.h"
#include "glutils.h"

using namespace std;

Scene *gScene;
float gFps = 0;

void drawFPS();
 
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void SetupRC()
{
    //////////////// PLUG IN SCENE HERE /////////////////
	gScene = new SceneJulia3D();
    ////////////////////////////////////////////////////

    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        cout <<"Error initializing GLEW: " << glewGetErrorString(err) << endl;
    }
    GLUtils::checkForOpenGLError(__FILE__,__LINE__);

    GLUtils::dumpGLInfo();

    gScene->initScene();
}

void RenderScene()
{
	// czyszczenie okna
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	gScene->update(0.0f);
	gScene->render();

	drawFPS();

	// Zamiana buforow
	glutSwapBuffers();
	//// petla animacji
	//glutPostRedisplay();
}

//-------------------------------------------------------------------------
// Calculates the frames per second
//-------------------------------------------------------------------------
void calculateFPS()
{
	static int frameCount = 0;
	static int currentTime = 0;
	static int previousTime = 0;

    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since glutInit called 
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        gFps = frameCount / (timeInterval / 1000.0f);

        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }
}

//-------------------------------------------------------------------------
//  Draws a string at the specified coordinates.
//-------------------------------------------------------------------------
void printw (float x, float y, float z, char* format, ...)
{
	//  Pointer to a font style..
	//  Fonts supported by GLUT are: GLUT_BITMAP_8_BY_13, 
	//  GLUT_BITMAP_9_BY_15, GLUT_BITMAP_TIMES_ROMAN_10, 
	//  GLUT_BITMAP_TIMES_ROMAN_24, GLUT_BITMAP_HELVETICA_10,
	//  GLUT_BITMAP_HELVETICA_12, and GLUT_BITMAP_HELVETICA_18.
	GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;

	va_list args;	//  Variable argument list
	int len;		//	String length
	int i;			//  Iterator
	char * text;	//	Text

	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	//  _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1; 

	//  Allocate memory for a string of the specified size
	text = (char *)malloc(len * sizeof(char));

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list 
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos3f (x, y, z);

	//  Draw the characters one by one
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

	//  Free the allocated memory for the string
	free(text);
}

//-------------------------------------------------------------------------
//  Draw FPS
//-------------------------------------------------------------------------
void drawFPS()
{
    //  Load the identity matrix so that FPS string being drawn
    //  won't get animates
	glLoadIdentity ();

	//  Print the FPS to the window
	printw (-0.9, -0.9, 0, "FPS: %4.2f", gFps);
}

void Idle (void)
{
    //  Animate the object
	gScene->update(0.0f);

    //  Calculate FPS
    calculateFPS();

    //  Call display function (draw the current frame)
    glutPostRedisplay ();
}

void OnKey(unsigned char key, int xmouse, int ymouse)
{
	gScene->onKey(key, xmouse, ymouse);
}

void OnSpecialKey(int key, int, int)
{
	gScene->onSpecialKey(key);
}

void OnMouseWheel( int wheel, int direction, int x, int y )
{
	gScene->onMouseWheel(direction);
}
 
// Entry point - GLUT setup and initialization
int main( int argc, char** argv )
{
	//gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Julia3D");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(OnKey);
	glutSpecialFunc(OnSpecialKey);
	glutMouseWheelFunc (OnMouseWheel);

	SetupRC();

	glutMainLoop();
	return 0;
}