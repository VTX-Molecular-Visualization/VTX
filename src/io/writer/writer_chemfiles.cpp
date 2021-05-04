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
				chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string(), 'w' );
				writeTrajectory( trajectory, p_molecule );
				trajectory.close();
			}
			void ChemfilesWriter::writeBuffer( std::string & p_buffer, const Model::Molecule & p_molecule )
			{
				chemfiles::Trajectory & trajectory = chemfiles::Trajectory::memory_writer( "PDB" );
				writeTrajectory( trajectory, p_molecule );

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

			void ChemfilesWriter::writeTrajectory( chemfiles::Trajectory & p_trajectory,
												   const Model::Molecule & p_molecule ) const
			{
				chemfiles::Frame frame = chemfiles::Frame();

				frame.reserve( p_molecule.getAtomCount() );

				frame.set( "name", p_molecule.getName() );
				frame.set( "pdb_idcode", p_molecule.getPdbIdCode() );

				std::map<uint, uint> mapNewAtomIndexes		  = std::map<uint, uint>();
				uint				 currentExportedAtomIndex = 0;

				// add residues
				for ( uint residueIndex = 0; residueIndex < p_molecule.getResidueCount(); residueIndex++ )
				{
					const VTX::Model::Residue * const residue = p_molecule.getResidue( residueIndex );

					if ( residue == nullptr )
						continue;

					uint			   firstResAtomIdx = residue->getIndexFirstAtom();
					const uint		   atomCount	   = firstResAtomIdx + residue->getAtomCount();
					chemfiles::Residue chemResidue
						= chemfiles::Residue( residue->getSymbolStr(), residue->getIndexInOriginalChain() );

					const Model::Chain * const chain	 = residue->getChainPtr();
					const std::string &		   chainName = chain->getName();

					chemResidue.set( "chainname", chainName );
					chemResidue.set( "chainid", chainName );

					chemResidue.set( "secondary_structure",
									 Util::SecondaryStructure::enumToPdbFormattedSecondaryStructure(
										 residue->getSecondaryStructure() ) );

					const bool stdPdb = residue->getType() == Model::Residue::TYPE::STANDARD
										&& residue->getSymbol() != Model::Residue::SYMBOL::UNKNOWN;

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

						mapNewAtomIndexes[ firstResAtomIdx ] = currentExportedAtomIndex;
						currentExportedAtomIndex++;
					}
					frame.add_residue( chemResidue );
				}
				// add bonds
				for ( VTX::uint bond = 0; bond < p_molecule.getBondCount(); bond++ )
				{
					const VTX::Model::Bond * bnd = p_molecule.getBond( bond );

					if ( bnd == nullptr )
						continue;

					frame.add_bond( mapNewAtomIndexes[ bnd->getIndexFirstAtom() ],
									mapNewAtomIndexes[ bnd->getIndexSecondAtom() ],
									chemfiles::Bond::UNKNOWN );
				}

				p_trajectory.write( frame );
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
