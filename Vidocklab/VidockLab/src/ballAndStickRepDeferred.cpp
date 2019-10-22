/*
 * File: ballAndStickRepDeferred.cpp
 * Author: Maxime MARIA
 */

#include "ballAndStickRepDeferred.hpp"

#include "app.hpp"

namespace vidocklab
{
	BallAndStickRepDeferred::BallAndStickRepDeferred(const AMolecule *mol)
		: AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("CylinderGeom");
		m_program->attachShader("cylinderImpostorGeom.vert");
		m_program->attachShader("cylinderImpostorGeom.geom");
		m_program->attachShader("cylinderImpostorDeferred.frag");

		m_program->link();

		m_uMVMatrixLoc = glGetUniformLocation(m_program->getId(), "uMVMatrix");
		m_uProjMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrix");
		m_uCylRad = glGetUniformLocation(m_program->getId(), "uCylRad");
	}
} // namespace vidocklab
