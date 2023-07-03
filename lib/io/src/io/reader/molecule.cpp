// #include "io/reader/molecule.hpp"
//// #include "app/old/application/setting.hpp"
//// #include "app/old/internal/algorithm/bond_order_guessing.hpp"
//// #include "app/old/internal/chemfiles/util.hpp"
//// #include "app/old/util/molecule.hpp"
// #include <algorithm>
// #include <core/base_struct/atom.hpp>
// #include <core/base_struct/bond.hpp>
// #include <core/base_struct/chain.hpp>
// #include <core/base_struct/molecule.hpp>
// #include <core/base_struct/residue.hpp>
// #include <core/base_struct/trajectory.hpp>
// #include <core/chemdb/category.hpp>
// #include <core/chemdb/chain.hpp>
// #include <iostream>
// #include <magic_enum.hpp>
//// #include <thread>
// #include <fstream>
// #include <unordered_map>
// #include <util/chrono.hpp>
// #include <util/color/rgba.hpp>
// #include <util/exceptions.hpp>
// #include <util/logger.hpp>

namespace VTX::IO::Reader
{
	//	namespace ChemDB = VTX::Core::ChemDB;
	//
	//	Molecule::ChemfileTrajectoryInfo::ChemfileTrajectoryInfo( const chemfiles::Trajectory & p_trajectory,
	//															  const chemfiles::Frame &		p_frame,
	//															  const FilePath &				p_filePath ) :
	//		trajectory( p_trajectory ),
	//		frame( p_frame ), filePath( p_filePath ), topology( p_frame.topology() ), residues( topology.residues() ),
	//		bonds( topology.bonds() )
	//	{
	//	}
	//
	//	void Molecule::readFile( const FilePath & p_path, Struct::Molecule & p_molecule )
	//	{
	//		Util::Chrono chrono = Util::Chrono();
	//		chrono.start();
	//
	//		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _warningCallback( p_log ); };
	//		chemfiles::set_warning_callback( callback );
	//
	//		chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string(), 'r', _getFormat( p_path ) );
	//		_readTrajectory( trajectory, p_path, p_molecule );
	//
	//		chrono.stop();
	//		VTX_INFO( "readFile : {}", chrono.elapsedTimeStr() );
	//	}
	//	void Molecule::readBuffer( const std::string & p_buffer, const FilePath & p_path, Struct::Molecule & p_molecule
	//)
	//	{
	//		chemfiles::Trajectory trajectory
	//			= chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), _getFormat( p_path ) );
	//		_readTrajectory( trajectory, p_path, p_molecule );
	//	}
	//	std::vector<Vec3f> Molecule::readTrajectoryFrame( chemfiles::Trajectory & p_trajectory )
	//	{
	//		std::vector<Vec3f> res = std::vector<Vec3f>();
	//
	//		const chemfiles::Frame					 frame	   = p_trajectory.read();
	//		const std::vector<chemfiles::Vector3D> & positions = frame.positions();
	//
	//		res.resize( positions.size() );
	//
	//		for ( size_t positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
	//		{
	//			const chemfiles::Vector3D & position = positions[ positionIdx ];
	//			res[ positionIdx ]					 = { position[ 0 ], position[ 1 ], position[ 2 ] };
	//		}
	//
	//		return res;
	//	}
	//
	//	void Molecule::fillTrajectoryFrame( Struct::Molecule &		   p_molecule,
	//										const size_t			   p_moleculeFrameIndex,
	//										const std::vector<Vec3f> & p_atomPositions )
	//	{
	//		Struct::Trajectory & moleculeTrajectory = p_molecule.getTrajectory();
	//		moleculeTrajectory.frames[ p_moleculeFrameIndex ].resize( p_atomPositions.size() );
	//
	//		std::copy(
	//			p_atomPositions.begin(), p_atomPositions.end(), moleculeTrajectory.frames[ p_moleculeFrameIndex
	//].begin()
	//);
	//	}
	//
	//	void Molecule::_readTrajectoryFrames( chemfiles::Trajectory &									 p_trajectory,
	//										  const std::vector<std::pair<Struct::Molecule *, size_t>> & p_targets,
	//										  const size_t p_trajectoryFrameStart )
	//	{
	//		// Fill other frames.
	//		Util::Chrono timeReadingFrames;
	//		timeReadingFrames.start();
	//		size_t startingFrame   = 1;
	//		size_t validFrameCount = 0;
	//		for ( size_t frameIdx = 0; frameIdx < p_trajectory.nsteps() - p_trajectoryFrameStart; ++frameIdx )
	//		{
	//			const std::vector<Vec3f> atomPositions = readTrajectoryFrame( p_trajectory );
	//
	//			if ( atomPositions.size() <= 0 )
	//				continue;
	//
	//			for ( const std::pair<Struct::Molecule *, size_t> & pairMoleculeStartFrame : p_targets )
	//			{
	//				fillTrajectoryFrame(
	//					*pairMoleculeStartFrame.first, pairMoleculeStartFrame.second + validFrameCount, atomPositions );
	//				validFrameCount++;
	//			}
	//
	// #ifdef _DEBUG
	//			if ( frameIdx > 1 && frameIdx % 100 == 0 )
	//			{
	//				VTX_DEBUG(
	//					"Frames from {} to {} read in: {}s.", startingFrame, frameIdx, timeReadingFrames.intervalTime()
	//); 				startingFrame = frameIdx;
	//			}
	// #endif // DEBUG
	//		}
	//		timeReadingFrames.stop();
	//		VTX_INFO( "Frames read in: {}s", timeReadingFrames.elapsedTime() );
	//
	//		// Erase supernumeraries frames
	//		for ( const std::pair<Struct::Molecule *, size_t> & pairMoleculeFirstFrame : p_targets )
	//		{
	//			Struct::Molecule &	 molecule	= *( pairMoleculeFirstFrame.first );
	//			Struct::Trajectory & trajectory = molecule.getTrajectory();
	//			if ( trajectory.frames.back().size() == 0 )
	//			{
	//				do
	//				{
	//					trajectory.frames.pop_back();
	//				} while ( trajectory.frames.back().size() == 0 );
	//
	//				trajectory.frames.shrink_to_fit();
	//			}
	//		}
	//	}
	//
	//	void Molecule::_readTrajectory( chemfiles::Trajectory & p_trajectory,
	//									const FilePath &		p_path,
	//									Struct::Molecule &		p_molecule )
	//	{
	//		if ( p_trajectory.nsteps() == 0 )
	//		{
	//			throw IOException( "Trajectory is empty" );
	//		}
	//
	//		VTX_INFO( "{} frames found.", p_trajectory.nsteps() );
	//
	//		Util::Chrono chrono;
	//
	//		chrono.start();
	//		_preRead( p_path, p_trajectory );
	//		chrono.stop();
	//		VTX_INFO( "_preRead: {}", chrono.elapsedTimeStr() );
	//
	//		chrono.start();
	//		chemfiles::Frame frame;
	//		_read( p_trajectory, frame );
	//		chrono.stop();
	//		VTX_INFO( "Trajectory read in: {}", chrono.elapsedTimeStr() );
	//
	//		chrono.start();
	//		_postRead( frame );
	//		chrono.stop();
	//		VTX_INFO( "_postRead: {}", chrono.elapsedTimeStr() );

