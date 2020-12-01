#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_enum.hpp"
#include "model/representation/representation_library.hpp"
#include "model/secondary_structure.hpp"
#include "representation/representation_manager.hpp"
#include "tool/chrono.hpp"
#include "vtx_app.hpp"
#include <map>

namespace VTX
{
	namespace Util
	{
		namespace Molecule
		{
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
				Model::Molecule::RepresentationState & state						= p_molecule.getRepresentationState();
				const Model::SecondaryStructure &	   secondaryStructure			= p_molecule.getSecondaryStructure();
				const std::map<uint, uint>			   residueToControlPointIndices = secondaryStructure.getResidueToControlPointIndice();

				std::vector<const Model::Representation::BaseRepresentation *> representations = std::vector<const Model::Representation::BaseRepresentation *>();
				Representation::RepresentationManager::get().fillVectorWithRepresentations( &p_molecule, representations );

				for ( const Model::Representation::BaseRepresentation * representation : representations )
					state.emplace( representation, Model::Molecule::RepresentationStruct() );

				for ( Model::Residue * const residue : p_molecule.getResidues() )
				{
					// Skip hidden items.
					if ( residue->isVisible() == false || residue->getChainPtr()->isVisible() == false || residue->getMoleculePtr()->isVisible() == false )
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

					std::pair<uint, uint> rangeAtoms   = std::pair( residue->getIndexFirstAtom(), residue->getAtomCount() );
					std::pair<uint, uint> rangeBonds   = std::pair( residue->getIndiceFirstBond(), residue->getBondIndiceCount() );
					std::pair<uint, uint> rangeRibbons = std::pair( 0, 0 );

					if ( residueToControlPointIndices.find( residue->getIndex() ) != residueToControlPointIndices.end() )
					{
						rangeRibbons = std::pair( residueToControlPointIndices.at( residue->getIndex() ), 4 );
					}

					const std::set<const Model::Representation::BaseRepresentation *> * representations = nullptr;

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
						for ( const Model::Representation::BaseRepresentation * const representation : *representations )
						{
							const Generic::REPRESENTATION & representationType = representation->getRepresentationType();

							// Check size to avoid map overriding with same first index.
							if ( rangeAtoms.second > 0 )
							{
								state[ representation ].atoms.emplace( rangeAtoms );
							}
							if ( rangeBonds.second > 0 )
							{
								state[ representation ].bonds.emplace( rangeBonds );
							}
							if ( rangeRibbons.second > 0 )
							{
								state[ representation ].ribbons.emplace( rangeRibbons );
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
						const Model::Representation::BaseRepresentation * const defaultRepresentation
							= Model::Representation::RepresentationLibrary::get().getRepresentation( VTX_SETTING().representation );
						{
							state[ defaultRepresentation ].atoms.emplace( rangeAtoms );
						}
						if ( rangeBonds.second > 0 )
						{
							state[ defaultRepresentation ].bonds.emplace( rangeBonds );
						}
						if ( rangeRibbons.second > 0 )
						{
							state[ defaultRepresentation ].ribbons.emplace( rangeRibbons );
						}
						for ( uint indexBond : residue->getIndexExtraBondStart() )
						{
							state[ defaultRepresentation ].bonds.emplace( indexBond * 2, 2 );
						}
						for ( uint indexBond : residue->getIndexExtraBondEnd() )
						{
							state[ defaultRepresentation ].bonds.emplace( indexBond * 2, 2 );
						}
					}
				}

				// Merge ranges.
				for ( std::pair<const Model::Representation::BaseRepresentation * const, Model::Molecule::RepresentationStruct> & pair : state )
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

					// Ribbons.
					std::map<uint, uint> & rangeRibbons = pair.second.ribbons;
					if ( rangeRibbons.size() > 1 )
					{
						// VTX_INFO( "Before merging ribbons: " + std::to_string( rangeRibbons.size() ) );
						mergeRanges( rangeRibbons );
						// VTX_INFO( "After merging ribbons: " + std::to_string( rangeRibbons.size() ) );
					}
				}
			}

		} // namespace Molecule
	}	  // namespace Util
} // namespace VTX

#endif
