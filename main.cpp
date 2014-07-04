//#include <GLTools.h>
//#include <GLShaderManager.h>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <cstdio>
#include <iostream>

#include "scene_julia3d.h"
#include "glutils.h"

using namespace std;

Scene *gScene;
 
void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void SetupRC()
{
	// tlo
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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

    glClearColor(0.2f,0.2f,0.2f,1.0f);

    gScene->initScene();
}

void RenderScene()
{
	// czyszczenie okna
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	gScene->update(0.0f);
	gScene->render();

	// Zamiana buforow
	glutSwapBuffers();
	// petla animacji
	glutPostRedisplay();
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

	SetupRC();

	glutMainLoop();
	return 0;
}