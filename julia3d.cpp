#include "julia3d.h"

#define VERT_SHADER_PATH "shader/julia.vert"
#define FRAG_SHADER_PATH "shader/julia.frag"


Julia3D::Julia3D()
	: m_vbo(0u),
	  m_eye(0.0, 0.0, 2.0),
	  m_q(-0.591,-0.399,0.339,0.437),
	  m_slice(0.0f),
	  m_step(0.01f),
	  m_maxIterations(8)
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

	m_prog.setUniform("u_r0", m_eye);
	m_prog.setUniform("u_c", m_q);
	m_prog.setUniform("wCoord", m_slice);
	m_prog.setUniform("u_maxIterations", m_maxIterations);

	glBindVertexArray(m_vbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glDisableVertexAttribArray(/*attribute_coord*/0);
}
