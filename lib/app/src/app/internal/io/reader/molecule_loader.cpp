#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/internal/util/molecule.hpp"
#include <algorithm>
#include <core/chemdb/category.hpp>
#include <core/chemdb/chain.hpp>
#include <core/struct/trajectory.hpp>
// #include <io/util/chemfiles.hpp>
#include <iostream>
#include <magic_enum.hpp>
#include <thread>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Internal::IO::Reader
{
	namespace ChemDB = VTX::Core::ChemDB;

	void MoleculeLoader::readFile( const FilePath & p_path, App::Component::Chemistry::Molecule & p_molecule )
	{
		std::unique_ptr<VTX::IO::Reader::Chemfiles> chemfilesReader = VTX::IO::Reader::Chemfiles::readFile( p_path );

		const bool recomputeBonds = _needToRecomputeBonds( _getFormat( p_path ) );
		_fillStructure( *chemfilesReader, p_molecule, recomputeBonds );
	}

	void MoleculeLoader::readBuffer( const std::string &				   p_buffer,
									 const FilePath &					   p_path,
									 App::Component::Chemistry::Molecule & p_molecule )
	{
		std::unique_ptr<VTX::IO::Reader::Chemfiles> chemfilesReader
			= VTX::IO::Reader::Chemfiles::readBuffer( p_buffer, p_path );

		_fillStructure( *chemfilesReader, p_molecule );
	}

	void MoleculeLoader::fillTrajectoryFrame( App::Component::Chemistry::Molecule & p_molecule,
											  const size_t							p_moleculeFrameIndex,
											  const std::vector<Vec3f> &			p_atomPositions )
	{
		VTX::Core::Struct::Trajectory & moleculeTrajectory = p_molecule.getTrajectory();
		moleculeTrajectory.frames[ p_moleculeFrameIndex ].resize( p_atomPositions.size() );

		std::copy(
			p_atomPositions.begin(), p_atomPositions.end(), moleculeTrajectory.frames[ p_moleculeFrameIndex ].begin() );
	}

	void MoleculeLoader::_readTrajectoryFrames(
		VTX::IO::Reader::Chemfiles &												  p_chemfilesReader,
		const std::vector<std::pair<App::Component::Chemistry::Molecule *, size_t>> & p_targets,
		const size_t																  p_trajectoryFrameStart )
	{
		// Fill other frames.
		VTX::Util::Chrono timeReadingFrames;
		timeReadingFrames.start();
		size_t startingFrame   = 1;
		size_t validFrameCount = 0;
		for ( size_t frameIdx = 0; frameIdx < p_chemfilesReader.getFrameCount() - p_trajectoryFrameStart; ++frameIdx )
		{
			p_chemfilesReader.readNextFrame();

			const std::vector<Vec3f> atomPositions = p_chemfilesReader.getCurrentFrameAtomPosition();

			if ( atomPositions.size() <= 0 )
				continue;

			for ( const std::pair<App::Component::Chemistry::Molecule *, size_t> & pairMoleculeStartFrame : p_targets )
			{
				fillTrajectoryFrame(
					*pairMoleculeStartFrame.first, pairMoleculeStartFrame.second + validFrameCount, atomPositions );
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
		for ( const std::pair<App::Component::Chemistry::Molecule *, size_t> & pairMoleculeFirstFrame : p_targets )
		{
			App::Component::Chemistry::Molecule & molecule	 = *( pairMoleculeFirstFrame.first );
			VTX::Core::Struct::Trajectory &		  trajectory = molecule.getTrajectory();
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

	void MoleculeLoader::_fillStructure( VTX::IO::Reader::Chemfiles &		   p_chemfilesReader,
										 App::Component::Chemistry::Molecule & p_molecule,
										 const bool							   p_recomputeBonds )
	{
		const FilePath & path = p_chemfilesReader.getPath();

		// Set molecule properties.
		p_molecule.setName( p_chemfilesReader.getFrameName() );
		p_molecule.setPdbIdCode( p_chemfilesReader.getPdbIdCode() );

		// Initialized in Molecule model
		// p_molecule.setColor( Util::Color::Rgba::randomPastel() );

		// Create models.
		p_molecule.getTrajectory().frames.resize( p_chemfilesReader.getFrameCount() );
		p_molecule.initResidues( p_chemfilesReader.getResidueCount() );
		p_molecule.initAtoms( p_chemfilesReader.getAtomCount() );
		// p_molecule.resizeBuffers();
		VTX::Core::Struct::Frame & modelFrame = p_molecule.getTrajectory().frames[ 0 ];
		modelFrame.resize( p_chemfilesReader.getAtomCount() );

		App::Component::Chemistry::Chain * modelChain				= nullptr;
		std::string						   lastChainName			= "";
		size_t							   chainModelId				= -1;
		size_t							   currentChainResidueCount = 0;

		std::map<size_t, std::vector<size_t>> mapResidueBonds	   = std::map<size_t, std::vector<size_t>>();
		std::map<size_t, std::vector<size_t>> mapResidueExtraBonds = std::map<size_t, std::vector<size_t>>();

		size_t							  oldIndexInChain  = INVALID_INDEX;
		VTX::Core::ChemDB::Category::TYPE lastCategoryEnum = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;

		for ( size_t residueIdx = 0; residueIdx < p_chemfilesReader.getResidueCount(); ++residueIdx )
		{
			p_chemfilesReader.setCurrentResidue( residueIdx );

			// Check if chain name changed.
			const std::string chainName		= p_chemfilesReader.getCurrentChainName();
			const std::string chainId		= p_chemfilesReader.getCurrentChainID();
			std::string		  residueSymbol = p_chemfilesReader.getCurrentResidueName();

			const VTX::Core::ChemDB::Category::TYPE categoryEnum
				= App::Internal::Util::Molecule::getResidueCategory( residueSymbol );

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
				modelChain = new Component::Chemistry::Chain( &p_molecule, ++chainModelId );
				p_molecule.getChains().emplace_back( modelChain );

				if ( chainName != "" )
				{
					modelChain->setName( chainName );
				}
				modelChain->setMoleculePtr( &p_molecule );
				modelChain->setIndexFirstResidue( residueIdx );
				currentChainResidueCount = 0;
				modelChain->setOriginalChainID( chainId );
				modelChain->setColor( ChemDB::Chain::getChainIdColor( chainId ) );

				p_molecule.getCategory( categoryEnum ).referenceChain( modelChain->getIndex() );
				// modelChain->setCategoryEnum( categoryEnum );

				lastChainName	 = chainName;
				lastCategoryEnum = categoryEnum;
				oldIndexInChain	 = INVALID_INDEX;
			}
			else
			{
				modelChain = static_cast<Component::Chemistry::Chain *>( p_molecule.getChain( chainModelId ) );
			}
			currentChainResidueCount++;

			// Setup residue.
			Component::Chemistry::Residue * modelResidue
				= static_cast<Component::Chemistry::Residue *>( p_molecule.getResidue( residueIdx ) );

			modelResidue->setChainPtr( modelChain );
			modelResidue->setIndexFirstAtom( p_chemfilesReader.getCurrentResidueFirstAtomIndex() );
			modelResidue->setAtomCount( p_chemfilesReader.getCurrentResidueAtomCount() );
			std::transform( residueSymbol.begin(),
							residueSymbol.end(),
							residueSymbol.begin(),
							[]( unsigned char c ) { return std::toupper( c ); } );
			const std::optional symbol = magic_enum::enum_cast<ChemDB::Residue::SYMBOL>( residueSymbol );

			int symbolValue;

			if ( symbol.has_value() )
			{
				symbolValue = int( symbol.value() );
			}
			else
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

			modelResidue->setSymbol( ChemDB::Residue::SYMBOL( symbolValue ) );

			const bool isStandard = p_chemfilesReader.getCurrentResidueBoolProperty( "is_standard_pdb", true );
			modelResidue->setType( isStandard ? ChemDB::Residue::TYPE::STANDARD : ChemDB::Residue::TYPE::NON_STANDARD );
			modelResidue->setColor( ChemDB::Residue::getResidueColor( modelResidue->getSymbol(), isStandard ) );

			// Check residue index in chain.
			const size_t indexInChain = p_chemfilesReader.getCurrentResidueId();
			// assert( oldIndexInChain <= indexInChain );
			modelResidue->setIndexInOriginalChain( indexInChain );

			const std::string insertionCodeStr
				= p_chemfilesReader.getCurrentResidueStringProperty( "insertion_code", " " );
			modelResidue->setInsertionCode( insertionCodeStr[ 0 ] );

			oldIndexInChain = indexInChain;

			mapResidueBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );
			mapResidueExtraBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );

			// PDB only.
			// TODO: modify chemfiles to load handedness!
			if ( path.extension() == "pdb" )
			{
				const std::string secondaryStructure
					= p_chemfilesReader.getCurrentResidueStringProperty( "secondary_structure", "" );
				// VTX_DEBUG( secondaryStructure );

				if ( secondaryStructure != "" )
				{
					if ( secondaryStructure == "extended" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::STRAND );
					}
					else if ( secondaryStructure == "turn" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::TURN );
					}
					else if ( secondaryStructure == "coil" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::COIL );
					}
					else if ( secondaryStructure == "right-handed alpha helix" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT );
					}
					else if ( secondaryStructure == "left-handed alpha helix" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT );
					}
					else if ( secondaryStructure == "right-handed 3-10 helix" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_3_10_RIGHT );
					}
					else if ( secondaryStructure == "left-handed 3-10 helix" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_3_10_LEFT );
					}
					else if ( secondaryStructure == "pi helix" )
					{
						modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_PI );
					}
					else if ( secondaryStructure == "right-handed omega helix" )
					{
						// ?
					}
					else if ( secondaryStructure == "left-handed omega helix" )
					{
						// ?
					}
					else if ( secondaryStructure == "right-handed gamma helix" )
					{
						// ?
					}
					else if ( secondaryStructure == "left-handed gamma helix" )
					{
						// ?
					}

					else if ( secondaryStructure == "2 - 7 ribbon / helix" )
					{
						// ?
					}
					else if ( secondaryStructure == "polyproline" )
					{
						// ?
					}

					if ( _metaData.isSecondaryStructureLoadedFromFile == false )
					{
						_metaData.isSecondaryStructureLoadedFromFile = true;
					}
				}
			}

			size_t solventCounter = 0;
			size_t ionCounter	  = 0;

			if ( p_chemfilesReader.getCurrentResidueAtomCount() == 0 )
			{
				VTX_WARNING( "Empty residue found" );
			}

			for ( VTX::IO::Reader::Chemfiles::ResidueIt it = p_chemfilesReader.getCurrentResidueAtomIteratorBegin();
				  it != p_chemfilesReader.getCurrentResidueAtomIteratorEnd();
				  it++ )
			{
				const size_t atomId = *it;
				p_chemfilesReader.setCurrentAtom( atomId );

				// Create atom.
				Component::Chemistry::Atom * modelAtom
					= static_cast<Component::Chemistry::Atom *>( p_molecule.getAtom( atomId ) );

				modelAtom->setName( p_chemfilesReader.getCurrentAtomName() );
				modelAtom->setSymbol( p_chemfilesReader.getCurrentAtomSymbol() );
				modelFrame[ atomId ] = p_chemfilesReader.getCurrentAtomPosition();

				int atomType = int( p_chemfilesReader.getCurrentAtomType() );

				// Check PRM.
				// TODO: look for a better way to do this.
				if ( atomType != -1 )
				{
					if ( std::find( _metaData.solventAtomIds.begin(), _metaData.solventAtomIds.end(), atomType )
						 != _metaData.solventAtomIds.end() )
					{
						solventCounter++;
						modelAtom->setType( ChemDB::Atom::TYPE::SOLVENT );
					}
					else if ( std::find( _metaData.ionAtomIds.begin(), _metaData.ionAtomIds.end(), atomType )
							  != _metaData.ionAtomIds.end() )
					{
						ionCounter++;
						modelAtom->setType( ChemDB::Atom::TYPE::ION );
					}
				}

				// Check PSF.
				if ( std::find(
						 _metaData.solventResidueSymbols.begin(), _metaData.solventResidueSymbols.end(), residueSymbol )
					 != _metaData.solventResidueSymbols.end() )
				{
					solventCounter++;
					modelAtom->setType( ChemDB::Atom::TYPE::SOLVENT );
				}
				else if ( std::find(
							  _metaData.ionResidueSymbols.begin(), _metaData.ionResidueSymbols.end(), residueSymbol )
						  != _metaData.ionResidueSymbols.end() )
				{
					ionCounter++;
					modelAtom->setType( ChemDB::Atom::TYPE::ION );
				}

				// Radius.
				modelAtom->updateData();
				// Id.
				// p_molecule.getBufferAtomIds()[ atomId ] = modelAtom->getId();
			}

			//// Check residue full of solvent/ion.
			//// This is working only with pdb/psf files,
			//// not with arc/prm because arc do not contains topology.
			if ( solventCounter == p_chemfilesReader.getCurrentResidueAtomCount() )
			{
				// modelResidue->setAtomType( ChemDB::Atom::TYPE::SOLVENT );
			}
			else if ( ionCounter == p_chemfilesReader.getCurrentResidueAtomCount() )
			{
				// modelResidue->setAtomType( ChemDB::Atom::TYPE::ION );
			}
		}

		if ( currentChainResidueCount != 0 )
		{
			modelChain->setResidueCount( modelChain->getResidueCount() + 1 );
		}

		if ( p_chemfilesReader.getFrameCount() > 1 )
		{
			// TODO: launch the filling of trajectory frames in another thread
			// std::thread fillFrames(
			//	&MoleculeLoader::fillTrajectoryFrames, this, std::ref( trajectory ), std::ref( p_molecule ) );
			// fillFrames.detach();
			std::pair<App::Component::Chemistry::Molecule *, size_t> pairMoleculeFirstFrame = { &p_molecule, 1 };
			_readTrajectoryFrames( p_chemfilesReader, { pairMoleculeFirstFrame }, 1 );
		}

		VTX::Util::Chrono bondComputationChrono = VTX::Util::Chrono();
		if ( p_recomputeBonds )
		{
			bondComputationChrono.start();

			// Internal::Chemfiles::Util::recomputeBonds( frame, p_molecule.getAABB() );
			bondComputationChrono.stop();
			VTX_DEBUG( "recomputeBonds : {}", bondComputationChrono.elapsedTimeStr() );
		}

		// Bonds.
		// Sort by residus.
		// Map with residue index to keep the order.
		size_t counter = 0;
		for ( size_t bondIdx = 0; bondIdx < p_chemfilesReader.getBondCount(); ++bondIdx )
		{
			p_chemfilesReader.setCurrentBond( bondIdx );

			App::Component::Chemistry::Residue * residueStart = static_cast<Component::Chemistry::Residue *>(
				p_molecule.getAtom( p_chemfilesReader.getCurrentBondFirstAtomIndex() )->getResiduePtr() );
			App::Component::Chemistry::Residue * residueEnd = static_cast<Component::Chemistry::Residue *>(
				p_molecule.getAtom( p_chemfilesReader.getCurrentBondSecondAtomIndex() )->getResiduePtr() );

			if ( residueStart == residueEnd )
			{
				// Intra bonds.
				mapResidueBonds[ residueStart->getIndex() ].emplace_back( bondIdx );
				counter++;
			}
			else
			{
				// Extra bonds.
				mapResidueExtraBonds[ residueStart->getIndex() ].emplace_back( bondIdx );
				mapResidueExtraBonds[ residueEnd->getIndex() ].emplace_back( bondIdx );
				counter += 2;
			}
		}

		// Create models.
		p_molecule.initBonds( counter );
		// p_molecule.getBufferBonds().resize( counter * 2 );

		const size_t counterOld = counter;
		counter					= 0;
		for ( size_t residueIdx = 0; residueIdx < p_chemfilesReader.getResidueCount(); ++residueIdx )
		{
			Component::Chemistry::Residue * const residue
				= static_cast<Component::Chemistry::Residue *>( p_molecule.getResidue( residueIdx ) );
			const std::vector<size_t> & vectorBonds		 = mapResidueBonds[ residueIdx ];
			const std::vector<size_t> & vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

			residue->setIndexFirstBond( counter );
			residue->setBondCount( vectorBonds.size() + vectorExtraBonds.size() );

			for ( size_t i = 0; i < vectorBonds.size(); ++i, ++counter )
			{
				p_chemfilesReader.setCurrentBond( i );
				Component::Chemistry::Bond * modelBond
					= static_cast<Component::Chemistry::Bond *>( p_molecule.getBond( counter ) );
				modelBond->setIndex( i );

				modelBond->setIndexFirstAtom( p_chemfilesReader.getCurrentBondFirstAtomIndex() );
				modelBond->setIndexSecondAtom( p_chemfilesReader.getCurrentBondSecondAtomIndex() );
				modelBond->setOrder( p_chemfilesReader.getCurrentBondOrder() );

				// p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
				// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
			}

			for ( size_t i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
			{
				p_chemfilesReader.setCurrentBond( i );
				App::Component::Chemistry::Bond * modelBond
					= static_cast<Component::Chemistry::Bond *>( p_molecule.getBond( counter ) );

				modelBond->setMoleculePtr( &p_molecule );
				modelBond->setIndexFirstAtom( p_chemfilesReader.getCurrentBondFirstAtomIndex() );
				modelBond->setIndexSecondAtom( p_chemfilesReader.getCurrentBondSecondAtomIndex() );
				modelBond->setOrder( p_chemfilesReader.getCurrentBondOrder() );

				// p_molecule.getBufferBonds()[ counter * 2u ]		= uint( bond[ 0 ] );
				// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
			}
		}

		// bondComputationChrono.start();
		// const bool allBondsRecomputed = Util::App::Old::Molecule::recomputeBondOrdersFromFile( p_molecule );

		// if ( !allBondsRecomputed )
		//{
		//	VTX_INFO( "recomputeBondOrders with algorithm." );
		//	Util::App::Molecule::recomputeBondOrders( p_molecule );
		// }
		// bondComputationChrono.stop();
		// VTX_INFO( "recomputeBondOrders: " + bondComputationChrono.elapsedTimeStr() );

		assert( counter == counterOld );
	}

	// http://chemfiles.org/chemfiles/latest/formats.html#list-of-supported-formats
	const std::string MoleculeLoader::_getFormat( const FilePath & p_path )
	{
		std::string extension = p_path.extension().string();
		std::transform( extension.begin(), extension.end(), extension.begin(), tolower );
		if ( extension == ".nc" )
		{
			return "Amber NetCDF";
		}
		else if ( extension == ".cif" )
		{
			return "mmCIF"; // Workaround.
		}
		else if ( extension == ".cml" )
		{
			return "CML";
		}
		else if ( extension == ".cssr" )
		{
			return "CSSR";
		}
		else if ( extension == ".dcd" )
		{
			return "DCD";
		}
		else if ( extension == ".gro" )
		{
			return "GRO";
		}
		else if ( extension == ".lammpstrj" )
		{
			return "LAMMPS";
		}
		else if ( extension == ".mmcif" )
		{
			return "mmCIF";
		}
		else if ( extension == ".mmtf" )
		{
			return "MMTF";
		}
		else if ( extension == ".mol2" )
		{
			return "MOL2";
		}
		else if ( extension == ".molden" )
		{
			return "Molden";
		}
		else if ( extension == ".pdb" )
		{
			return "PDB";
		}
		else if ( extension == ".sdf" )
		{
			return "SDF";
		}
		else if ( extension == ".smi" )
		{
			return "SMI";
		}
		else if ( extension == ".arc" )
		{
			return "Tinker";
		}
		else if ( extension == ".tng" )
		{
			return "TNG";
		}
		else if ( extension == ".trj" )
		{
			return "TRJ";
		}
		else if ( extension == ".trr" )
		{
			return "TRR";
		}
		else if ( extension == ".xtc" )
		{
			return "XTC";
		}
		else if ( extension == ".xyz" )
		{
			return "XYZ";
		}
		else
		{
			assert( false );
			return "Unknown";
		}
	}

	const bool MoleculeLoader::_needToRecomputeBonds( const std::string & p_format ) const
	{
		return p_format == "mmCIF";
	}

	void MoleculeLoader::_warningCallback( const std::string & p_log ) {}

} // namespace VTX::App::Internal::IO::Reader
