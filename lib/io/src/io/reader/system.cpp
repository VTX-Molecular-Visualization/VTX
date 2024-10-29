#include "io/reader/system.hpp"
#include "io/struct/system_configuration.hpp"
#include <core/chemdb/secondary_structure.hpp>
#include <core/struct/system.hpp>
#include <core/struct/trajectory.hpp>
#include <map>
#include <unordered_set>
#include <util/chrono.hpp>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::IO::Reader
{
	void System::readFile( const FilePath & p_path, VTX::Core::Struct::System & p_molecule )
	{
		_chemfilesReader = Reader::Chemfiles::readFile( p_path );
		_fillStructure( *_chemfilesReader, p_molecule );
	}
	void System::readBuffer(
		const std::string &			p_buffer,
		const FilePath &			p_path,
		VTX::Core::Struct::System & p_molecule
	)
	{
		_chemfilesReader = Reader::Chemfiles::readBuffer( p_buffer, p_path );
		_fillStructure( *_chemfilesReader, p_molecule );
	}

	void System::_fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, VTX::Core::Struct::System & p_molecule )
	{
		const std::string fileExtension = p_chemfileStruct.getPath().extension().string();

		size_t currentChainIndex = INVALID_INDEX;

		std::unordered_set<std::string> seenChainNames;
		size_t							currentChainResidueCount = 0;

		std::map<size_t, std::vector<size_t>> mapResidueBonds	   = std::map<size_t, std::vector<size_t>>();
		std::map<size_t, std::vector<size_t>> mapResidueExtraBonds = std::map<size_t, std::vector<size_t>>();

		VTX::Core::ChemDB::Category::TYPE lastCategoryEnum = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;

		p_molecule.trajectory.frames.resize( p_chemfileStruct.getFrameCount() );
		p_molecule.initResidues( p_chemfileStruct.getResidueCount() );
		p_molecule.initAtoms( p_chemfileStruct.getAtomCount() );

		VTX::Core::Struct::Frame & modelFrame = p_molecule.trajectory.frames[ 0 ];
		modelFrame.resize( p_chemfileStruct.getAtomCount() );

		for ( size_t residueIdx = 0; residueIdx < p_chemfileStruct.getResidueCount(); ++residueIdx )
		{
			p_chemfileStruct.setCurrentResidue( residueIdx );

			const std::string chainName	  = p_chemfileStruct.getCurrentChainName();
			const std::string chainId	  = p_chemfileStruct.getCurrentChainID();
			const std::string residueName = p_chemfileStruct.getCurrentResidueName();
			const size_t	  residueId	  = p_chemfileStruct.getCurrentResidueId();

			const VTX::Core::ChemDB::Category::TYPE categoryEnum = _findCategoryType( fileExtension, residueName );

			// Check if chain name changed.
			const bool createNewChain = p_molecule.getChainCount() == 0 ||			// No chain created
										not seenChainNames.contains( chainName ) || // unseen chain ID
										categoryEnum != lastCategoryEnum;			// New category

			if ( createNewChain )
			{
				if ( currentChainIndex != INVALID_INDEX )
				{
					p_molecule.chainResidueCounts[ currentChainIndex ] = currentChainResidueCount;
				}

				// Create chain.
				p_molecule.appendNewChain();
				currentChainIndex++;

				p_molecule.chainNames[ currentChainIndex ]		   = chainName;
				p_molecule.chainFirstResidues[ currentChainIndex ] = residueIdx;

				currentChainResidueCount = 0;

				p_molecule.getCategory( categoryEnum ).referenceChain( currentChainIndex );

				if ( not seenChainNames.contains( chainName ) )
					seenChainNames.emplace( chainName );
				lastCategoryEnum = categoryEnum;
			}

			currentChainResidueCount++;

			// Setup residue.
			const atom_index_t atomCount = p_chemfileStruct.getCurrentResidueAtomCount();
			if ( atomCount == 0 )
			{
				VTX_WARNING( "Empty residue found" );
			}

			p_molecule.residueChainIndexes[ residueIdx ]	 = currentChainIndex;
			p_molecule.residueFirstAtomIndexes[ residueIdx ] = p_chemfileStruct.getCurrentResidueFirstAtomIndex();
			p_molecule.residueAtomCounts[ residueIdx ]		 = atomCount;
			p_molecule.residueOriginalIds[ residueIdx ]		 = residueId;

			const ChemDB::Residue::SYMBOL residueSymbol = VTX::Core::ChemDB::Residue::getSymbolFromName( residueName );
			p_molecule.residueSymbols[ residueIdx ]		= residueSymbol;
			if ( residueSymbol == ChemDB::Residue::SYMBOL::UNKNOWN )
			{
				p_molecule.residueUnknownNames[ residueIdx ] = residueName;
			}

			const std::string secondaryStructure
				= p_chemfileStruct.getCurrentResidueStringProperty( "secondary_structure" );
			if ( secondaryStructure != "" )
			{
				p_molecule.residueSecondaryStructureTypes[ residueIdx ]
					= ChemDB::SecondaryStructure::pdbFormattedToEnum( secondaryStructure );
			}

			mapResidueBonds.emplace( residueIdx, std::vector<size_t>() );
			mapResidueExtraBonds.emplace( residueIdx, std::vector<size_t>() );

			// size_t solventCounter = 0;
			// size_t ionCounter	  = 0;
			Util::Math::RangeList<atom_index_t> atomSolvents = Util::Math::RangeList<atom_index_t>();
			Util::Math::RangeList<atom_index_t> atomIons	 = Util::Math::RangeList<atom_index_t>();

			for ( Chemfiles::ResidueIt it = p_chemfileStruct.getCurrentResidueAtomIteratorBegin();
				  it != p_chemfileStruct.getCurrentResidueAtomIteratorEnd();
				  ++it )
			{
				const atom_index_t atomIndex = *it;
				p_chemfileStruct.setCurrentAtom( atomIndex );

				p_molecule.atomResidueIndexes[ atomIndex ] = residueIdx;
				p_molecule.atomNames[ atomIndex ]		   = p_chemfileStruct.getCurrentAtomName();
				p_molecule.atomSymbols[ atomIndex ]		   = p_chemfileStruct.getCurrentAtomSymbol();

				const ChemDB::Atom::TYPE atomType = _getTypeInConfiguration( p_chemfileStruct );
				switch ( atomType )
				{
				case ChemDB::Atom::TYPE::NORMAL: break;
				case ChemDB::Atom::TYPE::ION: p_molecule.atomSolvents.addValue( atomIndex ); break;
				case ChemDB::Atom::TYPE::SOLVENT: p_molecule.atomIons.addValue( atomIndex ); break;
				default: break;
				}

				modelFrame[ atomIndex ] = p_chemfileStruct.getCurrentAtomPosition();
			}

			// TODO: Useless?
			//// Check residue full of solvent/ion.
			//// This is working only with pdb/psf files,
			//// not with arc/prm because arc do not contains topology.
			// if ( solventCounter == residue.size() )
			//{
			//	 modelResidue->setAtomType( ChemDB::Atom::TYPE::SOLVENT );
			// }
			// else if ( ionCounter == residue.size() )
			//{
			//	 modelResidue->setAtomType( ChemDB::Atom::TYPE::ION );
			// }
		}

		if ( currentChainResidueCount != 0 )
		{
			p_molecule.chainResidueCounts[ currentChainIndex ] = currentChainResidueCount;
			currentChainResidueCount						   = 0;
		}

		if ( p_chemfileStruct.getFrameCount() > 1 )
		{
			// TODO: launch the filling of trajectory frames in another thread
			// std::thread fillFrames(
			//	&MoleculeLoader::fillTrajectoryFrames, this, std::ref( trajectory ), std::ref( p_molecule ) );
			// fillFrames.detach();
			std::pair<VTX::Core::Struct::System *, size_t> pairMoleculeFirstFrame = { &p_molecule, 1 };
			_readTrajectoryFrames( p_chemfileStruct, { pairMoleculeFirstFrame }, 1 );
		}

		// Bonds.
		// Sort by residus.
		// Map with residue index to keep the order.
		size_t counter = 0;

		for ( size_t boundIdx = 0; boundIdx < p_chemfileStruct.getBondCount(); ++boundIdx )
		{
			p_chemfileStruct.setCurrentBond( boundIdx );

			const size_t firstAtomIndex = p_chemfileStruct.getCurrentBondFirstAtomIndex();
			const size_t residueStart	= p_molecule.atomResidueIndexes[ firstAtomIndex ];

			const size_t secondAtomIndex = p_chemfileStruct.getCurrentBondSecondAtomIndex();
			const size_t residueEnd		 = p_molecule.atomResidueIndexes[ secondAtomIndex ];

			if ( residueStart == residueEnd )
			{
				// Intra bonds.
				mapResidueBonds[ residueStart ].emplace_back( boundIdx );
				counter++;
			}
			else
			{
				// Extra bonds.
				mapResidueExtraBonds[ residueStart ].emplace_back( boundIdx );
				mapResidueExtraBonds[ residueEnd ].emplace_back( boundIdx );
				counter += 2;
			}
		}

		// Create models.
		p_molecule.initBonds( counter );

		const size_t counterOld = counter;
		counter					= 0;
		for ( size_t residueIdx = 0; residueIdx < p_chemfileStruct.getResidueCount(); ++residueIdx )
		{
			const std::vector<size_t> & vectorBonds		 = mapResidueBonds[ residueIdx ];
			const std::vector<size_t> & vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

			p_molecule.residueFirstBondIndexes[ residueIdx ] = counter;
			p_molecule.residueBondCounts[ residueIdx ]		 = vectorBonds.size() + vectorExtraBonds.size();

			for ( size_t i = 0; i < vectorBonds.size(); ++i, ++counter )
			{
				p_chemfileStruct.setCurrentBond( vectorBonds[ i ] );

				p_molecule.bondPairAtomIndexes[ counter * 2 ]	  = p_chemfileStruct.getCurrentBondFirstAtomIndex();
				p_molecule.bondPairAtomIndexes[ counter * 2 + 1 ] = p_chemfileStruct.getCurrentBondSecondAtomIndex();

				p_molecule.bondOrders[ counter ] = p_chemfileStruct.getCurrentBondOrder();
			}

			for ( size_t i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
			{
				p_chemfileStruct.setCurrentBond( vectorExtraBonds[ i ] );

				p_molecule.bondPairAtomIndexes[ counter * 2 ]	  = p_chemfileStruct.getCurrentBondFirstAtomIndex();
				p_molecule.bondPairAtomIndexes[ counter * 2 + 1 ] = p_chemfileStruct.getCurrentBondSecondAtomIndex();

				p_molecule.bondOrders[ counter ] = p_chemfileStruct.getCurrentBondOrder();
			}
		}

		// Util::Chrono bondComputationChrono = Util::Chrono();
		//  if ( !VTX::App::Application::Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
		//{
		//	bondComputationChrono.start();
		//	const bool allBondsRecomputed = Util::App::System::recomputeBondOrdersFromFile( p_molecule );

		//	if ( !allBondsRecomputed )
		//	{
		//		VTX_INFO( "recomputeBondOrders with algorithm." );
		//		Util::App::System::recomputeBondOrders( p_molecule );
		//	}
		//	bondComputationChrono.stop();
		//	VTX_INFO( "recomputeBondOrders: " + bondComputationChrono.elapsedTimeStr() );
		//}

		assert( counter == counterOld );
	}

	void System::_readTrajectoryFrames(
		IO::Reader::Chemfiles &												p_chemfileStruct,
		const std::vector<std::pair<VTX::Core::Struct::System *, size_t>> & p_targets,
		const size_t														p_trajectoryFrameStart
	)
	{
		// Fill other frames.
		Util::Chrono timeReadingFrames;
		timeReadingFrames.start();

#ifdef _DEBUG
		// Frame count for debug
		size_t startingFrame = 1;
#endif

		size_t validFrameCount = 0;
		for ( size_t frameIdx = 0; frameIdx < p_chemfileStruct.getFrameCount() - p_trajectoryFrameStart; ++frameIdx )
		{
			p_chemfileStruct.readNextFrame();
			const std::vector<Vec3f> atomPositions = p_chemfileStruct.getCurrentFrameAtomPosition();

			if ( atomPositions.size() <= 0 )
				continue;

			for ( const std::pair<VTX::Core::Struct::System *, size_t> & pairMoleculeStartFrame : p_targets )
			{
				VTX::Core::Struct::System & molecule   = *pairMoleculeStartFrame.first;
				const size_t				frameIndex = pairMoleculeStartFrame.second + validFrameCount;
				molecule.trajectory.fillFrame( frameIndex, atomPositions );

				validFrameCount++;
			}

#ifdef _DEBUG
			if ( frameIdx > 1 && frameIdx % 100 == 0 )
			{
				// VTX_DEBUG(
				//	"Frames from {} to {} read in: {}s.", startingFrame, frameIdx, timeReadingFrames.intervalTime()
				//);
				startingFrame = frameIdx;
			}
#endif // DEBUG
		}
		timeReadingFrames.stop();
		// VTX_INFO( "Frames read in: {}s", timeReadingFrames.elapsedTime() );

		// Erase supernumeraries frames
		for ( const std::pair<VTX::Core::Struct::System *, size_t> & pairMoleculeFirstFrame : p_targets )
		{
			VTX::Core::Struct::System &		molecule   = *( pairMoleculeFirstFrame.first );
			VTX::Core::Struct::Trajectory & trajectory = molecule.trajectory;
			if ( trajectory.frames.back().size() == 0 )
			{
				do
				{
					trajectory.frames.pop_back();
				} while ( trajectory.frames.back().size() == 0 );

				trajectory.frames.shrink_to_fit();
			}
		}
	}

	VTX::Core::ChemDB::Category::TYPE System::_findCategoryType(
		const std::string & p_fileExtension,
		const std::string & p_residueSymbol
	)
	{
		VTX::Core::ChemDB::Category::TYPE res;
		if ( p_fileExtension == "pdb" || p_fileExtension == "mmcif" || p_fileExtension == "mmtf" )
		{
			// TODO : Move that in Core::ChemDB
			// auto res = Util::App::Old::System::getResidueCategory( p_residueSymbol );
			res = VTX::Core::ChemDB::Category::TYPE::POLYMER;
		}
		else
		{
			res = VTX::Core::ChemDB::Category::TYPE::POLYMER;
		}

		return res;
	}

	ChemDB::Atom::TYPE System::_getTypeInConfiguration( const IO::Reader::Chemfiles & p_chemfileStruct ) const
	{
		if ( _configuration == nullptr )
			return ChemDB::Atom::TYPE::NORMAL;

		const int atomType = p_chemfileStruct.getCurrentAtomType();

		// Check better way to manage this
		// Check PRM.
		if ( atomType != -1 )
		{
			if ( std::find( _configuration->solventAtomIds.begin(), _configuration->solventAtomIds.end(), atomType )
				 != _configuration->solventAtomIds.end() )
			{
				return ChemDB::Atom::TYPE::SOLVENT;
			}
			else if ( std::find( _configuration->ionAtomIds.begin(), _configuration->ionAtomIds.end(), atomType )
					  != _configuration->ionAtomIds.end() )
			{
				return ChemDB::Atom::TYPE::ION;
			}
		}

		const std::string residueSymbol = p_chemfileStruct.getCurrentResidueName();

		// Check PSF.
		if ( std::find(
				 _configuration->solventResidueSymbols.begin(),
				 _configuration->solventResidueSymbols.end(),
				 residueSymbol
			 )
			 != _configuration->solventResidueSymbols.end() )
		{
			return ChemDB::Atom::TYPE::SOLVENT;
		}
		else if ( std::find(
					  _configuration->ionResidueSymbols.begin(), _configuration->ionResidueSymbols.end(), residueSymbol
				  )
				  != _configuration->ionResidueSymbols.end() )
		{
			return ChemDB::Atom::TYPE::ION;
		}

		return ChemDB::Atom::TYPE::NORMAL;
	}

} // namespace VTX::IO::Reader
