/*
 * File: vdwSphereRepPerspective.hpp
 * Author: Maxime MARIA
 */

#ifndef __VDW_SPHERE_SSBO_REP_HPP__
#define __VDW_SPHERE_SSBO_REP_HPP__

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSLProgramManager.hpp"
#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class VdwSphereSSBORep : public AMoleculeRep
	{
	public:
		VdwSphereSSBORep(const AMolecule *mol);
		~VdwSphereSSBORep();

		void setCameraMatrices(const FreeflyCamera &cam) const
		{
			glUniformMatrix4fv(m_uMVMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getViewMatrix()));
			glUniformMatrix4fv(m_uProjMatrixLoc, 1, GL_FALSE,
							   glm::value_ptr(cam.getProjectionMatrix()));
		}

		// TODO does not work anymore, vao is bound in AMolecule
		void bindBuffers() const
		{
			glBindVertexArray(m_vao);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		}

		void unbindBuffers() const
		{
			glBindVertexArray(0);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void draw(const uint first, const uint count) const
		{
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT,
						   (void *)(first * sizeof(uint)));
		}

	private:
		void initBuffers();

	private:
		GLint m_uMVMatrixLoc = GL_INVALID_INDEX;
		GLint m_uProjMatrixLoc = GL_INVALID_INDEX;

		/// TODO remove
		GLuint m_ssbo = GL_INVALID_VALUE;
		GLuint m_ibo = GL_INVALID_VALUE;
		GLuint m_vao = GL_INVALID_VALUE;
	};
} // namespace vidocklab

#endif // __VDW_SPHERE_SSBO_REP_HPP__
