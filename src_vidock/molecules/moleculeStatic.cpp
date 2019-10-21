/*
 * File: moleculeStatic.cpp
 * Author : Maxime MARIA
 */

#include "moleculeStatic.hpp"

namespace vidocklab
{
	void MoleculeStatic::shrinkVectorsToFit()
	{
		AMolecule::shrinkVectorsToFit();
		m_atomPositions.shrink_to_fit();
		m_bonds.shrink_to_fit();
	}

	// TODO move what you can in AMolecule
	void MoleculeStatic::initGLBuffers()
	{
		glGenBuffers(1, &m_atomPosVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_atomPosVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * m_nbAtoms,
					 m_atomPositions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * m_nbAtoms,
					 m_atomColors.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_radiiVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_radiiVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_nbAtoms,
					 m_atomRadii.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ibo
		glGenBuffers(1, &m_bondsIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bondsIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_bonds.size(),
					 m_bonds.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// vao
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bondsIBO);

		glBindBuffer(GL_ARRAY_BUFFER, m_atomPosVBO);
		glEnableVertexAttribArray(ATOM_POSITION);
		glVertexAttribPointer(ATOM_POSITION, 3, GL_FLOAT, GL_FALSE,
							  sizeof(Vec3f), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
		glEnableVertexAttribArray(ATOM_COLOR);
		glVertexAttribPointer(ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f),
							  0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_radiiVBO);
		glEnableVertexAttribArray(ATOM_RADIUS);
		glVertexAttribPointer(ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof(float),
							  0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
	}

	void MoleculeStatic::updateFrame(const uint idFrame)
	{
		// nothing to do
	}

} // namespace vidocklab
