#ifndef SCENEJULIA3D
#define SCENEJULIA3D

#include "scene.h"
#include "glslprogram.h"
#include "text2d.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp> 

class SceneJulia3D : public Scene
{
private:
	GLSLProgram prog;

    int width, height;
    GLuint vboHandles[2];
    GLuint vaoHandle;
	glm::vec4 m_bgColor;
	glm::vec3 u_rO;
	glm::vec4 u_c;
	GLfloat u_wCoord;
	Text2D text2d;

	void compileAndLinkShader();

public:
    SceneJulia3D();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
	virtual void onKey(unsigned char key, int xmouse, int ymouse);
	virtual void onSpecialKey(int key);
	virtual void onMouseWheel(int direction);
	void eyeUp();
	void eyeDown();
	void eyeLeft();
	void eyeRight();
	void zoomIn();
	void zoomOut();
};

#endif // SCENEJULIA3D