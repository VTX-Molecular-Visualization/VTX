#include "molecule_ball_and_stick.hpp"
#include "../materials/diffuse_material.hpp"
#include "cylinder.hpp"
#include "sphere.hpp"

namespace VTX
{
	namespace Renderer
	{
		MoleculeBallAndStick::MoleculeBallAndStick( Model::Molecule * p_molecule )
		{
			const uint nbAtoms = p_molecule->getAtomCount();
			const uint nbBonds = p_molecule->getBondCount() / 2;

			std::vector<Renderer::BasePrimitive *> primitives;
			primitives.resize( nbAtoms + nbBonds );

			const std::vector<Vec3f> & positions = p_molecule->getAtomPositionFrame( 0 );

			uint idPrimitive = 0;
			uint cptAtoms	 = 0;
			uint cptBonds	 = 0;

			_materials.emplace_back( new Renderer::DiffuseMaterial( VEC3F_XYZ ) );

			// Add atoms and bonds of each residue
			for ( uint i = 0; i < p_molecule->getResidueCount(); ++i )
			{
				// TODO: remove material duplication + only allow chain colors
				const Model::Residue & r = p_molecule->getResidue( i );
				_materials.emplace_back( new DiffuseMaterial( r.getChainPtr()->getColor() ) );

				const uint idFirstAtomRes = r.getIdFirstAtom();
				const uint nbAtomsRes	  = r.getAtomCount();
				const uint idFirstBondRes = r.getIdFirstBond();
				const uint nbBondsRes	  = r.getBondCount();

				for ( uint j = idFirstAtomRes; j < idFirstAtomRes + nbAtomsRes; ++j )
				{
					primitives[ idPrimitive ] = new Renderer::Sphere(
						positions[ j ], p_molecule->getAtomRadius( j ) * 0.3f, _materials.back() );
					idPrimitive++;
					cptAtoms++;
				}
				for ( uint j = idFirstBondRes; j < idFirstBondRes + nbBondsRes * 2; j += 2 )
				{
					const Vec3f & a1 = positions[ p_molecule->getBond( j ) ];
					const Vec3f & a2 = positions[ p_molecule->getBond( j + 1 ) ];

					primitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _materials.front() );
					idPrimitive++;
					cptBonds++;
				}
			}

			// Add bonds out of residues
			for ( ; cptBonds < nbBonds; cptBonds++ )
			{
				const Vec3f & a1 = positions[ p_molecule->getBond( cptBonds * 2 ) ];
				const Vec3f & a2 = positions[ p_molecule->getBond( cptBonds * 2 + 1 ) ];

				primitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _materials.front() );
				idPrimitive++;
			}

			// TODO: add options for splitMethod and maxPrimsLeaf
			const uint maxPrimsLeaf = 8;
			_bvh.build( primitives, maxPrimsLeaf, BVH::SplitMethod::SAH );
		}
	} // namespace Renderer
} // namespace VTX
