/*
 * File: moleculeDynamic.hpp
 * Author : Maxime MARIA
 */

#ifndef __MOLECULE_DYNAMIC_HPP__
#define __MOLECULE_DYNAMIC_HPP__

#include <vector>

#include "aMolecule.hpp"

#include "atom.hpp"
#include "chain.hpp"
#include "defines.hpp"
#include "math/aabb.hpp"
#include "residue.hpp"

namespace vidocklab
{
	struct MoleculeDynamic : public AMolecule
	{
		struct Frame
		{
			std::vector<Vec3f> m_atomPositions;
			std::vector<uint> m_bonds; // two uints per bond
			uint m_nbBondsNotWater = INVALID_ID;
			AABB m_aabb;
			Vec3f m_centroid = VEC3F_ZERO;
		};

		void shrinkVectorsToFit();
		void initGLBuffers();
		void updateFrame(const uint idFrame);

		std::vector<Frame> m_frames;

		// TODO: je pars du principe que les liens peuvent changer à chaque
		// frame
		// m_nbBonds (not stable) is updated when changing frame
	};
} // namespace vidocklab

#endif // __MOLECULE_DYNAMIC_HPP__
