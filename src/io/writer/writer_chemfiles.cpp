#include "writer_chemfiles.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/chain.hpp"
#include "model/residue.hpp"
#include "tool/chrono.hpp"
#include "util/secondary_structure.hpp"
#include <string>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void ChemfilesWriter::writeFile( const FilePath & p_path, const Model::Molecule & p_molecule )
			{
				_prepareChemfiles();

				chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string(), 'w' );
				_writeTrajectory( trajectory, p_molecule );
				trajectory.close();
			}
			void ChemfilesWriter::writeBuffer( std::string & p_buffer, const Model::Molecule & p_molecule )
			{
				_prepareChemfiles();

				chemfiles::Trajectory & trajectory = chemfiles::Trajectory::memory_writer( "PDB" );
				_writeTrajectory( trajectory, p_molecule );

				chemfiles::span<const char> & span
					= trajectory.memory_buffer().value_or( chemfiles::span<const char>() );
				p_buffer.append( span.begin(), span.end() );
			}

			void ChemfilesWriter::fillTrajectoryFrames( chemfiles::Trajectory & p_trajectory,
														Model::Molecule &		p_molecule ) const
			{
				// Fill other frames.
				Tool::Chrono timeReadingFrames;
				timeReadingFrames.start();
				int startingFrame = 1;
				for ( uint frameIdx = 1; frameIdx < p_trajectory.nsteps(); ++frameIdx )
				{
					Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( frameIdx );
					chemfiles::Frame					  frame			= p_trajectory.read_step( frameIdx );
					const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame[ positionIdx ]		 = { position[ 0 ], position[ 1 ], position[ 2 ] };
					}
#ifdef _DEBUG
					if ( frameIdx % 100 == 0 )
					{
						VTX_DEBUG( "Frames from " + std::to_string( startingFrame ) + " to "
								   + std::to_string( frameIdx )
								   + " read in: " + std::to_string( timeReadingFrames.intervalTime() ) + "s" );
						startingFrame = frameIdx;
					}
#endif // DEBUG
				}
				timeReadingFrames.stop();
				VTX_INFO( "Frames read in: " + std::to_string( timeReadingFrames.elapsedTime() ) + "s" );
			}

			void ChemfilesWriter::_writeTrajectory( chemfiles::Trajectory & p_trajectory,
													const Model::Molecule & p_molecule )
			{
				chemfiles::Frame frame = chemfiles::Frame();

				frame.reserve( p_molecule.getAtomCount() );

				frame.set( "name", p_molecule.getName() );
				frame.set( "pdb_idcode", p_molecule.getPdbIdCode() );

				uint						currentExportedAtomIndex	= 0;
				uint						currentExportedResidueIndex = 0;
				std::map<std::string, uint> newChainIndexes				= std::map<std::string, uint>();
				std::map<std::string, std::vector<const Model::Residue *>> mapChainResidus
					= std::map<std::string, std::vector<const Model::Residue *>>();

				std::vector<const Model::Chain *> sortedChains = std::vector<const Model::Chain *>();
				sortedChains.reserve( p_molecule.getRealChainCount() );

				for ( const Model::Chain * const chain : p_molecule.getChains() )
				{
					if ( chain == nullptr )
						continue;

					sortedChains.emplace_back( chain );
				}

				std::sort( sortedChains.begin(),
						   sortedChains.end(),
						   []( const Model::Chain * a, const Model::Chain * b )
						   { return a->getName() < b->getName(); } );

				const Model::Chain * previousChain = nullptr;

				for ( const Model::Chain * const chain : sortedChains )
				{
					const std::string & chainName = chain->getName();

					if ( previousChain != nullptr && chainName == previousChain->getName() )
					{
						_mergedChains.emplace( chain->getIndex() );
						_mergedChains.emplace( previousChain->getIndex() );
					}

					for ( uint residueIndex = chain->getIndexFirstResidue();
						  residueIndex <= chain->getIndexLastResidue();
						  residueIndex++ )
					{
						const VTX::Model::Residue * const residue = p_molecule.getResidue( residueIndex );

						if ( residue == nullptr )
							continue;

						uint			   firstResAtomIdx = residue->getIndexFirstAtom();
						const uint		   atomCount	   = firstResAtomIdx + residue->getAtomCount();
						chemfiles::Residue chemResidue
							= chemfiles::Residue( residue->getSymbolStr(), residue->getIndexInOriginalChain() );

						chemResidue.set( "chainname", chainName );
						chemResidue.set( "chainid", chainName );

						chemResidue.set( "secondary_structure",
										 Util::SecondaryStructure::enumToPdbFormattedSecondaryStructure(
											 residue->getSecondaryStructure() ) );

						const bool stdPdb = residue->getType() == Model::Residue::TYPE::STANDARD
											&& residue->getSymbol() != Model::Residue::SYMBOL::UNKNOWN;

						if ( residue->hasInsertionCode() )
							chemResidue.set( "insertion_code", std::string( 1, residue->getInsertionCode() ) );

						chemResidue.set( "is_standard_pdb", stdPdb );

						// A specific value is needed for compsition_type to create "TER" tag in pdb export.
						// if ( stdPdb )
						//	chemResidue.set( "composition_type", "TER" );

						for ( firstResAtomIdx; firstResAtomIdx < atomCount; ++firstResAtomIdx )
						{
							const VTX::Model::Atom * const atom = p_molecule.getAtom( firstResAtomIdx );

							if ( atom == nullptr )
								continue;

							const VTX::Vec3f &	vtxAtomPos = p_molecule.getAtomPositionFrame( 0 )[ firstResAtomIdx ];
							chemfiles::Vector3D chemAtomPos
								= chemfiles::Vector3D( vtxAtomPos[ 0 ], vtxAtomPos[ 1 ], vtxAtomPos[ 2 ] );

							frame.add_atom( chemfiles::Atom( atom->getName(), atom->getSymbolStr() ), chemAtomPos );
							chemResidue.add_atom( currentExportedAtomIndex );

							_mapNewAtomIndexes[ firstResAtomIdx ] = currentExportedAtomIndex;
							currentExportedAtomIndex++;
						}

						frame.add_residue( chemResidue );
						_mapNewResidueIndexes[ residue->getIndex() ] = currentExportedResidueIndex;

						mapChainResidus[ chainName ].emplace_back( residue );
						currentExportedResidueIndex++;
					}

					previousChain = chain;
				}

				// add bonds
				for ( VTX::uint bond = 0; bond < p_molecule.getBondCount(); bond++ )
				{
					const VTX::Model::Bond * bnd = p_molecule.getBond( bond );

					if ( bnd == nullptr )
						continue;

					frame.add_bond( _mapNewAtomIndexes[ bnd->getIndexFirstAtom() ],
									_mapNewAtomIndexes[ bnd->getIndexSecondAtom() ],
									chemfiles::Bond::UNKNOWN );
				}

				p_trajectory.write( frame );
			}

			bool ChemfilesWriter::isChainMerged( const Model::Chain & p_chain ) const
			{
				return _mergedChains.find( p_chain.getIndex() ) != _mergedChains.end();
			}
			uint ChemfilesWriter::getNewResidueIndex( const Model::Residue & p_residue ) const
			{
				return _mapNewResidueIndexes.at( p_residue.getIndex() );
			}
			uint ChemfilesWriter::getNewAtomIndex( const Model::Atom & p_atom ) const
			{
				return _mapNewAtomIndexes.at( p_atom.getIndex() );
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