	// const ChemfileTrajectoryInfo trajectoryInfo = ChemfileTrajectoryInfo( p_trajectory, frame, p_path );

	//_fillStructure( trajectoryInfo );

	// chrono.start();
	//_preFillStructure( p_molecule, trajectoryInfo );
	// chrono.stop();
	// VTX_INFO( "_preFillStructure: {}", chrono.elapsedTimeStr() );

	// chrono.start();
	//_fillStructure( p_molecule, trajectoryInfo );
	// chrono.stop();
	// VTX_INFO( "_fillStructure: {}", chrono.elapsedTimeStr() );

	// chrono.start();
	//_postFillStructure( p_molecule, trajectoryInfo );
	// chrono.stop();
	// VTX_INFO( "_postFillStructure: {}", chrono.elapsedTimeStr() );
} // namespace VTX::IO::Reader

// void Molecule::_preRead( const FilePath & p_path, chemfiles::Trajectory & p_trajectory )
//{
//	// If opening a DCD file check if a topology file is present in the same folder.
//	if ( p_path.extension() == "dcd" )
//	{
//		const std::string			   stem			  = p_path.stem().string();
//		const std::vector<std::string> topExtensions  = { ".xyz", ".pdb", ".mol2" };
//		std::string					   foundExtension = "";
//		for ( size_t ext = 0; ext < topExtensions.size(); ext++ )
//		{
//			std::fstream topFile;
//			topFile.open( stem + topExtensions[ ext ] );
//			if ( topFile.is_open() )
//			{
//				topFile.close();
//				foundExtension = topExtensions[ ext ];
//				break;
//			}
//		}
//		if ( foundExtension != "" )
//		{
//			chemfiles::Trajectory topolgy_file( stem + foundExtension, 'r' );
//			p_trajectory.set_topology( stem + foundExtension );
//		}
//	}
// }
// void Molecule::_read( chemfiles::Trajectory & p_trajectory, chemfiles::Frame & p_frame )
//{
//	p_frame = p_trajectory.read();
// }
// void Molecule::_postRead( chemfiles::Frame & p_frame )
//{
//	const chemfiles::Topology &				topology = p_frame.topology();
//	const std::vector<chemfiles::Residue> & residues = topology.residues();
//
//	if ( residues.size() == 0 )
//	{
//		// If no residue, create a fake one.
//		// TODO: check file format instead of residue count?
//		VTX_INFO( "No residues found" );
//		chemfiles::Residue residue = chemfiles::Residue( "UNK", 0 );
//		for ( size_t i = 0; i < p_frame.size(); ++i )
//		{
//			residue.add_atom( i );
//		}
//		p_frame.add_residue( residue );
//	}
//
//	if ( p_frame.size() != topology.size() )
//	{
//		throw IOException( "Data count missmatch" );
//	}
//
//	// Check properties, same for all atoms/residues?
//	if ( p_frame.size() > 0 )
//	{
//		std::string propAtom = std::to_string( p_frame[ 0 ].properties().size() ) + " properties in atoms:";
//		for ( chemfiles::property_map::const_iterator it = p_frame[ 0 ].properties().begin();
//			  it != p_frame[ 0 ].properties().end();
//			  ++it )
//		{
//			propAtom += " " + it->first;
//		}
//		VTX_DEBUG( "{}", propAtom );
//	}
//
//	if ( residues.size() > 0 )
//	{
//		std::string propResidue = std::to_string( residues[ 0 ].properties().size() ) + " properties in  residues :";
//
//		for ( chemfiles::property_map::const_iterator it = residues[ 0 ].properties().begin();
//			  it != residues[ 0 ].properties().end();
//			  ++it )
//		{
//			propResidue += " " + it->first;
//		}
//		VTX_DEBUG( "{}", propResidue );
//	}
// }

