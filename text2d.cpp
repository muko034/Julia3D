#include "text2d.h"

#include <glm/glm.hpp> 

#define FONT_PATH "font/FreeSans.ttf"
#define VERT_SHADER_PATH "shader/text2d.vert"
#define FRAG_SHADER_PATH "shader/text2d.frag"

using namespace std;

struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

void Text2D::init()
{
	/* Initialize the FreeType2 library */
	if (FT_Init_FreeType(&m_ft)) {
		fprintf(stderr, "Could not init freetype library\n");
		return;
	}

	/* Load a font */
	if (FT_New_Face(m_ft, FONT_PATH, 0, &m_face)) {
		fprintf(stderr, "Could not open font %s\n", FONT_PATH);
		return;
	}

	m_prog.compileAndLinkShaders(VERT_SHADER_PATH, FRAG_SHADER_PATH);

	// Create the vertex buffer object
	glGenBuffers(1, &m_vbo);
}

void Text2D::render(float x, float y, float sx, float sy)
{
	m_prog.use();

	/* Enable blending, necessary for our alpha texture */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Set font size to 48 pixels, color to black */
	FT_Set_Pixel_Sizes(m_face, 0, m_size);
	m_prog.setUniform("color", glm::vec4(1,1,1,1));

	const char *p;
	FT_GlyphSlot g = m_face->glyph;

	/* Create a texture that will be used to hold one "glyph" */
	GLuint tex;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	m_prog.setUniform("tex", 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(/*attribute_coord*/0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0/*attribute_coord*/, 4, GL_FLOAT, GL_FALSE, 0, 0);

	/* Loop through all characters */
	//for (p = text; *p; p++) {
	//	/* Try to load and render the character */
	//	if (FT_Load_Char(m_face, *p, FT_LOAD_RENDER))
	//		continue;
	for (int i=0; i < m_txt.length(); ++i) {
		/* Try to load and render the character */
		if (FT_Load_Char(m_face, m_txt[i], FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{x2, -y2, 0, 0},
			{x2 + w, -y2, 1, 0},
			{x2, -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(/*attribute_coord*/0);
	glDeleteTextures(1, &tex);
}