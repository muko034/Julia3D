#ifndef TEXT2D_H
#define TEXT2D_H

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <list>

#include "glslprogram.h"

class Text2D
{
public:
	Text2D();
	virtual ~Text2D() {}
	void init();
	void setFontSize(const int &size) { m_size = size; }
	int getFontSize() const { return m_size; }
	void setBold(const bool &bold) { m_bold = bold; }
	bool isBold() const { return m_bold; }
	void set(const std::string &txt) { m_txt = txt; }
	std::string get() const { return m_txt; }

	void render(float x, float y, float sx, float sy);
	void clearText() { m_txt.clear(); }
	void addLineText(std::string line);
private:
	void renderLine(const std::string &line, float x, float y, float sx, float sy);
	std::list<std::string> splitText();

	std::string m_txt;
	unsigned m_vbo;
	FT_Library m_ft;
	FT_Face m_face;
	FT_Face m_boldFace;
	GLSLProgram m_prog;
	int m_size;
	bool m_bold;
	glm::vec4 m_color;

};

#endif // TEXT2D_H