/*
 * File: vdwSpherePointSpritesRep.hpp
 * Author: Maxime MARIA
 */

#ifndef __VDW_SPHERE_POINT_SPRITES_REP_HPP__
#define __VDW_SPHERE_POINT_SPRITES_REP_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"
#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class VdwSpherePointSpritesRep : public AMoleculeRep
	{
	public:
		VdwSpherePointSpritesRep(const AMolecule *mol);
		~VdwSpherePointSpritesRep();

		void use() const
		{
			m_program->use();
		}

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			const glm::mat4 mv = cam.getViewMatrix();
			const glm::mat4 &p = cam.getProjectionMatrix();
			const glm::mat4 mvp = p * mv;
			glUniformMatrix4fv(m_uMVMatrixLoc, 1, GL_FALSE, glm::value_ptr(mv));
			glUniformMatrix4fv(m_uMVMatrixInverseLoc, 1, GL_FALSE,
							   glm::value_ptr(glm::inverse(mv)));
			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(p));
			glUniformMatrix4fv(m_uProjMatrixTransposeLoc, 1, GL_FALSE,
							   glm::value_ptr(glm::transpose(p)));
			glUniformMatrix4fv(m_uMVPMatrixInverseLoc, 1, GL_FALSE,
							   glm::value_ptr(glm::inverse(mvp)));
			glUniformMatrix4fv(m_uMVPMatrixTransposeLoc, 1, GL_FALSE,
							   glm::value_ptr(glm::transpose(mvp)));
		}

		void draw(const uint first, const uint count) const
		{
			glDrawArrays(GL_POINTS, first, count);
		}

	private:
		void initBuffers();

	private: // TODO
		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uMVMatrixInverseLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixTransposeLoc = GL_INVALID_INDEX;
		GLint m_uMVPMatrixInverseLoc = GL_INVALID_INDEX;
		GLint m_uMVPMatrixTransposeLoc = GL_INVALID_INDEX;

		GLuint m_posVBO = GL_INVALID_VALUE;
		GLuint m_radiiVBO = GL_INVALID_VALUE;
		GLuint m_colorVBO = GL_INVALID_VALUE;
		GLuint m_vao = GL_INVALID_VALUE;
	};
} // namespace vidocklab

#endif // __VDW_SPHERE_POINT_SPRITES_REP_HPP__
