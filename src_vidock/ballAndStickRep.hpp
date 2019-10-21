/*
 * File: ballAndStickRep.hpp
 * Author: Maxime MARIA
 */

#ifndef __BALL_AND_STICK_REP_HPP__
#define __BALL_AND_STICK_REP_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"

#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class BallAndStickRep : public AMoleculeRep
	{
	public:
		BallAndStickRep(const AMolecule *mol);
		~BallAndStickRep();

		void use() const
		{
			m_program->use();
		}

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(m_uMVMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getViewMatrix()));
			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()));

			glUniform1f(m_uCylinderRad, 0.2f);
		}

		void bindBuffers() const
		{
			glBindVertexArray(m_vao);
		}

		void unbindBuffers() const
		{
			glBindVertexArray(0);
		}

		void draw(const uint first, const uint count) const
		{
			glDrawElements(GL_LINES, count, GL_UNSIGNED_INT,
						   (void *)(sizeof(uint) * first));
		}

	private:
		void initBuffers();

	private:
		GLint m_nbIndices = 0;

		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;
		GLint m_uCylinderRad = GL_INVALID_INDEX;

		GLuint m_posVBO = GL_INVALID_VALUE;
		GLuint m_colorVBO = GL_INVALID_VALUE;
		GLuint m_vao = GL_INVALID_VALUE;
		GLuint m_ibo = GL_INVALID_VALUE;
	};
} // namespace vidocklab

#endif // __BALL_AND_STICK_REP_HPP__