// void Molecule::_preFillStructure( Struct::Molecule & p_molecule, const ChemfileTrajectoryInfo & p_trajectoryInfo
// )
//{
//	// Reset metadata
//	_metaData = Metadata::Molecule();
//	// App::Component::IO::MoleculeConfiguration & config	 = p_molecule.getConfiguration();
// }
// void Molecule::_fillStructure( Struct::Molecule & p_molecule, const ChemfileTrajectoryInfo & p_trajectoryInfo )
//{
//	FillStructureInfo readInfo = FillStructureInfo();

//	_setMoleculeData( p_molecule, p_trajectoryInfo );

//	const std::vector<chemfiles::Residue> & residues = p_trajectoryInfo.residues;

//	for ( readInfo.currentResidueIndex = 0; readInfo.currentResidueIndex < residues.size();
//		  ++readInfo.currentResidueIndex )
//	{
//		const chemfiles::Residue & residue = residues[ readInfo.currentResidueIndex ];

//		// Check if chain name changed.
//		readInfo.currentChainName	  = residue.properties().get( "chainname" ).value_or( "" ).as_string();
//		readInfo.currentChainId		  = residue.properties().get( "chainid" ).value_or( "" ).as_string();
//		readInfo.currentResidueSymbol = residue.name();

//		const VTX::Core::ChemDB::Category::TYPE categoryEnum
//			= Util::App::Old::Molecule::getResidueCategory( readInfo.currentResidueSymbol );

//		const bool createNewChain = p_molecule.getChainCount() == 0 ||					   // No chain created
//									readInfo.currentChainName != readInfo.lastChainName || // New chain ID
//									categoryEnum != readInfo.lastCategoryEnum;			   // New category

//		if ( createNewChain )
//		{
//			_createNewChain( p_molecule, readInfo );
//			_setChainData( *( readInfo.modelChain ), p_trajectoryInfo, readInfo );

