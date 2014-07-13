#include "scene_julia3d.h"

#include <cstdio>
#include <cstdlib>

#include <fstream>
using std::ifstream;
#include <sstream>
using std::ostringstream;

#include "glutils.h"

SceneJulia3D::SceneJulia3D()
{
	uniforms.rO = glm::vec3(0.0, 0.0, 2.0);
}

void SceneJulia3D::initScene()
{
    //////////////////////////////////////////////////////
    /////////// Vertex shader //////////////////////////
    //////////////////////////////////////////////////////

    GLchar * shaderCode;

    // Load contents of file into shaderCode here…
    ifstream inFile( "shader/julia.vert", ifstream::in );
    if( !inFile ) {
        fprintf(stderr, "Error opening file: shader/julia.vert\n" );
		system("pause");
        exit(1);
    }

    shaderCode = (char *)malloc(10000);
    int i = 0;
    while( inFile.good() ) {
        int c = inFile.get();
        shaderCode[i++] = c;
    }
    inFile.close();
    shaderCode[i++] = '\0';
    ////////////////////////////////////////////

    // Create the shader object
    GLuint vertShader = glCreateShader( GL_VERTEX_SHADER );
    if( 0 == vertShader )
    {
      fprintf(stderr, "Error creating vertex shader.\n");
	  system("pause");
      exit(1);
    }

    // Load the source code into the shader object
    const GLchar* codeArray[] = {shaderCode};
    glShaderSource( vertShader, 1, codeArray, NULL );
    free(shaderCode); // can be removed from book.

    // Compile the shader
    glCompileShader( vertShader );

    // Check compilation status
    GLint result;
    glGetShaderiv( vertShader, GL_COMPILE_STATUS, &result );
    if( GL_FALSE == result )
    {

       fprintf( stderr, "Vertex shader compilation failed!\n" );

       GLint logLen;
       glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &logLen );

       if( logLen > 0 )
       {
           char * log = (char *)malloc(logLen);

           GLsizei written;
           glGetShaderInfoLog(vertShader, logLen, &written, log);

           fprintf(stderr, "Shader log: \n%s", log);

           free(log);
       }
    }

    //////////////////////////////////////////////////////
    /////////// Fragment shader //////////////////////////
    //////////////////////////////////////////////////////

    //GLchar * shaderCode;

    // Load contents of file into shaderCode here…
    ifstream fragFile( "shader/julia.frag", ifstream::in );
    if( !fragFile ) {
        fprintf(stderr, "Error opening file: shader/julia.frag\n" );
		system("pause");
        exit(1);
    }

    shaderCode = (char *)malloc(10000);
    i = 0;
    while( fragFile.good() ) {
        int c = fragFile.get();
        shaderCode[i++] = c;
    }
    inFile.close();
    shaderCode[i++] = '\0';
    ////////////////////////////////////////////

    // Create the shader object
    GLuint fragShader = glCreateShader( GL_FRAGMENT_SHADER );
    if( 0 == fragShader )
    {
      fprintf(stderr, "Error creating fragment shader.\n");
	  system("pause");
      exit(1);
    }

    // Load the source code into the shader object
    //const GLchar* codeArray[] = {shaderCode};
    codeArray[0] = shaderCode;
    glShaderSource( fragShader, 1, codeArray, NULL );
    free(shaderCode); // can be removed from book.

    // Compile the shader
    glCompileShader( fragShader );

    // Check compilation status
    //GLint result;
    glGetShaderiv( fragShader, GL_COMPILE_STATUS, &result );
    if( GL_FALSE == result )
    {

       fprintf( stderr, "Fragment shader compilation failed!\n" );

       GLint logLen;
       glGetShaderiv( fragShader, GL_INFO_LOG_LENGTH, &logLen );

       if( logLen > 0 )
       {
           char * log = (char *)malloc(logLen);

           GLsizei written;
           glGetShaderInfoLog(fragShader, logLen, &written, log);

           fprintf(stderr, "Shader log: \n%s", log);

           free(log);
       }
    }

    linkMe(vertShader, fragShader);

    /////////////////// Create the VBO ////////////////////
    float positionData[] = {
        -1.0f, -1.0f, 
         1.0f, -1.0f, 
        -1.0f,  1.0f, 
		 1.0f,  1.0f };

    // Create and populate the buffer objects
    GLuint vboHandles[1];
    glGenBuffers(1, vboHandles);
    GLuint positionBufferHandle = vboHandles[0];

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), positionData, GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays( 1, &vaoHandle );
    glBindVertexArray(vaoHandle);

    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

}

void SceneJulia3D::linkMe(GLint vertShader, GLint fragShader)
{
    // Create the program object
    GLuint programHandle = glCreateProgram();
    if( 0 == programHandle )
    {
        fprintf(stderr, "Error creating program object.\n");
		system("pause");
        exit(1);
    }

    // Bind index 0 to the shader input variable "VertexPosition"
    glBindAttribLocation(programHandle, 0, "vertexPosition");

    // Attach the shaders to the program object
    glAttachShader( programHandle, vertShader );
    glAttachShader( programHandle, fragShader );

    // Link the program
    glLinkProgram( programHandle );

    // Check for successful linking
    GLint status;
    glGetProgramiv( programHandle, GL_LINK_STATUS, &status );
    if( GL_FALSE == status ) {

        fprintf( stderr, "Failed to link shader program!\n" );

        GLint logLen;
        glGetProgramiv( programHandle, GL_INFO_LOG_LENGTH, &logLen );

        if( logLen > 0 )
        {
            char * log = (char *)malloc(logLen);

            GLsizei written;
            glGetProgramInfoLog(programHandle, logLen, &written, log);

            fprintf(stderr, "Program log: \n%s", log);

            free(log);
        }
    }
    else
    {
        glUseProgram( programHandle );
		this->programHandle = programHandle;
        printActiveUniforms(programHandle);
    }
}

void SceneJulia3D::update( float t )
{

}

void SceneJulia3D::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

	GLuint rOLoc = glGetUniformLocation(programHandle, "u_r0");
	if( rOLoc >= 0 )
	{
		glUniform3fv(rOLoc, 1, &uniforms.rO[0]);
	}

    glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
}

void SceneJulia3D::resize(int w, int h)
{
    glViewport(0,0,w,h);
}

void SceneJulia3D::printActiveUniforms(GLuint programHandle) {

    GLint nUniforms, size, location, maxLen;
    GLchar * name;
    GLsizei written;
    GLenum type;

    glGetProgramiv( programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
    glGetProgramiv( programHandle, GL_ACTIVE_UNIFORMS, &nUniforms);

    name = (GLchar *) malloc( maxLen );

	printf("------------------------------------------------\n");
    printf(" Location | Name\n");
    printf("------------------------------------------------\n");
    for( int i = 0; i < nUniforms; ++i ) {
        glGetActiveUniform( programHandle, i, maxLen, &written, &size, &type, name );
        location = glGetUniformLocation(programHandle, name);
        printf(" %-8d | %s\n",location, name);
    }

    free(name);
}

void SceneJulia3D::onKey(unsigned char key, int xmouse, int ymouse)
{
        switch (key)
        {
			case '+': uniforms.rO.z -= 0.1f;
                break;
            case '-': uniforms.rO.z += 0.1f;
                break;
            case 'w': uniforms.rO.y += 0.1f;
                break;
			case 'a': uniforms.rO.x -= 0.1f;
                break;
            case 's': uniforms.rO.y -= 0.1f;
                break;
            case 'd': uniforms.rO.x += 0.1f;
                break;
            default:
                break;
        };
}
