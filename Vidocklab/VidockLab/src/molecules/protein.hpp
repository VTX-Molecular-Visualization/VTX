/*
 * File: molecule.hpp
 * Author : Maxime MARIA
 */

#ifndef __MOLECULE_HPP__
#define __MOLECULE_HPP__

#include <vector>

#include "math/aabb.hpp"

#include "defines.hpp"

#include "atom.hpp"
#include "chain.hpp"
#include "moleculeTrajectory.hpp"
#include "residue.hpp"

namespace vidocklab
{
	struct Molecule
	{
		void extendAABB(const Atom &atom)
		{
			m_aabb.extend(m_atomPositions[atom.m_id]);
		}

		const AABB &getAABB() const
		{
			return m_aabb;
		}

		void computeCentroid()
		{
			m_centroid = m_aabb.center();
		}

		const Vec3f *getPosPtr() const
		{
			return m_atomPositions.data();
		}

		const float *getRadPtr() const
		{
			return m_atomRadii.data();
		}

		const Vec3f *getColorPtr() const
		{
			return m_atomColors.data();
		}

		const uint *getBondsIdsPtr() const
		{
			return m_bonds.data();
		}

		std::string m_name = UNDEFINED_STRING;

		std::vector<Vec3f> m_atomPositions;
		std::vector<float> m_atomRadii;
		std::vector<Vec3f> m_atomColors;

		std::vector<uint> m_bonds; // two uints per bond

		std::vector<Chain> m_chains;
		std::vector<Residue> m_residues;
		std::vector<Atom> m_atoms;

		uint m_nbChains = 0;
		uint m_nbResidues = 0;
		uint m_nbAtoms = 0;
		uint m_nbBonds = 0;

		AABB m_aabb;
		Vec3f m_centroid = VEC3F_ZERO;
	};
} // namespace vidocklab

#endif // __PROTEIN_HPP__
