//#include <GLTools.h>
//#include <GLShaderManager.h>
//#define FREEGLUT_STATIC

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <iostream>
#include <map>
//#include <utility>
#include <tuple>
#include <functional>

#include "glutils.h"
#include "julia3d.h"
#include "text2d.h"

using namespace std;
using namespace std::placeholders;

struct Pos2D {
	int x;
	int y;
	Pos2D() : x(0), y(0) {}
	Pos2D(int _x, int _y) : x(_x), y(_y) {}
};

float gFps = 0;
Julia3D g_julia3d;
Text2D g_text2d;
int g_width;
int g_height;
int g_screenWidth;
int g_screenHeight;
unsigned char g_activeKey = 0;
float g_step = 0.01f;
Pos2D g_lastMousePos;
bool g_PPM = false;
map< unsigned char, 
	 tuple< int, 
			function<void(void)>, 
			function<void(void)> > > g_param;
 
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

	g_screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	g_screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	g_param['1'] = make_tuple(1, bind(&Julia3D::incQx, &g_julia3d), bind(&Julia3D::decQx, &g_julia3d));
	g_param['2'] = make_tuple(2, bind(&Julia3D::incQy, &g_julia3d), bind(&Julia3D::decQy, &g_julia3d));
	g_param['3'] = make_tuple(3, bind(&Julia3D::incQz, &g_julia3d), bind(&Julia3D::decQz, &g_julia3d));
	g_param['4'] = make_tuple(4, bind(&Julia3D::incQw, &g_julia3d), bind(&Julia3D::decQw, &g_julia3d));
	g_param['z'] = make_tuple(5, bind(&Julia3D::incSlice, &g_julia3d), bind(&Julia3D::decSlice, &g_julia3d));
	g_param['x'] = make_tuple(6, bind(&Julia3D::incMaxIterations, &g_julia3d), bind(&Julia3D::decMaxIterations, &g_julia3d));
	g_param['c'] = make_tuple(7, bind(&Julia3D::incStep, &g_julia3d), bind(&Julia3D::decStep, &g_julia3d));
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
	g_text2d.addLineText(string("[1] q_x: ") + to_string(g_julia3d.getQ().x));
	g_text2d.addLineText(string("[2] q_y: ") + to_string(g_julia3d.getQ().y));
	g_text2d.addLineText(string("[3] q_z: ") + to_string(g_julia3d.getQ().z));
	g_text2d.addLineText(string("[4] q_w: ") + to_string(g_julia3d.getQ().w));
	g_text2d.addLineText(string("[z] slice: ") + to_string(g_julia3d.getSlice()));
	g_text2d.addLineText(string("[x] max iterations: ") + to_string(g_julia3d.getMaxIterations()));
	g_text2d.addLineText(string("[c] step: ") + to_string(g_julia3d.getStep()));

    //  Call display function (draw the current frame)
    glutPostRedisplay ();
}

void OnKey(unsigned char key, int xmouse, int ymouse)
{
	if (key == 'w')
	{
		g_julia3d.zoomIn();
	}
	else if (key == 's')
	{
		g_julia3d.zoomOut();
	}
	else if (key == 'a')
	{
		g_julia3d.eyeLeft();
	}
	else if (key == 'd')
	{
		g_julia3d.eyeRight();
	}
	else if (key == 'q')
	{
		g_julia3d.eyeUp();
	}
	else if (key == 'e')
	{
		g_julia3d.eyeDown();
	}
	else if ( key == '1' ||
			  key == '2' ||
			  key == '3' ||
			  key == '4' ||
			  key == 'z' ||
			  key == 'x' ||
			  key == 'c'   )
	{
		g_activeKey = key;
		try
		{
			g_text2d.setActiveLine(get<0>(g_param.at(g_activeKey)));
		}
		catch (out_of_range) { }
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
	try
	{
		if (direction > 0)
		{
			get<1>(g_param.at(g_activeKey))();
		}
		else
		{
			get<2>(g_param.at(g_activeKey))();
		}
	}
	catch (out_of_range) { }
}

void OnMouseMotion(int x, int y)
{
	if (g_PPM)
	{
		float diffX = x-g_lastMousePos.x;
		g_julia3d.incBeta(diffX/(float)g_screenWidth * 360);
		float diffY = y-g_lastMousePos.y;
		g_julia3d.incAlpha(diffY/(float)g_screenHeight * 360);

		g_lastMousePos.x = x;
		g_lastMousePos.y = y;
	}
}

void OnMouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_lastMousePos.x = x;
			g_lastMousePos.y = y;
			g_PPM = true;
		}
		else
		{
			g_PPM = false;
		}
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
	glutMotionFunc(OnMouseMotion);
	glutMouseFunc(OnMouse);

	SetupRC();

	glutMainLoop();
	return 0;
}