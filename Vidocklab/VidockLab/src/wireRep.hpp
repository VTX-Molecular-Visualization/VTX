/*
 * File: wireRep.hpp
 * Author: Maxime MARIA
 */

#ifndef __WIRE_REP_HPP__
#define __WIRE_REP_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"

#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class WireRep : public AMoleculeRep
	{
	public:
		WireRep(const AMolecule *mol);
		~WireRep();

		void use() const
		{
			m_program->use();
		}

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(m_uMVPMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()
											  * cam.getViewMatrix()));
		}

		void bindBuffers() const
		{
			glBindVertexArray(m_vao);
		}

		void unbindBuffers() const
		{
			glBindVertexArray(0);
		}

		void draw() const
		{
			glDrawElements(GL_LINES, m_nbIndices, GL_UNSIGNED_INT, (void *)0);
		}

	private:
		void initBuffers();

	private:
		GLint m_nbIndices = 0;

		GLint m_uMVPMatrixLoc = GL_INVALID_INDEX;

		GLuint m_posVBO = GL_INVALID_VALUE;
		GLuint m_colorVBO = GL_INVALID_VALUE;
		GLuint m_vao = GL_INVALID_VALUE;
		GLuint m_ibo = GL_INVALID_VALUE;

		GLfloat m_lineWidth = 1.f;
	};
} // namespace vidocklab

#endif // __WIRE_REP_HPP__
