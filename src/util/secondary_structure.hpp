#ifndef __VTX_UTIL_SECONDARY_STRUCTURE__
#define __VTX_UTIL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Util
	{
		namespace SecondaryStructure
		{
			static void computeStride( Model::Molecule & p_molecule )
			{
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

					p_molecule.getResidue( idxFirstResidue )
						.setSecondaryStructure( Model::SecondaryStructure::VALUE::COIL );
					p_molecule.getResidue( idxFirstResidue + residueCount - 1 )
						.setSecondaryStructure( Model::SecondaryStructure::VALUE::COIL );

					for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
					{
						const Model::Residue & residue0 = p_molecule.getResidue( idxFirstResidue + residueIdx - 1 );
						Model::Residue &	   residue1 = p_molecule.getResidue( idxFirstResidue + residueIdx );
						const Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

						residue1.setSecondaryStructure( Model::SecondaryStructure::VALUE::COIL );

						const Model::Atom * C0	= residue0.findFirstAtomByName( "C" );
						const Model::Atom * N1	= residue1.findFirstAtomByName( "N" );
						const Model::Atom * CA1 = residue1.findFirstAtomByName( "CA" );
						const Model::Atom * C1	= residue1.findFirstAtomByName( "C" );
						const Model::Atom * N2	= residue2.findFirstAtomByName( "N" );

						if ( C0 == nullptr || N1 == nullptr || CA1 == nullptr || C1 == nullptr || N2 == nullptr )
						{
							// VTX_DEBUG( "Failed to get atoms in chain: " + chain.getName() );
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

					uint firstHelixIdx	= 0u;
					uint RHelixCount	= 0u;
					uint LHelixCount	= 0u;
					uint firstStrandIdx = 0u;
					uint strandCount	= 0u;

					for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
					{
						// Right-handed helix
						if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
													 Vec2f( -PIf / 3.f, -PIf / 4.f ) )
							   < ( PIf / 6.f ) )
							 && ( residueIdx < residueCount - 1 ) )
						{
							if ( RHelixCount == 0 )
							{
								firstHelixIdx = residueIdx;
							}
							RHelixCount++;
						}
						else
						{
							if ( 3 <= RHelixCount )
							{
								for ( uint k = firstHelixIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure(
										Model::SecondaryStructure::VALUE::HELIX_ALPHA_RIGHT );
								}
							}
							RHelixCount = 0;
						}

						// Left-handed helix
						if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
													 Vec2f( PIf / 3.f, PIf / 4.f ) )
							   < ( PIf / 6.f ) )
							 && ( residueIdx < residueCount - 1 ) )
						{
							if ( LHelixCount == 0 )
							{
								firstHelixIdx = residueIdx;
							}
							LHelixCount++;
						}
						else
						{
							if ( 3 <= LHelixCount )
							{
								for ( uint k = firstHelixIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure( Model::SecondaryStructure::VALUE::HELIX_ALPHA_LEFT );
								}
							}
							LHelixCount = 0;
						}

						// Strand
						if ( ( Util::Math::distance(
								   Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
								   Vec2f( -Util::Math::radians( 110.f ), Util::Math::radians( 130.f ) ) )
							   < ( PIf / 6.f ) )
							 && ( residueIdx < residueCount - 1 ) )
						{
							if ( strandCount == 0 )
							{
								firstStrandIdx = residueIdx;
							}
							strandCount++;
						}
						else
						{
							if ( 2 <= strandCount )
							{
								for ( uint k = firstStrandIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure( Model::SecondaryStructure::VALUE::STRAND );
								}
							}
							strandCount = 0;
						}
					}

					for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
					{
						Model::Residue & residue0 = p_molecule.getResidue( idxFirstResidue + residueIdx - 1 );
						Model::Residue & residue  = p_molecule.getResidue( idxFirstResidue + residueIdx );
						Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

						if ( ( residue0.getSecondaryStructure() == residue2.getSecondaryStructure() )
							 && ( ( residue0.getSecondaryStructure()
									== Model::SecondaryStructure::VALUE::HELIX_ALPHA_RIGHT )
								  || ( residue0.getSecondaryStructure()
									   == Model::SecondaryStructure::VALUE::HELIX_ALPHA_LEFT )
								  || ( residue0.getSecondaryStructure()
									   == Model::SecondaryStructure::VALUE::STRAND ) ) )

						{
							residue.setSecondaryStructure( residue0.getSecondaryStructure() );
						}
					}
				}
			}

			static void computeSecondaryStructure( Model::Molecule & p_molecule )
			{
				Tool::Chrono chrono;
				chrono.start();
				VTX_INFO( "Computing secondary structure..." );

				computeStride( p_molecule );

				p_molecule.getConfiguration().isSecondaryStructureLoadedFromFile = false;
				chrono.stop();
				VTX_INFO( "Secondary structure computed in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

		} // namespace SecondaryStructure
	}	  // namespace Util
} // namespace VTX

#endif
