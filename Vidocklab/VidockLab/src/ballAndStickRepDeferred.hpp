/*
 * File: ballAndStickRepDeferred.hpp
 * Author: Maxime MARIA
 */

#ifndef __BALL_AND_STICK_REP_DEFERRED_HPP__
#define __BALL_AND_STICK_REP_DEFERRED_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"

#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class BallAndStickRepDeferred : public AMoleculeRep
	{
	public:
		BallAndStickRepDeferred(const AMolecule *mol);
		~BallAndStickRepDeferred() = default;

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(m_uMVMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getViewMatrix()));

			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()));

			glUniform1f(m_uCylRad, 0.2f);
		}

		void draw(const uint first, const uint count) const
		{
			glDrawElements(GL_LINES, count, GL_UNSIGNED_INT,
						   (void *)(first * sizeof(uint)));
		}

	private:
		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;
		GLint m_uCylRad = GL_INVALID_INDEX;
	};
} // namespace vidocklab

#endif // __BALL_AND_STICK_REP_DEFERRED_HPP__
