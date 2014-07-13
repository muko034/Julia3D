#ifndef SCENEJULIA3D
#define SCENEJULIA3D

#include "scene.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp> 

class SceneJulia3D : public Scene
{
private:
    int width, height;
    GLuint vboHandles[2];
    GLuint vaoHandle;
	GLuint programHandle;

    void linkMe(GLint vertShader, GLint fragShader);
	void printActiveUniforms(GLuint);

private:
	struct
	{
		glm::vec3 rO;
	} uniforms;

public:
    SceneJulia3D();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
	virtual void onKey(unsigned char key, int xmouse, int ymouse);
};

#endif // SCENEJULIA3D