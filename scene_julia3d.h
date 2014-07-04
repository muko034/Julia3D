#ifndef SCENEJULIA3D
#define SCENEJULIA3D

#include "scene.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

class SceneJulia3D : public Scene
{
private:
    int width, height;
    GLuint vboHandles[2];
    GLuint vaoHandle;
	GLuint programHandle;

    void linkMe(GLint vertShader, GLint fragShader);
	void printActiveUniforms(GLuint);

public:
    //SceneJulia3D();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEJULIA3D