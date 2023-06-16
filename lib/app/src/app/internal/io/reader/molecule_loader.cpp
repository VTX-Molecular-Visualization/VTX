#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/model/chemistry/atom.hpp"
#include "app/model/chemistry/bond.hpp"
#include "app/model/chemistry/chain.hpp"
#include "app/model/chemistry/molecule.hpp"
#include "app/model/chemistry/residue.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/core/worker/base_thread.hpp"
#include "app/old/internal/algorithm/bond_order_guessing.hpp"
#include "app/old/internal/chemfiles/util.hpp"
#include "app/old/util/molecule.hpp"
#include <algorithm>
#include <core/chemdb/category.hpp>
#include <core/chemdb/chain.hpp>
#include <core/new_struct/trajectory.hpp>
#include <iostream>
#include <magic_enum.hpp>
#include <thread>
#include <unordered_map>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Internal::IO::Reader
{
	namespace ChemDB = VTX::Core::ChemDB;

	void MoleculeLoader::readFile( const FilePath & p_path, App::Model::Chemistry::Molecule & p_molecule )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _warningCallback( p_log ); };
		chemfiles::set_warning_callback( callback );

		chemfiles::Trajectory trajectory	 = chemfiles::Trajectory( p_path.string(), 'r', _getFormat( p_path ) );
		const bool			  recomputeBonds = _needToRecomputeBonds( _getFormat( p_path ) );
		_readTrajectory( trajectory, p_path, p_molecule, recomputeBonds );

		chrono.stop();
		VTX_INFO( "readFile : {}", chrono.elapsedTimeStr() );
	}

	void MoleculeLoader::readBuffer( const std::string &			   p_buffer,
									 const FilePath &				   p_path,
									 App::Model::Chemistry::Molecule & p_molecule )
	{
		chemfiles::Trajectory trajectory
			= chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), _getFormat( p_path ) );
		_readTrajectory( trajectory, p_path, p_molecule );
	}

	std::vector<Vec3f> MoleculeLoader::readTrajectoryFrame( chemfiles::Trajectory & p_trajectory )
	{
		std::vector<Vec3f> res = std::vector<Vec3f>();

		const chemfiles::Frame					 frame	   = p_trajectory.read();
		const std::vector<chemfiles::Vector3D> & positions = frame.positions();

		res.resize( positions.size() );

		for ( size_t positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
		{
			const chemfiles::Vector3D & position = positions[ positionIdx ];
			res[ positionIdx ]					 = { position[ 0 ], position[ 1 ], position[ 2 ] };
		}

		return res;
	}
	void MoleculeLoader::fillTrajectoryFrame( App::Model::Chemistry::Molecule & p_molecule,
											  const size_t						p_moleculeFrameIndex,
											  const std::vector<Vec3f> &		p_atomPositions )
	{
		VTX::Core::NewStruct::Trajectory & moleculeTrajectory = p_molecule.getTrajectory();
		moleculeTrajectory.frames[ p_moleculeFrameIndex ].resize( p_atomPositions.size() );

		std::copy(
			p_atomPositions.begin(), p_atomPositions.end(), moleculeTrajectory.frames[ p_moleculeFrameIndex ].begin() );
	}

	void MoleculeLoader::_readTrajectoryFrames(
		chemfiles::Trajectory &													  p_trajectory,
		const std::vector<std::pair<App::Model::Chemistry::Molecule *, size_t>> & p_targets,
		const size_t															  p_trajectoryFrameStart )
	{
		// Fill other frames.
		Util::Chrono timeReadingFrames;
		timeReadingFrames.start();
		size_t startingFrame   = 1;
		size_t validFrameCount = 0;
		for ( size_t frameIdx = 0; frameIdx < p_trajectory.nsteps() - p_trajectoryFrameStart; ++frameIdx )
		{
			const std::vector<Vec3f> atomPositions = readTrajectoryFrame( p_trajectory );

			if ( atomPositions.size() <= 0 )
				continue;

			for ( const std::pair<App::Model::Chemistry::Molecule *, size_t> & pairMoleculeStartFrame : p_targets )
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
		for ( const std::pair<App::Model::Chemistry::Molecule *, size_t> & pairMoleculeFirstFrame : p_targets )
		{
			App::Model::Chemistry::Molecule &  molecule	  = *( pairMoleculeFirstFrame.first );
			VTX::Core::NewStruct::Trajectory & trajectory = molecule.getTrajectory();
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

	void MoleculeLoader::_readTrajectory( chemfiles::Trajectory &			p_trajectory,
										  const FilePath &					p_path,
										  App::Model::Chemistry::Molecule & p_molecule,
										  const bool						p_recomputeBonds )
	{
		VTX_INFO( "{} frames found.", p_trajectory.nsteps() );

		if ( p_trajectory.nsteps() == 0 )
		{
			throw IOException( "Trajectory is empty" );
		}

		// If opening a DCD file check if a topology file is present in the same folder.
		if ( p_path.extension() == "dcd" )
		{
			const std::string			   stem			  = p_path.stem().string();
			const std::vector<std::string> topExtensions  = { ".xyz", ".pdb", ".mol2" };
			std::string					   foundExtension = "";
			for ( size_t ext = 0; ext < topExtensions.size(); ext++ )
			{
				std::fstream topFile;
				topFile.open( stem + topExtensions[ ext ] );
				if ( topFile.is_open() )
				{
					topFile.close();
					foundExtension = topExtensions[ ext ];
					break;
				}
			}
			if ( foundExtension != "" )
			{
				chemfiles::Trajectory topolgy_file( stem + foundExtension, 'r' );
				p_trajectory.set_topology( stem + foundExtension );
			}
		}

		Util::Chrono chrono;
		chrono.start();
		chemfiles::Frame frame = p_trajectory.read();
		chrono.stop();

		VTX_INFO( "Trajectory read in: {}", chrono.elapsedTimeStr() );

		const chemfiles::Topology &				topology = frame.topology();
		const std::vector<chemfiles::Residue> & residues = topology.residues();
		const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();

		// Reset metadata
		_metaData	   = Component::IO::MoleculeMetadata();
		_metaData.path = p_path;
		// App::Component::IO::MoleculeConfiguration & config	 = p_molecule.getConfiguration();
		// p_molecule.setPath( p_path );

		if ( topology.residues().size() == 0 )
		{
			// If no residue, create a fake one.
			// TODO: check file format instead of residue count?
			VTX_INFO( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK", 0 );
			for ( size_t i = 0; i < frame.size(); ++i )
			{
				residue.add_atom( i );
			}
			frame.add_residue( residue );
		}

		if ( frame.size() != topology.size() )
		{
			throw IOException( "Data count missmatch" );
		}

		// Set molecule properties.
		if ( frame.get( "name" ) )
		{
			p_molecule.setName( frame.get( "name" )->as_string() );
		}
		if ( frame.get( "pdb_idcode" ) )
		{
			p_molecule.setPdbIdCode( frame.get( "pdb_idcode" )->as_string() );
		}
		// Initialized in Molecule model
		// p_molecule.setColor( Util::Color::Rgba::randomPastel() );

		// Check properties, same for all atoms/residues?
		if ( frame.size() > 0 )
		{
			std::string propAtom = std::to_string( frame[ 0 ].properties().size() ) + " properties in atoms:";
			for ( chemfiles::property_map::const_iterator it = frame[ 0 ].properties().begin();
				  it != frame[ 0 ].properties().end();
				  ++it )
			{
				propAtom += " " + it->first;
			}
			VTX_DEBUG( "{}", propAtom );
		}

		if ( residues.size() > 0 )
		{
			std::string propResidue = std::to_string( residues[ 0 ].properties().size() ) + " properties in residues:";
			for ( chemfiles::property_map::const_iterator it = residues[ 0 ].properties().begin();
				  it != residues[ 0 ].properties().end();
				  ++it )
			{
				propResidue += " " + it->first;
			}
			VTX_DEBUG( "{}", propResidue );
		}

		// If no residue, create a fake one.
		// TODO: check file format instead of residue count?
		bool hasTopology = true;
		if ( residues.size() == 0 )
		{
			hasTopology = false;
			VTX_INFO( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK" );
			for ( size_t i = 0; i < frame.size(); ++i )
			{
				residue.add_atom( i );
			}
			frame.add_residue( residue );
		}

		// Create models.
		p_molecule.getTrajectory().frames.resize( p_trajectory.nsteps() );
		p_molecule.initResidues( topology.residues().size() );
		p_molecule.initAtoms( frame.size() );
		// p_molecule.resizeBuffers();
		VTX::Core::NewStruct::Frame & modelFrame = p_molecule.getTrajectory().frames[ 0 ];
		modelFrame.resize( frame.size() );

		App::Model::Chemistry::Chain * modelChain				= nullptr;
		std::string					   lastChainName			= "";
		size_t						   chainModelId				= -1;
		size_t						   currentChainResidueCount = 0;

		std::map<size_t, std::vector<size_t>> mapResidueBonds	   = std::map<size_t, std::vector<size_t>>();
		std::map<size_t, std::vector<size_t>> mapResidueExtraBonds = std::map<size_t, std::vector<size_t>>();

		int								  oldIndexInChain  = INT_MIN;
		VTX::Core::ChemDB::Category::TYPE lastCategoryEnum = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;

		for ( size_t residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
		{
			const chemfiles::Residue & residue = residues[ residueIdx ];

			// Check if chain name changed.
			const std::string chainName		= residue.properties().get( "chainname" ).value_or( "" ).as_string();
			const std::string chainId		= residue.properties().get( "chainid" ).value_or( "" ).as_string();
			std::string		  residueSymbol = residue.name();

			const VTX::Core::ChemDB::Category::TYPE categoryEnum
				= Util::App::Old::Molecule::getResidueCategory( residueSymbol );

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
				modelChain = new Model::Chemistry::Chain( &p_molecule, ++chainModelId );
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
				oldIndexInChain	 = INT_MIN;
			}
			else
			{
				modelChain = p_molecule.getChain( chainModelId );
			}
			currentChainResidueCount++;

			// Setup residue.
			App::Model::Chemistry::Residue * modelResidue = p_molecule.getResidues()[ residueIdx ];

			modelResidue->setChainPtr( modelChain );
			modelResidue->setIndexFirstAtom( *residue.begin() );
			modelResidue->setAtomCount( residue.size() );
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

			const bool isStandard = residue.properties().get( "is_standard_pdb" ).value_or( true ).as_bool();
			modelResidue->setType( isStandard ? ChemDB::Residue::TYPE::STANDARD : ChemDB::Residue::TYPE::NON_STANDARD );
			modelResidue->setColor( ChemDB::Residue::getResidueColor( modelResidue->getSymbol(), isStandard ) );

			// Check residue index in chain.
			const int indexInChain = (int)residue.id().value_or( INT_MIN );
			// assert( oldIndexInChain <= indexInChain );
			modelResidue->setIndexInOriginalChain( indexInChain );

			const std::string insertionCodeStr
				= residue.properties().get( "insertion_code" ).value_or( " " ).as_string();
			modelResidue->setInsertionCode( insertionCodeStr[ 0 ] );

			oldIndexInChain = indexInChain;

			mapResidueBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );
			mapResidueExtraBonds.emplace( modelResidue->getIndex(), std::vector<size_t>() );

			// PDB only.
			// TODO: modify chemfiles to load handedness!
			if ( p_path.extension() == "pdb" )
			{
				std::string secondaryStructure
					= residue.properties().get( "secondary_structure" ).value_or( "" ).as_string();
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

			if ( residue.size() == 0 )
			{
				VTX_WARNING( "Empty residue found" );
			}

			for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
			{
				const size_t			atomId = *it;
				const chemfiles::Atom & atom   = topology[ atomId ];
				int						atomType;
				atomType = int( atom.properties().get( "atom_type" ).value_or( -1 ).as_double() );

				// Create atom.
				App::Model::Chemistry::Atom * modelAtom = p_molecule.getAtoms()[ atomId ];
				modelAtom->setResiduePtr( modelResidue );
				modelAtom->setName( atom.name() );
				std::string atomSymbol = atom.type();
				std::transform( atomSymbol.begin(),
								atomSymbol.end(),
								atomSymbol.begin(),
								[]( unsigned char c ) { return std::toupper( c ); } );

				// VTX_INFO( atom.name() + " " + atom.type() );

				const std::optional symbol = magic_enum::enum_cast<ChemDB::Atom::SYMBOL>( "A_" + atomSymbol );

				// Manage unknown value in App::ChemDB::Residue and App::ChemDB::Atom
				//  symbol.has_value() ? modelAtom->setSymbol( symbol.value() )
				//				   : p_molecule.addUnknownAtomSymbol( atom.type() );
				if ( symbol.has_value() )
					modelAtom->setSymbol( symbol.value() );

				modelAtom->setName( atom.name() );

				const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
				const chemfiles::Vector3D &					 position  = positions[ atomId ];

				const Vec3f atomPosition = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
				modelFrame[ atomId ]	 = atomPosition;

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
			if ( solventCounter == residue.size() )
			{
				// modelResidue->setAtomType( ChemDB::Atom::TYPE::SOLVENT );
			}
			else if ( ionCounter == residue.size() )
			{
				// modelResidue->setAtomType( ChemDB::Atom::TYPE::ION );
			}
		}

		if ( currentChainResidueCount != 0 )
		{
			modelChain->setResidueCount( modelChain->getResidueCount() + 1 );
		}

		if ( p_trajectory.nsteps() > 1 )
		{
			// TODO: launch the filling of trajectory frames in another thread
			// std::thread fillFrames(
			//	&MoleculeLoader::fillTrajectoryFrames, this, std::ref( p_trajectory ), std::ref( p_molecule ) );
			// fillFrames.detach();
			std::pair<App::Model::Chemistry::Molecule *, size_t> pairMoleculeFirstFrame = { &p_molecule, 1 };
			_readTrajectoryFrames( p_trajectory, { pairMoleculeFirstFrame }, 1 );
		}

		Util::Chrono bondComputationChrono = Util::Chrono();
		if ( p_recomputeBonds )
		{
			bondComputationChrono.start();

			// Internal::Chemfiles::Util::recomputeBonds( frame, p_molecule.getAABB() );
			bondComputationChrono.stop();
			VTX_DEBUG( "recomputeBonds : {}", bondComputationChrono.elapsedTimeStr() );
		}

		if ( VTX::App::Old::Application::Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
		{
			bondComputationChrono.start();
			const bool allBondsRecomputed = Old::Internal::Chemfiles::Util::recomputeBondOrdersFromFile( frame );

			if ( !allBondsRecomputed )
			{
				VTX_DEBUG( "recomputeBondOrders with algorithm." );
				Old::Internal::Chemfiles::Util::recomputeBondOrders( frame );
			}

			bondComputationChrono.stop();
			VTX_DEBUG( "recomputeBondOrders : {}", bondComputationChrono.elapsedTimeStr() );
		}

		// Bonds.
		// Sort by residus.
		// Map with residue index to keep the order.
		size_t counter = 0;
		for ( size_t boundIdx = 0; boundIdx < bonds.size(); ++boundIdx )
		{
			const chemfiles::Bond & bond = bonds[ boundIdx ];

			App::Model::Chemistry::Residue * residueStart = p_molecule.getAtom( bond[ 0 ] )->getResiduePtr();
			App::Model::Chemistry::Residue * residueEnd	  = p_molecule.getAtom( bond[ 1 ] )->getResiduePtr();

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
		// p_molecule.getBufferBonds().resize( counter * 2 );

		const size_t counterOld = counter;
		counter					= 0;
		for ( size_t residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
		{
			App::Model::Chemistry::Residue * const residue			= p_molecule.getResidue( residueIdx );
			const std::vector<size_t> &			   vectorBonds		= mapResidueBonds[ residueIdx ];
			const std::vector<size_t> &			   vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

			residue->setIndexFirstBond( counter );
			residue->setBondCount( vectorBonds.size() + vectorExtraBonds.size() );

			for ( size_t i = 0; i < vectorBonds.size(); ++i, ++counter )
			{
				const chemfiles::Bond &		  bond		= topology.bonds()[ vectorBonds[ i ] ];
				App::Model::Chemistry::Bond * modelBond = p_molecule.getBonds()[ counter ];
				modelBond->setIndex( i );

				modelBond->setIndexFirstAtom( bond[ 0 ] );
				modelBond->setIndexSecondAtom( bond[ 1 ] );

				const chemfiles::Bond::BondOrder bondOrder	= topology.bond_orders()[ vectorBonds[ i ] ];
				const ChemDB::Bond::ORDER		 modelOrder = ChemDB::Bond::ORDER( int( bondOrder ) );
				modelBond->setOrder( modelOrder );

				// p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
				// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
			}

			for ( size_t i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
			{
				const chemfiles::Bond &		  bond		= topology.bonds()[ vectorExtraBonds[ i ] ];
				App::Model::Chemistry::Bond * modelBond = p_molecule.getBonds()[ counter ];

				modelBond->setMoleculePtr( &p_molecule );
				modelBond->setIndexFirstAtom( bond[ 0 ] );
				modelBond->setIndexSecondAtom( bond[ 1 ] );

				const chemfiles::Bond::BondOrder bondOrder		= topology.bond_orders()[ vectorExtraBonds[ i ] ];
				const ChemDB::Bond::ORDER		 modelBondOrder = ChemDB::Bond::ORDER( int( bondOrder ) );
				modelBond->setOrder( modelBondOrder );

				// p_molecule.getBufferBonds()[ counter * 2u ]		= uint( bond[ 0 ] );
				// p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
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
