#include "text2d.h"
#include <sstream>

#define FONT_PATH "font/CourierNew.ttf"
#define BOLD_FONT_PATH "font/CourierNewBold.ttf"
#define VERT_SHADER_PATH "shader/text2d.vert"
#define FRAG_SHADER_PATH "shader/text2d.frag"

using namespace std;

struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
};

void Tokenize(const string& str,
              list<string>& tokens,
              const string& delimiters = "\n")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

Text2D::Text2D()
	: m_size(12),
	  m_bold(false),
	  m_color(1.0, 1.0, 1.0, 1.0)
{
}

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

	/* Load a font */
	if (FT_New_Face(m_ft, BOLD_FONT_PATH, 0, &m_boldFace)) {
		fprintf(stderr, "Could not open font %s\n", BOLD_FONT_PATH);
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
	m_prog.setUniform("color", m_color);

	//const char *p;
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

	list<string> lines = splitText();
	int i = 0; 
	for (string &line : lines) {
		renderLine(line, x, y-m_size*i*sy, sx, sy);
		++i;
	}
	
	glDisableVertexAttribArray(/*attribute_coord*/0);
	glDeleteTextures(1, &tex);
}

void Text2D::renderLine(const std::string &line, float x, float y, float sx, float sy)
{
	FT_GlyphSlot g = m_face->glyph;

	for (int i=0; i < line.length(); ++i) {
		/* Try to load and render the character */
		if (FT_Load_Char(m_face, line[i], FT_LOAD_RENDER))
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
}

void Text2D::addLineText(std::string line)
{
	m_txt.append(line);
	m_txt.append("\n");
}

std::list<std::string> Text2D::splitText()
{
    list<string> tokens;

    Tokenize(m_txt, tokens);

	return tokens;
}