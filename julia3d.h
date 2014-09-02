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
	void eyeUp() { m_eye.y += 0.1f; }
	void eyeDown() { m_eye.y -= 0.1f; }
	void eyeLeft() { m_eye.x -= 0.1f; }
	void eyeRight() { m_eye.x += 0.1f; }
	void zoomIn() { m_eye.z += 0.1f; }
	void zoomOut() { m_eye.z -= 0.1f; }
	void incQx(float step) { m_q.x += step; }
	void decQx(float step) { m_q.x -= step; }
	void incQy(float step) { m_q.y += step; }
	void decQy(float step) { m_q.y -= step; }
	void incQz(float step) { m_q.z += step; }
	void decQz(float step) { m_q.z -= step; }
	void incQw(float step) { m_q.w += step; }
	void decQw(float step) { m_q.w -= step; }
	void incSlice(float step) { m_slice += step; }
	void decSlice(float step) { m_slice -= step; }
	void incMaxIterations() { ++m_maxIterations; }
	void decMaxIterations() { --m_maxIterations; }
private:
	unsigned m_vbo;
	GLSLProgram m_prog;
	glm::vec3 m_eye;
	glm::vec4 m_q;
	float m_slice;
	float m_step;
	int m_maxIterations;
};

#endif // JULIA3D_H