//			p_molecule.getCategory( categoryEnum ).referenceChain( readInfo.modelChain->getIndex() );

//			readInfo.currentChainResidueCount = 0;
//			readInfo.lastChainName			  = readInfo.currentChainName;
//			readInfo.lastCategoryEnum		  = categoryEnum;
//			readInfo.oldIndexInChain		  = INT_MIN;
//		}

//		readInfo.currentChainResidueCount++;

//		// Setup residue.
//		Struct::Residue * modelResidue = p_molecule.getResidues()[ readInfo.currentResidueIndex ];

//		_setResiudeData();
//		modelResidue->setChainPtr( readInfo.modelChain );
//		modelResidue->setIndexFirstAtom( *residue.begin() );
//		modelResidue->setAtomCount( residue.size() );
//		std::transform( readInfo.currentResidueSymbol.begin(),
//						readInfo.currentResidueSymbol.end(),
//						readInfo.currentResidueSymbol.begin(),
//						[]( unsigned char c ) { return std::toupper( c ); } );

//		const std::optional symbol
//			= magic_enum::enum_cast<ChemDB::Residue::SYMBOL>( readInfo.currentResidueSymbol );

//		int symbolValue;

//		if ( symbol.has_value() )
//		{
//			symbolValue = int( symbol.value() );
//		}
//		else
//		{
//			const int symbolIndex = 0;
//			// int symbolIndex = p_molecule.getUnknownResidueSymbolIndex( residueSymbol );
//			//  VTX::Core::ChemDB::UnknownResidueData * unknownResidueData;

//			// if ( symbolIndex >= 0 )
//			//{
//			//	unknownResidueData = p_molecule.getUnknownResidueSymbol( symbolIndex );
//			// }
//			// else
//			//{
//			//	unknownResidueData			   = new VTX::Core::ChemDB::UnknownResidueData();
//			//	unknownResidueData->symbolStr  = residueSymbol;
//			//	unknownResidueData->symbolName = Util::App::Molecule::getResidueFullName( residueSymbol
//			//);

//			//	symbolIndex = p_molecule.addUnknownResidueSymbol( unknownResidueData );
//			//}

//			symbolValue = int( ChemDB::Residue::SYMBOL::COUNT ) + symbolIndex;
//		}

//		modelResidue->setSymbol( ChemDB::Residue::SYMBOL( symbolValue ) );

//		const bool isStandard = residue.properties().get( "is_standard_pdb" ).value_or( true ).as_bool();
//		modelResidue->setType( isStandard ? ChemDB::Residue::TYPE::STANDARD : ChemDB::Residue::TYPE::NON_STANDARD );
//		modelResidue->setColor( ChemDB::Residue::getResidueColor( modelResidue->getSymbol(), isStandard ) );

//		// Check residue index in chain.
//		const int indexInChain = (int)residue.id().value_or( INT_MIN );
//		// assert( oldIndexInChain <= indexInChain );
//		modelResidue->setIndexInOriginalChain( indexInChain );

//		const std::string insertionCodeStr
//			= residue.properties().get( "insertion_code" ).value_or( " " ).as_string();
//		modelResidue->setInsertionCode( insertionCodeStr[ 0 ] );

//		oldIndexInChain = indexInChain;

//		mapResidueBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );
//		mapResidueExtraBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );

//		// PDB only.
//		// TODO: modify chemfiles to load handedness!
//		if ( p_path.extension() == "pdb" )
//		{
//			std::string secondaryStructure
//				= residue.properties().get( "secondary_structure" ).value_or( "" ).as_string();
//			// VTX_DEBUG( secondaryStructure );
//			if ( secondaryStructure != "" )
//			{
//				if ( secondaryStructure == "extended" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::STRAND );
//				}
//				else if ( secondaryStructure == "turn" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::TURN );
//				}
//				else if ( secondaryStructure == "coil" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::COIL );
//				}
//				else if ( secondaryStructure == "right-handed alpha helix" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT );
//				}
//				else if ( secondaryStructure == "left-handed alpha helix" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT );
//				}
//				else if ( secondaryStructure == "right-handed 3-10 helix" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_3_10_RIGHT );
//				}
//				else if ( secondaryStructure == "left-handed 3-10 helix" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_3_10_LEFT );
//				}
//				else if ( secondaryStructure == "pi helix" )
//				{
//					modelResidue->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_PI );
//				}
//				else if ( secondaryStructure == "right-handed omega helix" )
//				{
//					// ?
//				}
//				else if ( secondaryStructure == "left-handed omega helix" )
//				{
//					// ?
//				}
//				else if ( secondaryStructure == "right-handed gamma helix" )
//				{
//					// ?
//				}
//				else if ( secondaryStructure == "left-handed gamma helix" )
//				{
//					// ?
//				}

