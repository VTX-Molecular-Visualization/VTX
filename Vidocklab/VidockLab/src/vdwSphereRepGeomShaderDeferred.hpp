/*
 * File: vdwSphereRepGeomShaderDeferred.hpp
 * Author: Maxime MARIA
 */

#ifndef __VDW_SPHERE_REP_GEOM_SHADER_DEFERRED_HPP__
#define __VDW_SPHERE_REP_GEOM_SHADER_DEFERRED_HPP__

#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"
#include "aMoleculeRep.hpp"
#include "molecules/aMolecule.hpp"

namespace vidocklab
{
	class VdwSphereRepGeomShaderDeferred : public AMoleculeRep
	{
	public:
		VdwSphereRepGeomShaderDeferred(const AMolecule *mol);
		~VdwSphereRepGeomShaderDeferred() = default;

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(
				m_uMVMatrixLoc, 1, GL_FALSE,
				glm::value_ptr(cam.getViewMatrix()
							   * m_refMolecule->m_modelMatrix.getModel()));

			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()));
		}

		void draw(const uint first, const uint count) const
		{
			glDrawArrays(GL_POINTS, first, count);
		}

		void setRadScale(const float radScale) const
		{
			glUniform1f(m_uRadScale, radScale);
		}

	private:
		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;
		GLint m_uRadScale = GL_INVALID_INDEX;
	};
} // namespace vidocklab

#endif // __VDW_SPHERE_REP_GEOM_SHADER_DEFERRED_HPP__
