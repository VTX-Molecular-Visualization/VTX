#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "tool/chrono.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Molecule
		{
			static void computeSecondaryStructure( Model::Molecule & p_molecule )
			{
				Tool::Chrono chrono;
				chrono.start();
				VTX_DEBUG( "Computing secondary structure..." );

				const Model::Molecule::AtomPositionsFrame & positions
					= p_molecule.getAtomPositionFrame( p_molecule.getFrame() );
				p_molecule.setSecondaryStructureLoadedFromFile( false );

				for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
				{
					const Model::Chain & chain			 = p_molecule.getChain( chainIdx );
					uint				 residueCount	 = chain.getResidueCount();
					uint				 idxFirstResidue = chain.getIdFirstResidue();
					std::vector<float>	 phi			 = std::vector<float>( residueCount );
					std::vector<float>	 psi			 = std::vector<float>( residueCount );

					for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
					{
						if ( residueIdx == 0 || residueIdx == residueCount - 1 )
						{
							phi[ residueIdx ] = PI_2f;
							psi[ residueIdx ] = PI_2f;
						}
						else
						{
							const Model::Residue & residue0 = p_molecule.getResidue( idxFirstResidue + residueIdx - 1 );
							const Model::Residue & residue	= p_molecule.getResidue( idxFirstResidue + residueIdx );
							const Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

							const Model::Atom * C0	= residue0.findFirstAtomByName( "C" );
							const Model::Atom * N1	= residue.findFirstAtomByName( "N" );
							const Model::Atom * CA1 = residue.findFirstAtomByName( "CA" );
							const Model::Atom * C1	= residue.findFirstAtomByName( "C" );
							const Model::Atom * N2	= residue2.findFirstAtomByName( "N" );

							if ( C0 == nullptr || N1 == nullptr || CA1 == nullptr || C1 == nullptr || N2 == nullptr )
							{
								// VTX_DEBUG( "Failed to get atoms" );
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

					uint firstHelixIdx	= 0u;
					uint RHelixCount	= 0u;
					uint LHelixCount	= 0u;
					uint firstStrandIdx = 0u;
					uint strandCount	= 0u;

					for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
					{
						// VTX_DEBUG( std::to_string( phi[ residueIdx ] ) + " / " + std::to_string( psi[ residueIdx ] )
						// );

						// Right-handed helix
						if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
													 Vec2f( -PIf / 3.f, -PIf / 4.f ) )
							   < ( PIf / 6.f ) )
							 && ( residueIdx < residueCount - 1 ) )
						{
							if ( RHelixCount == 0 ) { firstHelixIdx = residueIdx; }
							RHelixCount++;
						}
						else
						{
							if ( 3 <= RHelixCount )
							{
								for ( uint k = firstHelixIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
									residue.setHandedness( Model::Residue::HANDEDNESS::RIGHT );
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
							if ( LHelixCount == 0 ) { firstHelixIdx = residueIdx; }
							LHelixCount++;
						}
						else
						{
							if ( 3 <= LHelixCount )
							{
								for ( uint k = firstHelixIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
									residue.setHandedness( Model::Residue::HANDEDNESS::LEFT );
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
							if ( strandCount == 0 ) { firstStrandIdx = residueIdx; }
							strandCount++;
						}
						else
						{
							if ( 2 <= strandCount )
							{
								for ( uint k = firstStrandIdx; k < residueIdx; k++ )
								{
									Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + k );
									residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::STRAND );
									residue.setHandedness( Model::Residue::HANDEDNESS::RIGHT );
								}
							}
							strandCount = 0;
						}

						// ????
						// Model::Residue & residue = p_molecule.getResidue( idxFirstResidue + residueIdx );
						// residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::COIL );
						// residue.setHandedness( Model::Residue::HANDEDNESS::RIGHT );
					}
				}

				chrono.stop();
				VTX_DEBUG( "Secondary structure computed in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}
		} // namespace Molecule
	}	  // namespace Util
} // namespace VTX

#endif