//				else if ( secondaryStructure == "2 - 7 ribbon / helix" )
//				{
//					// ?
//				}
//				else if ( secondaryStructure == "polyproline" )
//				{
//					// ?
//				}

//				if ( _metaData.isSecondaryStructureLoadedFromFile == false )
//				{
//					_metaData.isSecondaryStructureLoadedFromFile = true;
//				}
//			}
//		}

//		size_t solventCounter = 0;
//		size_t ionCounter	  = 0;

//		if ( residue.size() == 0 )
//		{
//			VTX_WARNING( "Empty residue found" );
//		}

//		for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
//		{
//			const size_t			atomId = *it;
//			const chemfiles::Atom & atom   = topology[ atomId ];
//			int						atomType;
//			atomType = int( atom.properties().get( "atom_type" ).value_or( -1 ).as_double() );

//			// Create atom.
//			Struct::Atom * modelAtom = p_molecule.getAtoms()[ atomId ];
//			modelAtom->setResiduePtr( modelResidue );
//			modelAtom->setName( atom.name() );
//			std::string atomSymbol = atom.type();
//			std::transform( atomSymbol.begin(),
//							atomSymbol.end(),
//							atomSymbol.begin(),
//							[]( unsigned char c ) { return std::toupper( c ); } );

//			// VTX_INFO( atom.name() + " " + atom.type() );

//			const std::optional symbol = magic_enum::enum_cast<ChemDB::Atom::SYMBOL>( "A_" + atomSymbol );

//			// Manage unknown value in App::ChemDB::Residue and App::ChemDB::Atom
//			//  symbol.has_value() ? modelAtom->setSymbol( symbol.value() )
//			//				   : p_molecule.addUnknownAtomSymbol( atom.type() );
//			if ( symbol.has_value() )
//				modelAtom->setSymbol( symbol.value() );

//			modelAtom->setName( atom.name() );

//			const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
//			const chemfiles::Vector3D &					 position  = positions[ atomId ];

//			const Vec3f atomPosition = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
//			modelFrame[ atomId ]	 = atomPosition;

//			// Check PRM.
//			// TODO: look for a better way to do this.
//			if ( atomType != -1 )
//			{
//				if ( std::find( _metaData.solventAtomIds.begin(), _metaData.solventAtomIds.end(), atomType )
//					 != _metaData.solventAtomIds.end() )
//				{
//					solventCounter++;
//					modelAtom->setType( ChemDB::Atom::TYPE::SOLVENT );
//				}
//				else if ( std::find( _metaData.ionAtomIds.begin(), _metaData.ionAtomIds.end(), atomType )
//						  != _metaData.ionAtomIds.end() )
//				{
//					ionCounter++;
//					modelAtom->setType( ChemDB::Atom::TYPE::ION );
//				}
//			}

//			// Check PSF.
//			if ( std::find(
//					 _metaData.solventResidueSymbols.begin(), _metaData.solventResidueSymbols.end(), residueSymbol )
//				 != _metaData.solventResidueSymbols.end() )
//			{
//				solventCounter++;
//				modelAtom->setType( ChemDB::Atom::TYPE::SOLVENT );
//			}
//			else if ( std::find(
//						  _metaData.ionResidueSymbols.begin(), _metaData.ionResidueSymbols.end(), residueSymbol )
//					  != _metaData.ionResidueSymbols.end() )
//			{
//				ionCounter++;
//				modelAtom->setType( ChemDB::Atom::TYPE::ION );
//			}

//			// Radius.
//			modelAtom->updateData();
//			// Id.
//			// p_molecule.getBufferAtomIds()[ atomId ] = modelAtom->getId();
//		}

