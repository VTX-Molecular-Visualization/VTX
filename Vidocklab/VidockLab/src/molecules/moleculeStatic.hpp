/*
 * File: moleculeStatic.hpp
 * Author : Maxime MARIA
 */

#ifndef __MOLECULE_STATIC_HPP__
#define __MOLECULE_STATIC_HPP__

#include <vector>

#include "aMolecule.hpp"

#include "atom.hpp"
#include "chain.hpp"
#include "defines.hpp"
#include "math/aabb.hpp"
#include "residue.hpp"

namespace vidocklab
{
	struct MoleculeStatic : public AMolecule
	{
		void shrinkVectorsToFit();
		void initGLBuffers();
		// TODO bof non ?
		void updateFrame(const uint idFrame);

		// ============================ attributes
		std::vector<Vec3f> m_atomPositions;
		std::vector<uint> m_bonds; // two uints per bond
		AABB m_aabb;
	};
} // namespace vidocklab

#endif // __MOLECULE_STATIC_HPP__
