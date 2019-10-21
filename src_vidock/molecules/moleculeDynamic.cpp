/*
 * File: moleculeDynamic.cpp
 * Author : Maxime MARIA
 */

#include "moleculeDynamic.hpp"

#include "io/ioUtils.hpp" // TODO remove

namespace vidocklab
{
	void MoleculeDynamic::shrinkVectorsToFit()
	{
		AMolecule::shrinkVectorsToFit();
		m_frames.shrink_to_fit();
		for (uint i = 0; i < m_frames.size(); ++i)
		{
			m_frames[i].m_atomPositions.shrink_to_fit();
			m_frames[i].m_bonds.shrink_to_fit();
		}
	}

	// TODO move what you can in AMolecule
	void MoleculeDynamic::initGLBuffers()
	{
		// init with first frame
		const Frame &f = m_frames[0];
		m_nbAtoms = uint(f.m_atomPositions.size());
		m_nbBonds = uint(f.m_bonds.size());

		glGenBuffers(1, &m_atomPosVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_atomPosVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * m_nbAtoms,
					 f.m_atomPositions.data(), GL_DYNAMIC_DRAW);
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_nbBonds,
					 f.m_bonds.data(), GL_DYNAMIC_DRAW);
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

	void MoleculeDynamic::updateFrame(const uint idFrame)
	{
		// TODO separate positions and bonds update ?
		std::cout << "Current frame: " << idFrame << std::endl;
		m_nbBonds = uint(m_frames[idFrame].m_bonds.size());
		m_nbBondsNotWater = m_frames[idFrame].m_nbBondsNotWater;
		m_centroid = m_frames[idFrame].m_centroid;
		glBindBuffer(GL_ARRAY_BUFFER, m_atomPosVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * m_nbAtoms,
					 m_frames[idFrame].m_atomPositions.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bondsIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m_nbBonds,
					 m_frames[idFrame].m_bonds.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

} // namespace vidocklab
