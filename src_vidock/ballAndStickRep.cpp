/*
 * File: ballAndStickRep.cpp
 * Author: Maxime MARIA
 */

#include "ballAndStickRep.hpp"

#include "app.hpp"

namespace vidocklab
{
	BallAndStickRep::BallAndStickRep(const AMolecule *mol) : AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("CylinderGeom");
		m_program->attachShader("cylinderImpostorGeom.vert");
		m_program->attachShader("cylinderImpostorGeom.geom");
		m_program->attachShader("cylinderImpostor.frag");

		m_program->link();

		m_uMVMatrixLoc = glGetUniformLocation(m_program->getId(), "uMVMatrix");
		m_uProjMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrix");
		m_uCylinderRad
			= glGetUniformLocation(m_program->getId(), "uCylinderRad");

		initBuffers();
	}

	BallAndStickRep::~BallAndStickRep()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_posVBO);
		glDeleteBuffers(1, &m_colorVBO);
		glDeleteBuffers(1, &m_ibo);
	}

	void BallAndStickRep::initBuffers()
	{
		// m_nbAtoms = m_refMolecule.m_nbAtoms;
		// m_nbIndices = m_refMolecule.m_bonds.size();

		// std::cout << m_nbIndices << std::endl;
		///// TODO: getPtr() instead of data()
		// glGenBuffers(1, &m_posVBO);
		// glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
		// glBufferData(GL_ARRAY_BUFFER, m_nbAtoms * sizeof(Vec3f),
		//			 m_refMolecule.getPosPtr(), GL_STATIC_DRAW);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		// glGenBuffers(1, &m_colorVBO);
		// glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		// glBufferData(GL_ARRAY_BUFFER, m_nbAtoms * sizeof(Vec3f),
		//			 m_refMolecule.getColorPtr(), GL_STATIC_DRAW);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		//// ibo
		// glGenBuffers(1, &m_ibo);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndices * sizeof(GLuint),
		//			 m_refMolecule.getBondsIdsPtr(), GL_STATIC_DRAW);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//// vao
		// glGenVertexArrays(1, &m_vao);
		// glBindVertexArray(m_vao);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		// const GLuint SPHERE_CENTER = 0;
		// const GLuint SPHERE_COLOR = 1;

		// glBindBuffer(GL_ARRAY_BUFFER, m_posVBO);
		// glEnableVertexAttribArray(SPHERE_CENTER);
		// glVertexAttribPointer(SPHERE_CENTER, 3, GL_FLOAT, GL_FALSE,
		//					  sizeof(Vec3f), 0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		// glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		// glEnableVertexAttribArray(SPHERE_COLOR);
		// glVertexAttribPointer(SPHERE_COLOR, 3, GL_FLOAT, GL_FALSE,
		//					  sizeof(Vec3f), 0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindVertexArray(0);
	}
} // namespace vidocklab
