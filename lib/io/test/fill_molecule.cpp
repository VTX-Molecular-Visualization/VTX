#include "fill_molecule.hpp"
#include "define.hpp"
#include <magic_enum.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::IO::Test
{
	namespace ChemDB = VTX::Core::ChemDB;
	void fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, CustomMolecule & p_molecule )
	{
		const std::string fileExtension = p_chemfileStruct.getPath().extension().string();

		CustomChain * modelChain			   = nullptr;
		std::string	  lastChainName			   = "";
		size_t		  chainModelId			   = -1;
		size_t		  currentChainResidueCount = 0;

		std::map<size_t, std::vector<size_t>> mapResidueBonds	   = std::map<size_t, std::vector<size_t>>();
		std::map<size_t, std::vector<size_t>> mapResidueExtraBonds = std::map<size_t, std::vector<size_t>>();

		int								  oldIndexInChain  = INT_MIN;
		VTX::Core::ChemDB::Category::TYPE lastCategoryEnum = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;

		p_molecule.getTrajectory().frames.resize( p_chemfileStruct.getFrameCount() );
		p_molecule.initResidues( p_chemfileStruct.getResidueCount() );
		p_molecule.initAtoms( p_chemfileStruct.getAtomCount() );

		VTX::Core::Struct::Frame & modelFrame = p_molecule.getTrajectory().frames[ 0 ];
		modelFrame.resize( p_chemfileStruct.getAtomCount() );

		for ( size_t residueIdx = 0; residueIdx < p_chemfileStruct.getResidueCount(); ++residueIdx )
		{
			p_chemfileStruct.setCurrentResidue( residueIdx );

			const std::string chainName	  = p_chemfileStruct.getCurrentChainName();
			const std::string chainId	  = p_chemfileStruct.getCurrentChainID();
			std::string		  residueName = p_chemfileStruct.getCurrentResidueName();

			const VTX::Core::ChemDB::Category::TYPE categoryEnum = _findCategoryType( fileExtension, residueName );

			// Check if chain name changed.
			const bool createNewChain = p_molecule.getChainCount() == 0 || // No chain created
										chainName != lastChainName ||	   // New chain ID
										categoryEnum != lastCategoryEnum;  // New category

			if ( createNewChain )
			{
				if ( modelChain != nullptr )
				{
					modelChain->setResidueCount( currentChainResidueCount );
				}

				// Create chain.
				modelChain = p_molecule.appendNewChain();

				if ( chainName != "" )
				{
					modelChain->setName( chainName );
				}
				modelChain->setIndexFirstResidue( residueIdx );
				currentChainResidueCount = 0;

				p_molecule.getCategory( categoryEnum ).referenceChain( modelChain->getIndex() );

				lastChainName	 = chainName;
				lastCategoryEnum = categoryEnum;
				oldIndexInChain	 = INT_MIN;
			}

			currentChainResidueCount++;

			// Setup residue.
			CustomResidue * modelResidue = p_molecule.getResidues()[ residueIdx ];

			const size_t atomCount = p_chemfileStruct.getCurrentResidueAtomCount();
			if ( atomCount == 0 )
			{
				VTX_WARNING( "Empty residue found" );
			}

			modelResidue->setChainPtr( modelChain );
			modelResidue->setIndexFirstAtom( p_chemfileStruct.getCurrentResidueFirstAtomIndex() );
			modelResidue->setAtomCount( atomCount );

			ChemDB::Residue::SYMBOL residueSymbol = Core::ChemDB::Residue::getSymbolFromName( residueName );

			int symbolValue;

			if ( residueSymbol == ChemDB::Residue::SYMBOL::UNKNOWN )
			{
				const int symbolIndex = 0;
				// int symbolIndex = p_molecule.getUnknownResidueSymbolIndex( residueSymbol );
				//  VTX::Core::ChemDB::UnknownResidueData * unknownResidueData;

				// if ( symbolIndex >= 0 )
				//{
				//	unknownResidueData = p_molecule.getUnknownResidueSymbol( symbolIndex );
				// }
				// else
				//{
				//	unknownResidueData			   = new VTX::Core::ChemDB::UnknownResidueData();
				//	unknownResidueData->symbolStr  = residueSymbol;
				//	unknownResidueData->symbolName = Util::App::Molecule::getResidueFullName( residueSymbol );

				//	symbolIndex = p_molecule.addUnknownResidueSymbol( unknownResidueData );
				//}

				symbolValue = int( ChemDB::Residue::SYMBOL::COUNT ) + symbolIndex;
			}
			else
			{
				symbolValue = int( residueSymbol );
			}

			modelResidue->setSymbol( residueSymbol );

			mapResidueBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );
			mapResidueExtraBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );

			size_t solventCounter = 0;
			size_t ionCounter	  = 0;

			for ( IO::Reader::Chemfiles::ResidueIt it = p_chemfileStruct.getCurrentResidueAtomIteratorBegin();
				  it != p_chemfileStruct.getCurrentResidueAtomIteratorEnd();
				  it++ )
			{
				const size_t atomId = *it;
				p_chemfileStruct.setCurrentAtom( atomId );

				// Create atom.
				CustomAtom * modelAtom = p_molecule.getAtoms()[ atomId ];
				modelAtom->setResiduePtr( modelResidue );
				modelAtom->setName( p_chemfileStruct.getCurrentAtomName() );
				modelAtom->setSymbol( p_chemfileStruct.getCurrentAtomSymbol() );

				modelFrame[ atomId ] = p_chemfileStruct.getCurrentAtomPosition();
			}
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
			modelChain->setResidueCount( modelChain->getResidueCount() + 1 );
		}

		if ( p_chemfileStruct.getFrameCount() > 1 )
		{
			// TODO: launch the filling of trajectory frames in another thread
			// std::thread fillFrames(
			//	&MoleculeLoader::fillTrajectoryFrames, this, std::ref( trajectory ), std::ref( p_molecule ) );
			// fillFrames.detach();
			std::pair<CustomMolecule *, size_t> pairMoleculeFirstFrame = { &p_molecule, 1 };
			_readTrajectoryFrames( p_chemfileStruct, { pairMoleculeFirstFrame }, 1 );
		}

		Util::Chrono bondComputationChrono = Util::Chrono();

		// Bonds.
		// Sort by residus.
		// Map with residue index to keep the order.
		size_t counter = 0;

		for ( size_t boundIdx = 0; boundIdx < p_chemfileStruct.getBondCount(); ++boundIdx )
		{
			p_chemfileStruct.setCurrentBond( boundIdx );

			CustomResidue * residueStart
				= p_molecule.getAtom( p_chemfileStruct.getCurrentBondFirstAtomIndex() )->getResiduePtr();
			CustomResidue * residueEnd
				= p_molecule.getAtom( p_chemfileStruct.getCurrentBondSecondAtomIndex() )->getResiduePtr();

			if ( residueStart == residueEnd )
			{
				// Intra bonds.
				mapResidueBonds[ residueStart->getIndex() ].emplace_back( boundIdx );
				counter++;
			}
			else
			{
				// Extra bonds.
				mapResidueExtraBonds[ residueStart->getIndex() ].emplace_back( boundIdx );
				mapResidueExtraBonds[ residueEnd->getIndex() ].emplace_back( boundIdx );
				counter += 2;
			}
		}

		// Create models.
		p_molecule.initBonds( counter );

		const size_t counterOld = counter;
		counter					= 0;
		for ( size_t residueIdx = 0; residueIdx < p_chemfileStruct.getResidueCount(); ++residueIdx )
		{
			CustomResidue * const		residue			 = p_molecule.getResidue( residueIdx );
			const std::vector<size_t> & vectorBonds		 = mapResidueBonds[ residueIdx ];
			const std::vector<size_t> & vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

			residue->setIndexFirstBond( counter );
			residue->setBondCount( vectorBonds.size() + vectorExtraBonds.size() );

			for ( size_t i = 0; i < vectorBonds.size(); ++i, ++counter )
			{
				p_chemfileStruct.setCurrentBond( vectorBonds[ i ] );
				CustomBond * modelBond = p_molecule.getBonds()[ counter ];
				modelBond->setIndex( i );

				modelBond->setIndexFirstAtom( p_chemfileStruct.getCurrentBondFirstAtomIndex() );
				modelBond->setIndexSecondAtom( p_chemfileStruct.getCurrentBondSecondAtomIndex() );
				modelBond->setOrder( p_chemfileStruct.getCurrentBondOrder() );
			}

			for ( size_t i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
			{
				p_chemfileStruct.setCurrentBond( vectorExtraBonds[ i ] );
				CustomBond * modelBond = p_molecule.getBonds()[ counter ];

				modelBond->setMoleculePtr( &p_molecule );
				modelBond->setIndexFirstAtom( p_chemfileStruct.getCurrentBondFirstAtomIndex() );
				modelBond->setIndexSecondAtom( p_chemfileStruct.getCurrentBondSecondAtomIndex() );
				modelBond->setOrder( p_chemfileStruct.getCurrentBondOrder() );
			}
		}

		// if ( !VTX::App::Application::Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
		//{
		//	bondComputationChrono.start();
		//	const bool allBondsRecomputed = Util::App::Molecule::recomputeBondOrdersFromFile( p_molecule );

		//	if ( !allBondsRecomputed )
		//	{
		//		VTX_INFO( "recomputeBondOrders with algorithm." );
		//		Util::App::Molecule::recomputeBondOrders( p_molecule );
		//	}
		//	bondComputationChrono.stop();
		//	VTX_INFO( "recomputeBondOrders: " + bondComputationChrono.elapsedTimeStr() );
		//}

		assert( counter == counterOld );
	}
	void _readTrajectoryFrames( IO::Reader::Chemfiles &									 p_chemfileStruct,
								const std::vector<std::pair<CustomMolecule *, size_t>> & p_targets,
								const size_t											 p_trajectoryFrameStart )
	{
		// Fill other frames.
		Util::Chrono timeReadingFrames;
		timeReadingFrames.start();
		size_t startingFrame   = 1;
		size_t validFrameCount = 0;
		for ( size_t frameIdx = 0; frameIdx < p_chemfileStruct.getFrameCount() - p_trajectoryFrameStart; ++frameIdx )
		{
			p_chemfileStruct.readNextFrame();
			const std::vector<Vec3f> atomPositions = p_chemfileStruct.getCurrentFrameAtomPosition();

			if ( atomPositions.size() <= 0 )
				continue;

			for ( const std::pair<CustomMolecule *, size_t> & pairMoleculeStartFrame : p_targets )
			{
				CustomMolecule & molecule	= *pairMoleculeStartFrame.first;
				const size_t	 frameIndex = pairMoleculeStartFrame.second + validFrameCount;
				molecule.getTrajectory().fillFrame( frameIndex, atomPositions );

				validFrameCount++;
			}

#ifdef _DEBUG
			if ( frameIdx > 1 && frameIdx % 100 == 0 )
			{
				VTX_DEBUG(
					"Frames from {} to {} read in: {}s.", startingFrame, frameIdx, timeReadingFrames.intervalTime() );
				startingFrame = frameIdx;
			}
#endif // DEBUG
		}
		timeReadingFrames.stop();
		VTX_INFO( "Frames read in: {}s", timeReadingFrames.elapsedTime() );

		// Erase supernumeraries frames
		for ( const std::pair<CustomMolecule *, size_t> & pairMoleculeFirstFrame : p_targets )
		{
			CustomMolecule &				molecule   = *( pairMoleculeFirstFrame.first );
			VTX::Core::Struct::Trajectory & trajectory = molecule.getTrajectory();
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

	VTX::Core::ChemDB::Category::TYPE _findCategoryType( const std::string & p_fileExtension,
														 const std::string & p_residueSymbol )
	{
		VTX::Core::ChemDB::Category::TYPE res;
		if ( p_fileExtension == "pdb" || p_fileExtension == "mmcif" || p_fileExtension == "mmtf" )
		{
			// TODO : Move that in Core::ChemDB
			// res = Util::App::Old::Molecule::getResidueCategory( p_residueSymbol );
			res = VTX::Core::ChemDB::Category::TYPE::POLYMER;
		}
		else
		{
			res = VTX::Core::ChemDB::Category::TYPE::POLYMER;
		}

		return res;
	}

} // namespace VTX::IO::Test
