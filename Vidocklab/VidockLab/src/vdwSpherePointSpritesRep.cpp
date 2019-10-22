/*
 * File: vdwSpherePointSpritesRep.cpp
 * Author: Maxime MARIA
 */

#include "vdwSpherePointSpritesRep.hpp"

#include "app.hpp"

namespace vidocklab
{
	VdwSpherePointSpritesRep::VdwSpherePointSpritesRep(const AMolecule *mol)
		: AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("SpherePointSprites");
		m_program->attachShader("spherePointSpritesPerspective.vert");
		m_program->attachShader("spherePointSpritesPerspective.frag");

		m_program->link();

		m_uMVMatrixLoc = glGetUniformLocation(m_program->getId(), "uMVMatrix");
		m_uMVMatrixInverseLoc
			= glGetUniformLocation(m_program->getId(), "uMVMatrixInverse");
		m_uProjMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrix");
		m_uProjMatrixTransposeLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrixTranspose");
		m_uMVPMatrixInverseLoc
			= glGetUniformLocation(m_program->getId(), "uMVPMatrixInverse");
		m_uMVPMatrixTransposeLoc
			= glGetUniformLocation(m_program->getId(), "uMVPMatrixTranspose");
		initBuffers();
	}

	VdwSpherePointSpritesRep::~VdwSpherePointSpritesRep()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_posVBO);
		glDeleteBuffers(1, &m_radiiVBO);
		glDeleteBuffers(1, &m_colorVBO);
	}

	void VdwSpherePointSpritesRep::initBuffers()
	{
		// m_nbAtoms = m_refMolecule.m_nbAtoms;

		// std::cout << m_nbAtoms << std::endl;
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

		// glGenBuffers(1, &m_radiiVBO);
		// glBindBuffer(GL_ARRAY_BUFFER, m_radiiVBO);
		// glBufferData(GL_ARRAY_BUFFER, m_nbAtoms * sizeof(float),
		//			 m_refMolecule.getRadPtr(), GL_STATIC_DRAW);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		//// vao
		// glGenVertexArrays(1, &m_vao);
		// glBindVertexArray(m_vao);
		// const GLuint SPHERE_CENTER = 0;
		// const GLuint SPHERE_COLOR = 1;
		// const GLuint SPHERE_RADIUS = 2;

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

		// glBindBuffer(GL_ARRAY_BUFFER, m_radiiVBO);
		// glEnableVertexAttribArray(SPHERE_RADIUS);
		// glVertexAttribPointer(SPHERE_RADIUS, 1, GL_FLOAT, GL_FALSE,
		//					  sizeof(float), 0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		// glBindVertexArray(0);
	}
} // namespace vidocklab
