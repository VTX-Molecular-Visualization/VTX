/*
 * File: vdwSphereRepGeomShader.hpp
 * Author: Maxime MARIA
 */

#ifndef __VDW_SPHERE_REP_GEOM_SHADER_HPP__
#define __VDW_SPHERE_REP_GEOM_SHADER_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"
#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class VdwSphereRepGeomShader : public AMoleculeRep
	{
	public:
		VdwSphereRepGeomShader(const AMolecule *mol);
		~VdwSphereRepGeomShader();

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(m_uMVMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getViewMatrix()));
			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()));
		}

		void draw(const uint first, const uint count) const
		{
			glDrawArrays(GL_POINTS, first, count);
		}

	private:
		void initBuffers();

	private:
		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;

		GLuint m_posVBO = GL_INVALID_VALUE;
		GLuint m_radiiVBO = GL_INVALID_VALUE;
		GLuint m_colorVBO = GL_INVALID_VALUE;
		GLuint m_vao = GL_INVALID_VALUE;
	};
} // namespace vidocklab

#endif // __VDW_SPHERE_REP_GEOM_SHADER_HPP__
