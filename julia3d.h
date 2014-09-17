#ifndef JULIA3D_H
#define JULIA3D_H

#include <glm/glm.hpp>
#include "glslprogram.h"

#include <iostream>

class Julia3D
{
public:
	Julia3D();
	virtual ~Julia3D();
	void init();
	void render();
	glm::vec4 getQ() const { return m_q; }
	float getSlice() const { return m_slice; }
	int getMaxIterations() const { return m_maxIterations; }
	float getStep() const { return m_step; }
	void setShadows(const bool &shadows) { m_shadows = shadows; }
	bool getShadows() const { return m_shadows; }
	void eyeUp() { m_eye.y += 0.05f; }
	void eyeDown() { m_eye.y -= 0.05f; }
	void eyeLeft() { m_eye.x -= 0.05f; }
	void eyeRight() { m_eye.x += 0.05f; }
	void zoomIn() { m_eye.z += 0.05f; }
	void zoomOut() { m_eye.z -= 0.05f; }
	void incQx() { m_q.x += m_step; }
	void decQx() { m_q.x -= m_step; }
	void incQy() { m_q.y += m_step; }
	void decQy() { m_q.y -= m_step; }
	void incQz() { m_q.z += m_step; }
	void decQz() { m_q.z -= m_step; }
	void incQw() { m_q.w += m_step; }
	void decQw() { m_q.w -= m_step; }
	void incSlice() { m_slice += m_step; }
	void decSlice() { m_slice -= m_step; }
	void incMaxIterations() { ++m_maxIterations; }
	void decMaxIterations() { --m_maxIterations; }
	void incStep() { m_step *= 10; }
	void decStep() { m_step /= 10; }
	void incAlpha(float f) { m_alpha += f; }
	void decAlpha(float f) { m_alpha -= f; }
	void incBeta(float f) { m_beta += f; }
	void decbeta(float f) { m_beta -= f; }
private:
	unsigned m_vbo;
	GLSLProgram m_prog;
	glm::vec3 m_eye;
	glm::vec4 m_q;
	float m_slice;
	float m_step;
	int m_maxIterations;
	float m_alpha;	// kat obrotu wokol osi OX
	float m_beta;	// kat obrotu wokol osi OY
	bool m_shadows;
};

#endif // JULIA3D_H