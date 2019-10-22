/*
 * File: vdwSphereRepGeomShader.cpp
 * Author: Maxime MARIA
 */

#include "vdwSphereRepGeomShaderDeferred.hpp"

#include "app.hpp"

namespace vidocklab
{
	VdwSphereRepGeomShaderDeferred::VdwSphereRepGeomShaderDeferred(
		const AMolecule *mol)
		: AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("SphereImpostorGeomShader");
		m_program->attachShader("sphereImpostorGeom.vert");
		m_program->attachShader("sphereImpostorGeomQuad.geom");
		m_program->attachShader("sphereImpostorDeferred.frag");

		m_program->link();

		m_uMVMatrixLoc = glGetUniformLocation(m_program->getId(), "uMVMatrix");
		m_uProjMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrix");
		m_uRadScale = glGetUniformLocation(m_program->getId(), "uRadScale");
	}
} // namespace vidocklab
