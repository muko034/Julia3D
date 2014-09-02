//#include <GLTools.h>
//#include <GLShaderManager.h>
//#define FREEGLUT_STATIC

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <iostream>
#include <map>
#include <utility>
#include <functional>

#include "glutils.h"
#include "julia3d.h"
#include "text2d.h"

using namespace std;
using namespace std::placeholders;

float gFps = 0;
Julia3D g_julia3d;
Text2D g_text2d;
int g_width;
int g_height;
unsigned char g_activeKey = 0;
float g_step = 0.01f;
 
void ChangeSize(int w, int h)
{
	glViewport(0,0,w,h);
	g_width = w;
    g_height = h;
}

void SetupRC()
{
    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        cout <<"Error initializing GLEW: " << glewGetErrorString(err) << endl;
    }
    GLUtils::checkForOpenGLError(__FILE__,__LINE__);

    GLUtils::dumpGLInfo();

	g_julia3d.init();
	g_text2d.init();
	
}

void RenderScene()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	g_julia3d.render();

	float sx = 2.0 / g_width;
	float sy = 2.0 / g_height;
	
	g_text2d.setFontSize(floor(g_height*0.03));
	g_text2d.render(-0.97, 1-g_text2d.getFontSize()*sy, sx, sy);

	// Zamiana buforow
	glutSwapBuffers();
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

void Idle (void)
{
    //  Animate the object

    //  Calculate FPS
    calculateFPS();
	g_text2d.clearText();
	g_text2d.addLineText(string("FPS: ") + to_string(gFps));
	g_text2d.addLineText(string("[x] q_x: ") + to_string(g_julia3d.getQ().x));
	g_text2d.addLineText(string("[y] q_y: ") + to_string(g_julia3d.getQ().y));
	g_text2d.addLineText(string("[z] q_z: ") + to_string(g_julia3d.getQ().z));
	g_text2d.addLineText(string("[w] q_w: ") + to_string(g_julia3d.getQ().w));
	g_text2d.addLineText(string("[d] slice: ") + to_string(g_julia3d.getSlice()));
	g_text2d.addLineText(string("[i] max iterations: ") + to_string(g_julia3d.getMaxIterations()));

    //  Call display function (draw the current frame)
    glutPostRedisplay ();
}

void OnKey(unsigned char key, int xmouse, int ymouse)
{
	if (key == '+' || key == '-')
	{
		map< unsigned char, pair< function<void(void)>, function<void(void)> > > func;
		func['x'] = make_pair(bind(&Julia3D::incQx, &g_julia3d, g_step), bind(&Julia3D::decQx, &g_julia3d, g_step));
		func['y'] = make_pair(bind(&Julia3D::incQy, &g_julia3d, g_step), bind(&Julia3D::decQy, &g_julia3d, g_step));
		func['z'] = make_pair(bind(&Julia3D::incQz, &g_julia3d, g_step), bind(&Julia3D::decQz, &g_julia3d, g_step));
		func['w'] = make_pair(bind(&Julia3D::incQw, &g_julia3d, g_step), bind(&Julia3D::decQw, &g_julia3d, g_step));
		func['d'] = make_pair(bind(&Julia3D::incSlice, &g_julia3d, g_step), bind(&Julia3D::decSlice, &g_julia3d, g_step));
		func['i'] = make_pair(bind(&Julia3D::incMaxIterations, &g_julia3d), bind(&Julia3D::decMaxIterations, &g_julia3d));
		try
		{

			if (key == '+') 
			{
				func.at(g_activeKey).first();
			} else if (key == '-')
			{
				func.at(g_activeKey).second();
			}
		}
		catch (out_of_range) { }
	} 
	else if ( key == 'x' ||
			  key == 'y' ||
			  key == 'z' ||
			  key == 'w' ||
			  key == 'i' ||
			  key == 'd'    )
	{
		g_activeKey = key;
	}
}

void OnSpecialKey(int key, int, int)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:
			g_julia3d.eyeLeft();
			break;
		case GLUT_KEY_RIGHT:
			g_julia3d.eyeRight();
			break;
		case GLUT_KEY_UP:
			g_julia3d.eyeUp();
			break;
		case GLUT_KEY_DOWN:
			g_julia3d.eyeDown();
			break;
		default:
			break;
	}
}

void OnMouseWheel( int wheel, int direction, int x, int y )
{
	if (direction > 0)
	{
		g_julia3d.zoomIn();
	}
	else
	{
		g_julia3d.zoomOut();
	}
}
 
// Entry point - GLUT setup and initialization
int main( int argc, char** argv )
{
	//gltSetWorkingDirectory(argv[0]);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(800, 600);
	glutCreateWindow("Julia3D");
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutIdleFunc(Idle);
	glutKeyboardFunc(OnKey);
	glutSpecialFunc(OnSpecialKey);
	glutMouseWheelFunc(OnMouseWheel);

	SetupRC();

	glutMainLoop();
	return 0;
}