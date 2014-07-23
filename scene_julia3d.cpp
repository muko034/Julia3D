#include "scene_julia3d.h"

#include <cstdio>
#include <cstdlib>

#include <fstream>
using std::ifstream;
#include <sstream>
using std::ostringstream;

#include "glutils.h"

#define VERT_SHADER_PATH "shader/julia.vert"
#define FRAG_SHADER_PATH "shader/julia.frag"

SceneJulia3D::SceneJulia3D()
	: m_bgColor(0.3, 0.3, 0.3, 1.0),
	  u_rO(0.0, 0.0, 2.0),
	  u_c(-0.591,-0.399,0.339,0.437)
	  //u_c( 0.439000, -0.389000, 0.359000, -0.333000 )
{
}

void SceneJulia3D::initScene()
{
	compileAndLinkShader();
	
	//t³o
	glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, m_bgColor.w);

    glEnable(GL_DEPTH_TEST);

    /////////////////// Create the VBO ////////////////////
    float positionData[] = {
        //-1.0f, -1.0f,
		-1.0f, -0.8f,
         //1.0f, -1.0f,
		 1.0f, -0.8f,
        -1.0f,  1.0f, 
		 1.0f,  1.0f };

	// Create and populate the buffer objects
    GLuint handle[1];
    glGenBuffers(1, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positionData, GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
}

void SceneJulia3D::update( float t )
{

}

void SceneJulia3D::render()
{
	printf("mu: ( %f, %f, %f, %f )\n", u_c.x, u_c.y, u_c.z, u_c.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog.setUniform("u_r0", u_rO);
	prog.setUniform("u_c", u_c);

    glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
}

void SceneJulia3D::resize(int w, int h)
{
    glViewport(0,0,w,h);
	width = w;
    height = h;
}

void SceneJulia3D::onKey(unsigned char key, int xmouse, int ymouse)
{
        switch (key)
        {
			case '+': u_rO.z -= 0.1f;
                break;
            case '-': u_rO.z += 0.1f;
                break;
            case 'w': u_rO.y += 0.1f;
                break;
			case 'a': u_rO.x -= 0.1f;
                break;
            case 's': u_rO.y -= 0.1f;
                break;
            case 'd': u_rO.x += 0.1f;
                break;
			case 't': u_c.x += 0.01f;
                break;
			case 'g': u_c.x -= 0.01f;
                break;
			case 'y': u_c.y += 0.01f;
                break;
			case 'h': u_c.y -= 0.01f;
                break;
			case 'u': u_c.z += 0.01f;
                break;
			case 'j': u_c.z -= 0.01f;
                break;
			case 'i': u_c.w += 0.01f;
                break;
			case 'k': u_c.w -= 0.01f;
                break;
            default:
                break;
        };
}

void SceneJulia3D::compileAndLinkShader()
{
	if( ! prog.compileShaderFromFile(VERT_SHADER_PATH, GLSLShader::VERTEX) )
    {
        printf("Vertex shader failed to compile!\n%s",
               prog.log().c_str());
		system("pause");
        exit(1);
    }
	if( ! prog.compileShaderFromFile(FRAG_SHADER_PATH, GLSLShader::FRAGMENT))
    {
        printf("Fragment shader failed to compile!\n%s",
               prog.log().c_str());
		system("pause");
        exit(1);
    }
    if( ! prog.link() )
    {
        printf("Shader program failed to link!\n%s",
               prog.log().c_str());
		system("pause");
        exit(1);
    }

    if( ! prog.validate() )
    {
        printf("Program failed to validate!\n%s",
               prog.log().c_str());
		system("pause");
        exit(1);
    }

    prog.use();
}
