#ifndef TEXT2D_H
#define TEXT2D_H

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "glslprogram.h"

class Text2D
{
public:
	Text2D() {}
	virtual ~Text2D() {}
	void init();
	void setSize(const int &size) { m_size = size; }
	int getSize() const { return m_size; }
	void set(const std::string &txt) { m_txt = txt; }
	std::string get() const { return m_txt; }
	void render(float x, float y, float sx, float sy);
private:
	std::string m_txt;
	unsigned m_vbo;
	FT_Library m_ft;
	FT_Face m_face;
	GLSLProgram m_prog;
	int m_size;

};

#endif // TEXT2D_H