//		//// Check residue full of solvent/ion.
//		//// This is working only with pdb/psf files,
//		//// not with arc/prm because arc do not contains topology.
//		if ( solventCounter == residue.size() )
//		{
//			// modelResidue->setAtomType( ChemDB::Atom::TYPE::SOLVENT );
//		}
//		else if ( ionCounter == residue.size() )
//		{
//			// modelResidue->setAtomType( ChemDB::Atom::TYPE::ION );
//		}
//	}

//	if ( currentChainResidueCount != 0 )
//	{
//		modelChain->setResidueCount( modelChain->getResidueCount() + 1 );
//	}

//	if ( p_trajectory.nsteps() > 1 )
//	{
//		// TODO: launch the filling of trajectory frames in another thread
//		// std::thread fillFrames(
//		//	&MoleculeLoader::fillTrajectoryFrames, this, std::ref( p_trajectory ), std::ref( p_molecule
//		//) );
//		// fillFrames.detach();
//		std::pair<Struct::Molecule *, size_t> pairMoleculeFirstFrame = { &p_molecule, 1 };
//		_readTrajectoryFrames( p_trajectory, { pairMoleculeFirstFrame }, 1 );
//	}

//	const bool recomputeBonds = _needToRecomputeBonds( _getFormat( p_path ) );

//	Util::Chrono bondComputationChrono = Util::Chrono();
//	if ( recomputeBonds )
//	{
//		bondComputationChrono.start();

//		// Internal::Chemfiles::Util::recomputeBonds( frame, p_molecule.getAABB() );
//		bondComputationChrono.stop();
//		VTX_DEBUG( "recomputeBonds : {}", bondComputationChrono.elapsedTimeStr() );
//	}

//	if ( VTX::App::Old::Application::Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
//	{
//		bondComputationChrono.start();
//		const bool allBondsRecomputed = Old::Internal::Chemfiles::Util::recomputeBondOrdersFromFile( frame );

//		if ( !allBondsRecomputed )
//		{
//			VTX_DEBUG( "recomputeBondOrders with algorithm." );
//			Old::Internal::Chemfiles::Util::recomputeBondOrders( frame );
//		}

//		bondComputationChrono.stop();
//		VTX_DEBUG( "recomputeBondOrders : {}", bondComputationChrono.elapsedTimeStr() );
//	}

//	// Bonds.
//	// Sort by residus.
//	// Map with residue index to keep the order.
//	size_t								 counter = 0;
//	const std::vector<chemfiles::Bond> & bonds	 = p_trajectoryInfo.bonds;
//	for ( size_t boundIdx = 0; boundIdx < bonds.size(); ++boundIdx )
//	{
//		const chemfiles::Bond & bond = bonds[ boundIdx ];

//		Struct::Residue * residueStart = p_molecule.getAtom( bond[ 0 ] )->getResiduePtr();
//		Struct::Residue * residueEnd   = p_molecule.getAtom( bond[ 1 ] )->getResiduePtr();

//		if ( residueStart == residueEnd )
//		{
//			// Intra bonds.
//			mapResidueBonds[ residueStart->getIndex() ].emplace_back( boundIdx );
//			counter++;
//		}
//		else
//		{
//			// Extra bonds.
//			mapResidueExtraBonds[ residueStart->getIndex() ].emplace_back( boundIdx );
//			mapResidueExtraBonds[ residueEnd->getIndex() ].emplace_back( boundIdx );
//			counter += 2;
//		}
//	}

//	// Create models.
//	p_molecule.initBonds( counter );
//	// p_molecule.getBufferBonds().resize( counter * 2 );

//	const size_t counterOld = counter;
//	counter					= 0;
//	for ( size_t residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
//	{
//		Struct::Residue * const		residue			 = p_molecule.getResidue( residueIdx );
//		const std::vector<size_t> & vectorBonds		 = mapResidueBonds[ residueIdx ];
//		const std::vector<size_t> & vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

//		residue->setIndexFirstBond( counter );
//		residue->setBondCount( vectorBonds.size() + vectorExtraBonds.size() );

//		for ( size_t i = 0; i < vectorBonds.size(); ++i, ++counter )
//		{
//			const chemfiles::Bond & bond	  = topology.bonds()[ vectorBonds[ i ] ];
//			Struct::Bond *			modelBond = p_molecule.getBonds()[ counter ];
//			modelBond->setIndex( i );

//			modelBond->setIndexFirstAtom( bond[ 0 ] );
//			modelBond->setIndexSecondAtom( bond[ 1 ] );

