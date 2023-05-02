#include "app/internal/io/writer/writer_chemfiles.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/old_app/define.hpp"
#include "app/util/secondary_structure.hpp"
#include <string>

namespace VTX::App::Internal::IO::Writer
{
	namespace ChemDB = VTX::App::Internal::ChemDB;

	void ChemfilesWriter::writeFile( const FilePath & p_path, const App::Component::Chemistry::Molecule & p_molecule )
	{
		_prepareChemfiles();

		chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string(), 'w' );
		_writeTrajectory( trajectory, p_molecule );
		trajectory.close();
	}
	void ChemfilesWriter::writeBuffer( std::string &							   p_buffer,
									   const App::Component::Chemistry::Molecule & p_molecule,
									   const std::string &						   p_format )
	{
		_prepareChemfiles();

		chemfiles::Trajectory trajectory = chemfiles::Trajectory::memory_writer( p_format );
		_writeTrajectory( trajectory, p_molecule );

		chemfiles::span<const char> span = trajectory.memory_buffer().value_or( chemfiles::span<const char>() );
		p_buffer.append( span.begin(), span.end() );
	}

	void ChemfilesWriter::fillTrajectoryFrames( chemfiles::Trajectory &				  p_trajectory,
												App::Component::Chemistry::Molecule & p_molecule ) const
	{
		// Fill other frames.
		Util::Chrono timeReadingFrames;
		timeReadingFrames.start();
		int startingFrame = 1;
		for ( uint frameIdx = 1; frameIdx < p_trajectory.nsteps(); ++frameIdx )
		{
			App::Component::Chemistry::Molecule::AtomPositionsFrame & moleculeFrame
				= p_molecule.getAtomPositionFrame( frameIdx );
			chemfiles::Frame							 frame	   = p_trajectory.read_step( frameIdx );
			const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
			for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
			{
				const chemfiles::Vector3D & position = positions[ positionIdx ];
				moleculeFrame[ positionIdx ]		 = { position[ 0 ], position[ 1 ], position[ 2 ] };
			}
#ifdef _DEBUG
			if ( frameIdx % 100 == 0 )
			{
				_logDebug( "Frames from " + std::to_string( startingFrame ) + " to " + std::to_string( frameIdx )
						   + " read in: " + std::to_string( timeReadingFrames.intervalTime() ) + "s" );
				startingFrame = frameIdx;
			}
#endif // DEBUG
		}
		timeReadingFrames.stop();
		_logInfo( "Frames read in: " + std::to_string( timeReadingFrames.elapsedTime() ) + "s" );
	}

	void ChemfilesWriter::_writeTrajectory( chemfiles::Trajectory &						p_trajectory,
											const App::Component::Chemistry::Molecule & p_molecule )
	{
		_vecNewAtomIndexes.resize( p_molecule.getAtomCount(), INVALID_ID );
		_vecNewResidueIndexes.resize( p_molecule.getResidueCount(), INVALID_ID );
		_vecNewChainIndexes.resize( p_molecule.getChainCount(), INVALID_ID );

		for ( uint i = 0; i < p_molecule.getFrameCount(); i++ )
		{
			chemfiles::Frame frame = chemfiles::Frame();

			frame.reserve( p_molecule.getAtomCount() );

			frame.set( "name", p_molecule.getName() );
			frame.set( "pdb_idcode", p_molecule.getPdbIdCode() );

			uint currentExportedAtomIndex	 = 0;
			uint currentExportedResidueIndex = 0;
			uint currentExportedChainIndex	 = 0;

			for ( const App::Component::Chemistry::Chain * const chain : p_molecule.getChains() )
			{
				if ( chain == nullptr )
					continue;

				const std::string & chainName = chain->getName();
				const std::string & chainId
					= chain->getOriginalChainID().empty() ? chainName : chain->getOriginalChainID();

				for ( uint residueIndex = chain->getIndexFirstResidue(); residueIndex <= chain->getIndexLastResidue();
					  residueIndex++ )
				{
					const VTX::App::Component::Chemistry::Residue * const residue
						= p_molecule.getResidue( residueIndex );

					if ( residue == nullptr )
						continue;

					uint			   firstResAtomIdx = residue->getIndexFirstAtom();
					const uint		   atomCount	   = firstResAtomIdx + residue->getAtomCount();
					chemfiles::Residue chemResidue
						= chemfiles::Residue( residue->getSymbolStr(), residue->getIndexInOriginalChain() );

					chemResidue.set( "chainname", chainName );
					chemResidue.set( "chainid", chainId );

					chemResidue.set( "secondary_structure",
									 Util::App::SecondaryStructure::enumToPdbFormattedSecondaryStructure(
										 residue->getSecondaryStructure() ) );

					if ( residue->hasInsertionCode() )
						chemResidue.set( "insertion_code", std::string( 1, residue->getInsertionCode() ) );

					const bool isStdPdb = residue->getType() == ChemDB::Residue::TYPE::STANDARD
										  && residue->getSymbol() != ChemDB::Residue::SYMBOL::UNKNOWN;
					chemResidue.set( "is_standard_pdb", isStdPdb );

					for ( ; firstResAtomIdx < atomCount; ++firstResAtomIdx )
					{
						const VTX::App::Component::Chemistry::Atom * const atom = p_molecule.getAtom( firstResAtomIdx );

						if ( atom == nullptr )
							continue;

						const VTX::Vec3f &	vtxAtomPos = p_molecule.getAtomPositionFrame( i )[ firstResAtomIdx ];
						chemfiles::Vector3D chemAtomPos
							= chemfiles::Vector3D( vtxAtomPos[ 0 ], vtxAtomPos[ 1 ], vtxAtomPos[ 2 ] );

						frame.add_atom( chemfiles::Atom( atom->getName(), atom->getSymbolStr() ), chemAtomPos );
						chemResidue.add_atom( currentExportedAtomIndex );

						_vecNewAtomIndexes[ firstResAtomIdx ] = currentExportedAtomIndex;
						currentExportedAtomIndex++;
					}

					frame.add_residue( chemResidue );
					_vecNewResidueIndexes[ residue->getIndex() ] = currentExportedResidueIndex;

					currentExportedResidueIndex++;
				}

				_vecNewChainIndexes[ chain->getIndex() ] = currentExportedChainIndex;
				currentExportedChainIndex++;
			}

			// add bonds
			for ( VTX::uint bond = 0; bond < p_molecule.getBondCount(); bond++ )
			{
				const VTX::App::Component::Chemistry::Bond * bnd = p_molecule.getBond( bond );

				if ( bnd == nullptr )
					continue;

				const uint firstAtomIndex  = _vecNewAtomIndexes[ bnd->getIndexFirstAtom() ];
				const uint secondAtomIndex = _vecNewAtomIndexes[ bnd->getIndexSecondAtom() ];

				if ( firstAtomIndex == INVALID_ID || secondAtomIndex == INVALID_ID )
					continue;

				frame.add_bond( firstAtomIndex, secondAtomIndex, chemfiles::Bond::BondOrder( int( bnd->getOrder() ) ) );
			}
			p_trajectory.write( frame );
		}
	}

	uint ChemfilesWriter::getNewChainIndex( const App::Component::Chemistry::Chain & p_chain ) const
	{
		return _vecNewChainIndexes[ p_chain.getIndex() ];
	}
	uint ChemfilesWriter::getNewResidueIndex( const App::Component::Chemistry::Residue & p_residue ) const
	{
		return _vecNewResidueIndexes[ p_residue.getIndex() ];
	}
	uint ChemfilesWriter::getNewAtomIndex( const App::Component::Chemistry::Atom & p_atom ) const
	{
		return _vecNewAtomIndexes[ p_atom.getIndex() ];
	}
} // namespace VTX::App::Internal::IO::Writer
