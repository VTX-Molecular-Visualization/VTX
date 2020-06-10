#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "model/ribbon.hpp"
#include "setting.hpp"
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
				p_molecule.getConfiguration().isSecondaryStructureLoadedFromFile = false;

				for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
				{
					const Model::Chain & chain			 = p_molecule.getChain( chainIdx );
					uint				 residueCount	 = chain.getResidueCount();
					uint				 idxFirstResidue = chain.getIndexFirstResidue();
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
							const Model::Residue & residue1 = p_molecule.getResidue( idxFirstResidue + residueIdx );
							const Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

							const Model::Atom * C0	= residue0.findFirstAtomByName( "C" );
							const Model::Atom * N1	= residue1.findFirstAtomByName( "N" );
							const Model::Atom * CA1 = residue1.findFirstAtomByName( "CA" );
							const Model::Atom * C1	= residue1.findFirstAtomByName( "C" );
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

					// ????
					/*
					for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
					{
						Model::Residue & residue0 = p_molecule.getResidue( idxFirstResidue + residueIdx - 1 );
						Model::Residue & residue  = p_molecule.getResidue( idxFirstResidue + residueIdx );
						Model::Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

						if ( ( residue0.getSecondaryStructure() == Model::Residue::SECONDARY_STRUCTURE::HELIX )
							 && ( residue2.getSecondaryStructure() == Model::Residue::SECONDARY_STRUCTURE::HELIX ) )
						{ residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX ); }
						else if ( ( residue0.getSecondaryStructure() == Model::Residue::SECONDARY_STRUCTURE::STRAND )
								  && ( residue2.getSecondaryStructure()
									   == Model::Residue::SECONDARY_STRUCTURE::STRAND ) )
						{
							residue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::STRAND );
						}
					}
					*/
				}

				chrono.stop();
				VTX_DEBUG( "Secondary structure computed in " + std::to_string( chrono.elapsedTime() ) + "s" );
			}

			static void mergeRanges( std::map<uint, uint> & p_ranges )
			{
				std::map<uint, uint>::iterator itFirst	= p_ranges.begin();
				std::map<uint, uint>::iterator itSecond = p_ranges.begin();
				itSecond++;
				while ( itSecond != p_ranges.end() )
				{
					if ( itSecond->first == itFirst->first + itFirst->second )
					{
						itFirst->second = itFirst->second + itSecond->second;
						p_ranges.erase( itSecond );
						itSecond = itFirst;
						itSecond++;
					}
					else
					{
						itFirst++;
						itSecond++;
					}
				}
			}

			static void refreshRepresentationState( Model::Molecule & p_molecule )
			{
				// Sort ranges by rep.
				Model::Molecule::RepresentationState & state = p_molecule.getRepresentationState();
				state.clear();
				state.emplace( Generic::REPRESENTATION::BALL_AND_STICK, Model::Molecule::RepresentationStruct() );
				state.emplace( Generic::REPRESENTATION::VAN_DER_WAALS, Model::Molecule::RepresentationStruct() );
				state.emplace( Generic::REPRESENTATION::STICK, Model::Molecule::RepresentationStruct() );
				state.emplace( Generic::REPRESENTATION::SAS, Model::Molecule::RepresentationStruct() );

				for ( Model::Residue * const residue : p_molecule.getResidues() )
				{
					// Skip hidden items.
					if ( residue->isVisible() == false || residue->getChainPtr()->isVisible() == false
						 || residue->getMoleculePtr()->isVisible() == false )
					{
						continue;
					}

					if ( p_molecule.showSolvent() == false && residue->getAtomType() == Model::Atom::TYPE::SOLVENT )
					{
						continue;
					}
					if ( p_molecule.showIon() == false && residue->getAtomType() == Model::Atom::TYPE::ION )
					{
						continue;
					}

					std::pair<uint, uint> rangeAtoms
						= std::pair( residue->getIndexFirstAtom(), residue->getAtomCount() );
					std::pair<uint, uint> rangeBonds
						= std::pair( residue->getIndiceFirstBond(), residue->getBondIndiceCount() );
					std::pair<uint, uint> rangeTriangles
						= std::pair( residue->getIndiceRibbonFirstTriangle(), residue->getIndiceRibbonTriangleCount() );

					const std::set<Generic::REPRESENTATION> * representations = nullptr;

					if ( residue->getRepresentations().size() > 0 )
					{
						representations = &residue->getRepresentations();
					}
					else if ( residue->getChainPtr()->getRepresentations().size() > 0 )
					{
						representations = &residue->getChainPtr()->getRepresentations();
					}
					else if ( residue->getMoleculePtr()->getRepresentations().size() > 0 )
					{
						representations = &residue->getMoleculePtr()->getRepresentations();
					}

					if ( representations != nullptr )
					{
						for ( const Generic::REPRESENTATION representation : *representations )
						{
							// Check size to avoid map overriding with same first index.
							if ( rangeAtoms.second > 0 )
							{
								state[ representation ].atoms.emplace( rangeAtoms );
							}
							if ( rangeBonds.second > 0 )
							{
								state[ representation ].bonds.emplace( rangeBonds );
							}
							if ( rangeTriangles.second > 0 )
							{
								state[ representation ].triangles.emplace( rangeTriangles );
							}
							for ( uint indexBond : residue->getIndexExtraBondStart() )
							{
								state[ representation ].bonds.emplace( indexBond * 2, 2 );
							}
							for ( uint indexBond : residue->getIndexExtraBondEnd() )
							{
								state[ representation ].bonds.emplace( indexBond * 2, 2 );
							}
						}
					}
					else
					{
						if ( rangeAtoms.second > 0 )
						{
							state[ Setting::Rendering::representation ].atoms.emplace( rangeAtoms );
						}
						if ( rangeBonds.second > 0 )
						{
							state[ Setting::Rendering::representation ].bonds.emplace( rangeBonds );
						}
						if ( rangeTriangles.second > 0 )
						{
							state[ Setting::Rendering::representation ].triangles.emplace( rangeTriangles );
						}
						for ( uint indexBond : residue->getIndexExtraBondStart() )
						{
							state[ Setting::Rendering::representation ].bonds.emplace( indexBond * 2, 2 );
						}
						for ( uint indexBond : residue->getIndexExtraBondEnd() )
						{
							state[ Setting::Rendering::representation ].bonds.emplace( indexBond * 2, 2 );
						}
					}
				}

				//

				// Merge  ranges.
				for ( std::pair<const Generic::REPRESENTATION, Model::Molecule::RepresentationStruct> & pair : state )
				{
					// Atoms.
					std::map<uint, uint> & rangeAtoms = pair.second.atoms;
					if ( rangeAtoms.size() > 1 )
					{
						// VTX_INFO( "Before merging atoms: " + std::to_string( rangeAtoms.size() ) );
						mergeRanges( rangeAtoms );
						// VTX_INFO( "After merging atoms: " + std::to_string( rangeAtoms.size() ) );
					}

					// Bonds.
					std::map<uint, uint> & rangeBonds = pair.second.bonds;
					if ( rangeBonds.size() > 1 )
					{
						// VTX_INFO( "Before merging bonds: " + std::to_string( rangeBonds.size() ) );
						mergeRanges( rangeBonds );
						// VTX_INFO( "After merging bonds: " + std::to_string( rangeBonds.size() ) );
					}

					// Triangles.
					std::map<uint, uint> & rangeTriangle = pair.second.triangles;
					if ( rangeTriangle.size() > 1 )
					{
						// VTX_INFO( "Before merging triangles: " + std::to_string( rangeTriangle.size() ) );
						mergeRanges( rangeTriangle );
						// VTX_INFO( "After merging triangles: " + std::to_string( rangeTriangle.size() ) );
					}
				}
			}

		} // namespace Molecule
	}	  // namespace Util
} // namespace VTX

#endif