//			const chemfiles::Bond::BondOrder bondOrder	= topology.bond_orders()[ vectorBonds[ i ] ];
//			const ChemDB::Bond::ORDER		 modelOrder = ChemDB::Bond::ORDER( int( bondOrder ) );
//			modelBond->setOrder( modelOrder );

//			// p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
//			// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
//		}

//		for ( size_t i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
//		{
//			const chemfiles::Bond & bond	  = topology.bonds()[ vectorExtraBonds[ i ] ];
//			Struct::Bond *			modelBond = p_molecule.getBonds()[ counter ];

//			modelBond->setMoleculePtr( &p_molecule );
//			modelBond->setIndexFirstAtom( bond[ 0 ] );
//			modelBond->setIndexSecondAtom( bond[ 1 ] );

//			const chemfiles::Bond::BondOrder bondOrder		= topology.bond_orders()[ vectorExtraBonds[ i ] ];
//			const ChemDB::Bond::ORDER		 modelBondOrder = ChemDB::Bond::ORDER( int( bondOrder ) );
//			modelBond->setOrder( modelBondOrder );

//			// p_molecule.getBufferBonds()[ counter * 2u ]		= uint( bond[ 0 ] );
//			// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
//		}
//	}

//	assert( counter == counterOld );
//}
// void Molecule::_postFillStructure( Struct::Molecule & p_molecule, const ChemfileTrajectoryInfo & p_trajectoryInfo
// )
//{
//}

// void Molecule::_setMoleculeData( Struct::Molecule & p_molecule, const ChemfileTrajectoryInfo & p_trajectoryInfo )
//{
//	const chemfiles::Trajectory & trajectory = p_trajectoryInfo.trajectory;
//	const chemfiles::Frame &	  frame		 = p_trajectoryInfo.frame;
//	const chemfiles::Topology &	  topology	 = p_trajectoryInfo.topology;

//	_metaData.path = p_trajectoryInfo.filePath;
//	// p_molecule.setPath( p_path );

//	// Set molecule properties.
//	if ( frame.get( "name" ) )
//	{
//		_metaData.name = frame.get( "name" )->as_string();
//	}
//	if ( frame.get( "pdb_idcode" ) )
//	{
//		_metaData.pdbIDCode = frame.get( "pdb_idcode" )->as_string();
//	}
//	// Initialized in Molecule model
//	// p_molecule.setColor( Util::Color::Rgba::randomPastel() );

//	// Create models.
//	p_molecule.getTrajectory().frames.resize( trajectory.nsteps() );
//	p_molecule.initResidues( topology.residues().size() );
//	p_molecule.initAtoms( frame.size() );
//	// p_molecule.resizeBuffers();
//	Struct::Frame & modelFrame = p_molecule.getTrajectory().frames[ 0 ];
//	modelFrame.resize( frame.size() );
//}

// void Molecule::_createNewChain( Struct::Molecule & p_molecule, FillStructureInfo & p_readInfo )
//{
//	// modelChain = new Component::Chemistry::Chain( &p_molecule, ++chainModelId );
//	// p_molecule.getChains().emplace_back( modelChain );

//	if ( p_readInfo.modelChain != nullptr )
//	{
//		p_readInfo.modelChain->setResidueCount( p_readInfo.currentChainResidueCount );
//	}

//	p_readInfo.modelChain = p_molecule.appendNewChain();
//}
// void Molecule::_setChainData( Struct::Chain & p_modelChain,
//							  const ChemfileTrajectoryInfo &,
//							  const FillStructureInfo & p_readInfo )
//{
//	p_modelChain.setName( p_readInfo.currentChainName );
//	p_modelChain.setIndexFirstResidue( p_readInfo.currentResidueIndex );

//	p_modelChain.setOriginalChainID( p_readInfo.currentChainId );
//	p_modelChain.setColor( ChemDB::Chain::getChainIdColor( p_readInfo.currentChainId ) );
//	// modelChain->setCategoryEnum( categoryEnum );
//}

// const bool Molecule::_needToRecomputeBonds( const std::string & p_format ) const { return p_format == "mmCIF"; }
// const bool Molecule::_checkTrajectoryFiles( const std::string & p_format ) const { return p_format == "dcd"; }

// void Molecule::_warningCallback( const std::string & p_log ) {}

//} // namespace VTX::IO::Reader
