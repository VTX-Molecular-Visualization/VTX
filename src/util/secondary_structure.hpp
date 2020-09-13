#ifndef __VTX_UTIL_SECONDARY_STRUCTURE__
#define __VTX_UTIL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "tool/chrono.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Util
	{
		namespace SecondaryStructure
		{
			static void computeStride( Model::Molecule & p_molecule )
			{
				return;
				const Model::Molecule::AtomPositionsFrame & positions
					= p_molecule.getAtomPositionFrame( p_molecule.getFrame() );

				for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
				{
					const Model::Chain & chain		  = p_molecule.getChain( chainIdx );
					uint				 residueCount = chain.getResidueCount();

					// Not enought residues.
					if ( residueCount < 4 )
					{
						VTX_DEBUG( "Chain residue count < 4" );
						continue;
					}

					uint			   idxFirstResidue = chain.getIndexFirstResidue();
					std::vector<float> phi			   = std::vector<float>( residueCount );
					std::vector<float> psi			   = std::vector<float>( residueCount );
					// std::vector<float> omega		   = std::vector<float>( residueCount );

					// Compute dihedral angles psi and phi.

					// Terminal values.
					phi[ 0 ]				= PI_2f;
					psi[ 0 ]				= PI_2f;
					phi[ residueCount - 1 ] = PI_2f;
					psi[ residueCount - 1 ] = PI_2f;

					for ( uint residueIdx = 1; residueIdx < -1; ++residueIdx )
					{
						const Model::Residue & residue0 = p_molecule.getResidue( idxFirstResidue + residueIdx - 1 );
						const Model::Residue & residue1 = p_molecule.getResidue( idxFirstResidue + residueIdx );
						const Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

						const Model::Atom * C0	= residue0.findFirstAtomByName( "C" );
						const Model::Atom * N1	= residue1.findFirstAtomByName( "N" );
						const Model::Atom * CA1 = residue1.findFirstAtomByName( "CA" );
						const Model::Atom * C1	= residue1.findFirstAtomByName( "C" );
						const Model::Atom * N2	= residue2.findFirstAtomByName( "N" );

						if ( C0 == nullptr || N1 == nullptr || CA1 == nullptr || C1 == nullptr || N2 == nullptr )
						{
							VTX_DEBUG( "Failed to get atoms in chain: " + chain.getName() );
							continue;
						}

						const Vec3f & positionC0  = positions[ C0->getIndex() ];
						const Vec3f & positionN1  = positions[ N1->getIndex() ];
						const Vec3f & positionCA1 = positions[ CA1->getIndex() ];
						const Vec3f & positionC1  = positions[ C1->getIndex() ];
						const Vec3f & positionN2  = positions[ N2->getIndex() ];

						phi[ residueIdx ]
							= Util::Math::torsionalAngle( positionC0, positionN1, positionCA1, positionC1 );
						psi[ residueIdx ]
							= Util::Math::torsionalAngle( positionN1, positionCA1, positionC1, positionN2 );
					}
				}
			}

			static void computeSecondaryStructure( Model::Molecule & p_molecule )
			{
				Tool::Chrono chrono;
				chrono.start();
				VTX_DEBUG( "Computing secondary structure..." );

				switch ( p_molecule.getSecondaryStructureAlgo() )
				{
				case Model::SecondaryStructure::ALGO ::STRIDE: computeStride( p_molecule ); break;
				default: VTX_WARNING( "Secondary structure algorithm not implemented yet" ); break;
				}

				chrono.stop();
				VTX_DEBUG( "Secondary structure computed in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

		} // namespace SecondaryStructure
	}	  // namespace Util
} // namespace VTX

#endif
