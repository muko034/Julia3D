#include "julia3d.h"
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

#define VERT_SHADER_PATH "shader/julia.vert"
#define FRAG_SHADER_PATH "shader/julia.frag"

using namespace glm;


Julia3D::Julia3D()
	: m_vbo(0u),
	  m_eye(0.0, 0.0, -2.0),
	//m_q(-0.591,-0.399,0.339,0.437),
	  m_q(0.0f, 0.0f, 0.0f, 0.0f),
	  m_slice(0.0f),
	  m_step(0.01f),
	  m_maxIterations(8),
	  m_alpha(0.0f),
	  m_beta(0.0f),
	  m_shadows(false)
{
}


Julia3D::~Julia3D()
{
}

void Julia3D::init()
{
	m_prog.compileAndLinkShaders(VERT_SHADER_PATH, FRAG_SHADER_PATH);

	// Create the vertex buffer object
	glGenBuffers(1, &m_vbo);
}

void Julia3D::render()
{
	mat4 rotX = glm::rotate(glm::mat4(1.0f), m_alpha, vec3(1.0f, 0.0f, 0.0f));
	mat4 rotY = glm::rotate(glm::mat4(1.0f), m_beta, vec3(0.0f, 1.0f, 0.0f));
	mat4 rot = rotX*rotY;
	mat4 trans = glm::translate(glm::mat4(1.0f), m_eye);
	mat4 cameraToWorld = rot*trans*rot;


	/////////////////// Create the VBO ////////////////////
    float positionData[] = { -1.0f, -1.0f,
							  1.0f, -1.0f,
							 -1.0f,  1.0f, 
							  1.0f,  1.0f };

	// Create and populate the buffer objects
    GLuint handle;
    glGenBuffers(1, &handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positionData, GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays(1, &m_vbo);
    glBindVertexArray(m_vbo);

    glEnableVertexAttribArray(0);  // Vertex position

    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

	m_prog.use();

	m_prog.setUniform("u_q", m_q);
	m_prog.setUniform("u_slice", m_slice);
	m_prog.setUniform("u_maxIterations", m_maxIterations);
	m_prog.setUniform("u_cameraToWorld", cameraToWorld);
	m_prog.setUniform("u_renderShadows", m_shadows);
	

	glBindVertexArray(m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glDisableVertexAttribArray(/*attribute_coord*/0);
}
