#ifndef SCENEJULIA3D
#define SCENEJULIA3D

#include "scene.h"
#include "glslprogram.h"

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

	void compileAndLinkShader();

public:
    SceneJulia3D();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
	virtual void onKey(unsigned char key, int xmouse, int ymouse);
};

#endif // SCENEJULIA3D