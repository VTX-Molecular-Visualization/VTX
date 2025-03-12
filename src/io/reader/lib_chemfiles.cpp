#include "lib_chemfiles.hpp"
#include "color/rgba.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/category.hpp"
#include "model/category_enum.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "util/bond_guessing/bond_order_guessing.hpp"
#include "util/bond_guessing/bond_recomputation.hpp"
#include "util/chemfiles.hpp"
#include "util/molecule.hpp"
#include "worker/base_thread.hpp"
#include <QDir>
#include <QFileInfo>
#include <algorithm>
#include <iostream>
#include <magic_enum.hpp>
#include <thread>
#include <unordered_map>

namespace VTX::IO::Reader
{
	LibChemfiles::LibChemfiles( const Worker::BaseThread * const p_loader ) : ChemfilesIO( p_loader ) {}

	void LibChemfiles::readFile( const IO::FilePath & p_path, Model::Molecule & p_molecule )
	{
		_prepareChemfiles();
		chemfiles::Trajectory trajectory	 = chemfiles::Trajectory( p_path.path(), 'r', _getFormat( p_path ) );
		const bool			  recomputeBonds = _needToRecomputeBonds( _getFormat( p_path ) );
		_readTrajectory( trajectory, p_path, p_molecule, recomputeBonds );
	}

	void LibChemfiles::readBuffer( const std::string &	p_buffer,
								   const IO::FilePath & p_path,
								   Model::Molecule &	p_molecule )
	{
		_prepareChemfiles();
		chemfiles::Trajectory trajectory
			= chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), _getFormat( p_path ) );
		_readTrajectory( trajectory, p_path, p_molecule );
	}

	bool LibChemfiles::readDynamic( const IO::FilePath & p_path, std::vector<Model::Molecule *> & p_potentialTargets )
	{
		_prepareChemfiles();
		chemfiles::Trajectory dynamicTrajectory = chemfiles::Trajectory( p_path.path(), 'r', _getFormat( p_path ) );
		return _tryApplyingDynamicOnTargets( dynamicTrajectory, p_potentialTargets );
	}

	std::vector<Vec3f> LibChemfiles::readTrajectoryFrame( chemfiles::Trajectory & p_trajectory ) const
	{
		std::vector<Vec3f> res = std::vector<Vec3f>();

		const chemfiles::Frame					 frame	   = p_trajectory.read();
		const std::vector<chemfiles::Vector3D> & positions = frame.positions();

		res.resize( positions.size() );

		for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
		{
			const chemfiles::Vector3D & position = positions[ positionIdx ];
			res[ positionIdx ]					 = { position[ 0 ], position[ 1 ], position[ 2 ] };
		}

		return res;
	}
	void LibChemfiles::fillTrajectoryFrame( Model::Molecule &		   p_molecule,
											const uint				   p_moleculeFrameIndex,
											const std::vector<Vec3f> & p_atomPositions ) const
	{
		if ( _isThreadInterrupted() )
			return;

		Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( p_moleculeFrameIndex );
		const size_t atomCount = Util::Math::min( moleculeFrame.size(), p_atomPositions.size() );

		for ( size_t i = 0; i < atomCount; i++ )
		{
			moleculeFrame[ i ] = p_atomPositions[ i ];
		}
	}

	void LibChemfiles::_readTrajectoryFrames( chemfiles::Trajectory &								  p_trajectory,
											  const std::vector<std::pair<Model::Molecule *, uint>> & p_targets,
											  const uint p_trajectoryFrameStart ) const
	{
		// Fill other frames.
		Tool::Chrono timeReadingFrames;
		timeReadingFrames.start();
		int startingFrame	= 1;
		int validFrameCount = 0;
		for ( uint frameIdx = 0; frameIdx < p_trajectory.nsteps() - p_trajectoryFrameStart; ++frameIdx )
		{
			const std::vector<Vec3f> atomPositions = readTrajectoryFrame( p_trajectory );

			if ( atomPositions.size() <= 0 )
				continue;

			for ( const std::pair<Model::Molecule *, uint> & pairMoleculeStartFrame : p_targets )
			{
				if ( _isThreadInterrupted() )
					return;
				fillTrajectoryFrame(
					*pairMoleculeStartFrame.first, pairMoleculeStartFrame.second + validFrameCount, atomPositions );
			}

			validFrameCount++;
			_logDebug( "Frame " + std::to_string( validFrameCount ) + " done." );

			if ( _isThreadInterrupted() )
				return;

#ifdef _DEBUG
			if ( frameIdx > 1 && frameIdx % 100 == 0 )
			{
				_logDebug( "Frames from " + std::to_string( startingFrame ) + " to " + std::to_string( frameIdx )
						   + " read in: " + std::to_string( timeReadingFrames.intervalTime() ) + "s" );
				startingFrame = frameIdx;
			}
#endif // DEBUG
		}
		timeReadingFrames.stop();
		_logInfo( "Frames read in: " + std::to_string( timeReadingFrames.elapsedTime() ) + "s" );

		if ( _isThreadInterrupted() )
			return;

		// Erase supernumeraries frames
		for ( const std::pair<Model::Molecule *, uint> & pairMoleculeFirstFrame : p_targets )
		{
			if ( _isThreadInterrupted() )
				return;

			Model::Molecule & molecule = *( pairMoleculeFirstFrame.first );
			if ( molecule.getFrames().back().size() == 0 )
			{
				do
				{
					molecule.getFrames().pop_back();
					if ( _isThreadInterrupted() )
						return;

				} while ( molecule.getFrames().back().size() == 0 );

				if ( _isThreadInterrupted() )
					return;

				molecule.getFrames().shrink_to_fit();
			}
		}
	}

	void LibChemfiles::_readTrajectory( chemfiles::Trajectory & p_trajectory,
										const IO::FilePath &	p_path,
										Model::Molecule &		p_molecule,
										const bool				p_recomputeBonds ) const
	{
		_logInfo( std::to_string( p_trajectory.nsteps() ) + " frames found" );

		if ( p_trajectory.nsteps() == 0 )
		{
			throw Exception::IOException( "Trajectory is empty" );
		}

		// if opening a DCD file check if a topology file is present in the same folder
		QFileInfo fileInfo( p_path.qpath() );
		if ( fileInfo.suffix().toStdString() == "dcd" )
		{
			const std::string filePathWithoutExt
				= QString( fileInfo.path() + QDir::separator() + fileInfo.baseName() ).toStdString();
			const std::vector<std::string> topExtensions  = { ".xyz", ".pdb", ".mol2" };
			std::string					   foundExtension = "";
			for ( size_t ext = 0; ext < topExtensions.size(); ext++ )
			{
				std::fstream topFile;
				topFile.open( filePathWithoutExt + topExtensions[ ext ] );
				if ( topFile.is_open() )
				{
					topFile.close();
					foundExtension = topExtensions[ ext ];
					break;
				}
			}
			if ( foundExtension != "" )
			{
				chemfiles::Trajectory topolgy_file( filePathWithoutExt + foundExtension, 'r' );
				p_trajectory.set_topology( filePathWithoutExt + foundExtension );
			}
		}

		Tool::Chrono chrono;
		chrono.start();
		chemfiles::Frame frame = p_trajectory.read();
		chrono.stop();

		if ( _isThreadInterrupted() )
			return;

		_logInfo( "Trajectory read in: " + chrono.elapsedTimeStr() );

		const chemfiles::Topology &				topology = frame.topology();
		const std::vector<chemfiles::Residue> & residues = topology.residues();
		const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();
		Model::Configuration::Molecule &		config	 = p_molecule.getConfiguration();
		p_molecule.setPath( p_path );

		if ( topology.residues().size() == 0 )
		{
			// If no residue, create a fake one.
			// TODO: check file format instead of residue count?
			_logInfo( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK", 0 );
			for ( uint i = 0; i < frame.size(); ++i )
			{
				residue.add_atom( i );
			}
			frame.add_residue( residue );
		}

		if ( frame.size() != topology.size() )
		{
			throw Exception::IOException( "Data count missmatch" );
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
		p_molecule.setColor( Color::Rgba::randomPastel() );

		// Check properties, same for all atoms/residues?
		if ( frame.size() > 0 )
		{
			if ( frame[ 0 ].properties() )
			{
				std::string propAtom = std::to_string( frame[ 0 ].properties()->size() ) + " properties in atoms:";
				for ( chemfiles::property_map::const_iterator it = frame[ 0 ].properties()->begin();
					  it != frame[ 0 ].properties()->end();
					  ++it )
				{
					propAtom += " " + it->first;
				}
				_logDebug( propAtom );
			}
			else
			{
				_logDebug( "No properties in atoms." );
			}
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
			_logDebug( propResidue );
		}

		// If no residue, create a fake one.
		// TODO: check file format instead of residue count?
		bool hasTopology = true;
		if ( residues.size() == 0 )
		{
			hasTopology = false;
			_logInfo( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK" );
			for ( uint i = 0; i < frame.size(); ++i )
			{
				residue.add_atom( i );
			}
			frame.add_residue( residue );
		}

		// Create models.
		p_molecule.getAtoms().resize( frame.size() );
		p_molecule.getResidues().resize( topology.residues().size() );
		p_molecule.resizeAtomPositionFrames( p_trajectory.nsteps() );

		p_molecule.resizeBuffers();
		Model::Molecule::AtomPositionsFrame & modelFrame = p_molecule.getAtomPositionFrame( 0 );
		modelFrame.resize( frame.size() );

		Model::Chain * modelChain;
		std::string	   lastChainName = "";
		uint		   chainModelId	 = -1;

		std::map<uint, std::vector<size_t>> mapResidueBonds		 = std::map<uint, std::vector<size_t>>();
		std::map<uint, std::vector<size_t>> mapResidueExtraBonds = std::map<uint, std::vector<size_t>>();

		int			  oldIndexInChain  = INT_MIN;
		CATEGORY_ENUM lastCategoryEnum = CATEGORY_ENUM::UNKNOWN;

		for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
		{
			if ( _isThreadInterrupted() )
				return;

			const chemfiles::Residue & residue = residues[ residueIdx ];

			// Check if chain name changed.
			const std::string chainName		= residue.properties().get( "chainname" ).value_or( "" ).as_string();
			const std::string chainId		= residue.properties().get( "chainid" ).value_or( "" ).as_string();
			std::string		  residueSymbol = residue.name();

			const CATEGORY_ENUM categoryEnum = Util::Molecule::getResidueCategory( residueSymbol );

			const bool createNewChain = p_molecule.getChainCount() == 0 || // No chain created
										chainName != lastChainName ||	   // New chain ID
										categoryEnum != lastCategoryEnum;  // New category

			if ( createNewChain )
			{
				// Create chain.
				modelChain = &p_molecule.addChain();
				chainModelId++;
				modelChain->setIndex( chainModelId );
				if ( chainName != "" )
				{
					modelChain->setName( chainName );
				}
				modelChain->setMoleculePtr( &p_molecule );
				modelChain->setIndexFirstResidue( residueIdx );
				modelChain->setResidueCount( 0 );
				modelChain->setOriginalChainID( chainId );
				modelChain->setColor( Model::Chain::getChainIdColor( chainId ) );
				modelChain->setCategoryEnum( categoryEnum );

				lastChainName	 = chainName;
				lastCategoryEnum = categoryEnum;
				oldIndexInChain	 = INT_MIN;
			}

			modelChain = p_molecule.getChain( chainModelId );
			modelChain->setResidueCount( modelChain->getResidueCount() + 1 );

			// Create residue.
			Model::Residue * modelResidue		   = MVC::MvcManager::get().instantiateModel<Model::Residue>();
			p_molecule.getResidues()[ residueIdx ] = modelResidue;
			modelResidue->setIndex( residueIdx );

			modelResidue->setChainPtr( modelChain );
			modelResidue->setIndexFirstAtom( uint( *residue.begin() ) );
			modelResidue->setAtomCount( uint( residue.size() ) );
			std::transform( residueSymbol.begin(),
							residueSymbol.end(),
							residueSymbol.begin(),
							[]( unsigned char c ) { return std::toupper( c ); } );
			const std::optional symbol = magic_enum::enum_cast<Model::Residue::SYMBOL>( residueSymbol );

			int symbolValue;

			if ( symbol.has_value() )
			{
				symbolValue = int( symbol.value() );
			}
			else
			{
				int							symbolIndex = p_molecule.getUnknownResidueSymbolIndex( residueSymbol );
				Model::UnknownResidueData * unknownResidueData;

				if ( symbolIndex >= 0 )
				{
					unknownResidueData = p_molecule.getUnknownResidueSymbol( symbolIndex );
				}
				else
				{
					unknownResidueData			   = new Model::UnknownResidueData();
					unknownResidueData->symbolStr  = residueSymbol;
					unknownResidueData->symbolName = Util::Molecule::getResidueFullName( residueSymbol );

					symbolIndex = p_molecule.addUnknownResidueSymbol( unknownResidueData );
				}

				symbolValue = int( Model::Residue::SYMBOL::COUNT ) + symbolIndex;
			}

			modelResidue->setSymbol( symbolValue );

			modelResidue->setColor( Model::Residue::getResidueColor( *modelResidue ) );

			const bool isStandard = residue.properties().get( "is_standard_pdb" ).value_or( true ).as_bool();
			modelResidue->setType( isStandard ? Model::Residue::TYPE::STANDARD : Model::Residue::TYPE::NON_STANDARD );

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
				// _logDebug( secondaryStructure );
				if ( secondaryStructure != "" )
				{
					if ( secondaryStructure == "extended" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::STRAND );
					}
					else if ( secondaryStructure == "turn" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::TURN );
					}
					else if ( secondaryStructure == "coil" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::COIL );
					}
					else if ( secondaryStructure == "right-handed alpha helix" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT );
					}
					else if ( secondaryStructure == "left-handed alpha helix" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT );
					}
					else if ( secondaryStructure == "right-handed 3-10 helix" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::HELIX_3_10_RIGHT );
					}
					else if ( secondaryStructure == "left-handed 3-10 helix" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::HELIX_3_10_LEFT );
					}
					else if ( secondaryStructure == "pi helix" )
					{
						modelResidue->setSecondaryStructure( Model::SecondaryStructure::TYPE::HELIX_PI );
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

					if ( config.isSecondaryStructureLoadedFromFile == false )
					{
						config.isSecondaryStructureLoadedFromFile = true;
					}
				}
			}

			uint solventCounter = 0;
			uint ionCounter		= 0;

			if ( residue.size() == 0 )
			{
				_logWarning( "Empty residue found" );
			}

			for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
			{
				if ( _isThreadInterrupted() )
					return;

				const uint				atomId	 = uint( *it );
				const chemfiles::Atom & atom	 = topology[ atomId ];
				uint					atomType = -1;
				if ( atom.properties() )
				{
					atomType = uint( atom.properties()->get( "atom_type" ).value_or( -1 ).as_double() );
				}

				// Create atom.
				Model::Atom * modelAtom			= MVC::MvcManager::get().instantiateModel<Model::Atom>();
				p_molecule.getAtoms()[ atomId ] = modelAtom;
				modelAtom->setIndex( atomId );
				modelAtom->setResiduePtr( modelResidue );
				modelAtom->setName( atom.name() );
				std::string atomSymbol = atom.type();
				std::transform( atomSymbol.begin(),
								atomSymbol.end(),
								atomSymbol.begin(),
								[]( unsigned char c ) { return std::toupper( c ); } );

				// _logInfo( atom.name() + " " + atom.type() );

				std::optional symbol = magic_enum::enum_cast<Model::Atom::SYMBOL>( "A_" + atomSymbol );

				symbol.has_value() ? modelAtom->setSymbol( symbol.value() )
								   : p_molecule.addUnknownAtomSymbol( atom.type() );

				modelAtom->setName( atom.name() );
				// modelAtom->setColor( Model::Atom::SYMBOL_COLOR[ int( modelAtom->getSymbol() ) ] );

				const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
				const chemfiles::Vector3D &					 position  = positions[ atomId ];

				const Vec3f atomPosition = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
				modelFrame[ atomId ]	 = atomPosition;

				// Check PRM.
				// TODO: look for a better way to do this.
				if ( atomType != -1 )
				{
					if ( std::find( config.solventAtomIds.begin(), config.solventAtomIds.end(), atomType )
						 != config.solventAtomIds.end() )
					{
						solventCounter++;
						modelAtom->setType( Model::Atom::TYPE::SOLVENT );
					}
					else if ( std::find( config.ionAtomIds.begin(), config.ionAtomIds.end(), atomType )
							  != config.ionAtomIds.end() )
					{
						ionCounter++;
						modelAtom->setType( Model::Atom::TYPE::ION );
					}
				}

				// Check PSF.
				if ( std::find(
						 config.solventResidueSymbols.begin(), config.solventResidueSymbols.end(), residueSymbol )
					 != config.solventResidueSymbols.end() )
				{
					solventCounter++;
					modelAtom->setType( Model::Atom::TYPE::SOLVENT );
				}
				else if ( std::find( config.ionResidueSymbols.begin(), config.ionResidueSymbols.end(), residueSymbol )
						  != config.ionResidueSymbols.end() )
				{
					ionCounter++;
					modelAtom->setType( Model::Atom::TYPE::ION );
				}

				// Radius.
				p_molecule.getBufferAtomRadius()[ atomId ] = modelAtom->getVdwRadius();
				// Id.
				p_molecule.getBufferAtomIds()[ atomId ] = modelAtom->getId();
			}

			// Check residue full of solvent/ion.
			// This is working only with pdb/psf files,
			// not with arc/prm because arc do not contains topology.
			if ( solventCounter == residue.size() )
			{
				modelResidue->setAtomType( Model::Atom::TYPE::SOLVENT );
			}
			else if ( ionCounter == residue.size() )
			{
				modelResidue->setAtomType( Model::Atom::TYPE::ION );
			}
		}

		if ( p_trajectory.nsteps() > 1 )
		{
			// TODO: launch the filling of trajectory frames in another thread
			// std::thread fillFrames(
			//	&LibChemfiles::fillTrajectoryFrames, this, std::ref( p_trajectory ), std::ref( p_molecule ) );
			// fillFrames.detach();
			std::pair<Model::Molecule *, uint> pairMoleculeFirstFrame = { &p_molecule, 1 };
			_readTrajectoryFrames( p_trajectory, { pairMoleculeFirstFrame }, 1 );
		}

		Tool::Chrono bondComputationChrono = Tool::Chrono();
		if ( frame.size() > 0 && not frame.has_bond() )
		{
			if ( p_recomputeBonds || Util::BondGuessing::shouldRecomputeBonds( frame ) )
			{
				VTX_INFO( "Bond information not found if file. Recomputing atom bonding ... " );
				bondComputationChrono.start();
				Util::Chemfiles::recomputeBonds( frame, p_molecule.getAABB() );
				bondComputationChrono.stop();
				_logDebug( "recomputeBonds : " + bondComputationChrono.elapsedTimeStr() );
			}
		}

		if ( Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
		{
			bondComputationChrono.start();
			const bool allBondsRecomputed = Util::Chemfiles::recomputeBondOrdersFromFile( frame );

			if ( !allBondsRecomputed )
			{
				_logDebug( "recomputeBondOrders with algorithm." );
				Util::Chemfiles::recomputeBondOrders( frame );
			}

			bondComputationChrono.stop();
			_logDebug( "recomputeBondOrders : " + bondComputationChrono.elapsedTimeStr() );
		}

		// Bonds.
		// Sort by residus.
		// Map with residue index to keep the order.
		uint counter = 0;
		for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
		{
			if ( _isThreadInterrupted() )
				return;

			const chemfiles::Bond & bond = bonds[ boundIdx ];

			Model::Residue * residueStart = p_molecule.getAtom( uint( bond[ 0 ] ) )->getResiduePtr();
			Model::Residue * residueEnd	  = p_molecule.getAtom( uint( bond[ 1 ] ) )->getResiduePtr();

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
		p_molecule.getBonds().resize( counter );
		p_molecule.getBufferBonds().resize( counter * 2 );

		const uint counterOld = counter;
		counter				  = 0;
		for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
		{
			if ( _isThreadInterrupted() )
				return;

			Model::Residue * const		residue			 = p_molecule.getResidue( residueIdx );
			const std::vector<size_t> & vectorBonds		 = mapResidueBonds[ residueIdx ];
			const std::vector<size_t> & vectorExtraBonds = mapResidueExtraBonds[ residueIdx ];

			residue->setIndexFirstBond( counter );
			residue->setBondCount( uint( vectorBonds.size() ) + uint( vectorExtraBonds.size() ) );

			for ( uint i = 0; i < vectorBonds.size(); ++i, ++counter )
			{
				if ( _isThreadInterrupted() )
					return;

				const chemfiles::Bond & bond	  = topology.bonds()[ vectorBonds[ i ] ];
				Model::Bond *			modelBond = MVC::MvcManager::get().instantiateModel<Model::Bond>();
				p_molecule.getBonds()[ counter ]  = modelBond;

				modelBond->setMoleculePtr( &p_molecule );
				modelBond->setIndexFirstAtom( uint( bond[ 0 ] ) );
				modelBond->setIndexSecondAtom( uint( bond[ 1 ] ) );

				const chemfiles::Bond::BondOrder bondOrder	= topology.bond_orders()[ vectorBonds[ i ] ];
				const Model::Bond::ORDER		 modelOrder = Model::Bond::ORDER( int( bondOrder ) );
				modelBond->setOrder( modelOrder );

				p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
				p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
			}

			for ( uint i = 0; i < vectorExtraBonds.size(); ++i, ++counter )
			{
				if ( _isThreadInterrupted() )
					return;

				const chemfiles::Bond & bond	  = topology.bonds()[ vectorExtraBonds[ i ] ];
				Model::Bond *			modelBond = MVC::MvcManager::get().instantiateModel<Model::Bond>();
				p_molecule.getBonds()[ counter ]  = modelBond;

				modelBond->setMoleculePtr( &p_molecule );
				modelBond->setIndexFirstAtom( uint( bond[ 0 ] ) );
				modelBond->setIndexSecondAtom( uint( bond[ 1 ] ) );

				const chemfiles::Bond::BondOrder bondOrder		= topology.bond_orders()[ vectorExtraBonds[ i ] ];
				const Model::Bond::ORDER		 modelBondOrder = Model::Bond::ORDER( int( bondOrder ) );
				modelBond->setOrder( modelBondOrder );

				p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
				p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
			}
		}

		if ( !Setting::COMPUTE_BOND_ORDER_ON_CHEMFILE )
		{
			bondComputationChrono.start();
			const bool allBondsRecomputed = Util::Molecule::recomputeBondOrdersFromFile( p_molecule );

			if ( !allBondsRecomputed )
			{
				_logDebug( "recomputeBondOrders with algorithm." );
				Util::Molecule::recomputeBondOrders( p_molecule );
			}
			bondComputationChrono.stop();
			_logDebug( "recomputeBondOrders: " + bondComputationChrono.elapsedTimeStr() );
		}

		assert( counter == counterOld );
	}

	bool LibChemfiles::_tryApplyingDynamicOnTargets( chemfiles::Trajectory &		  p_dynamicTrajectory,
													 std::vector<Model::Molecule *> & p_potentialTargets ) const
	{
		if ( p_dynamicTrajectory.nsteps() <= 0 )
			return false;

		const std::vector<Vec3f> positions		= readTrajectoryFrame( p_dynamicTrajectory );
		const size_t			 frameAtomCount = positions.size();

		std::vector<std::pair<Model::Molecule *, uint>> validTargets
			= std::vector<std::pair<Model::Molecule *, uint>>();
		validTargets.reserve( p_potentialTargets.size() );

		auto it = p_potentialTargets.begin();

		while ( it != p_potentialTargets.end() )
		{
			if ( _isThreadInterrupted() )
				return false;

			Model::Molecule * const molecule = *it;

			if ( molecule->getAtomCount() == frameAtomCount )
			{
				if ( _isThreadInterrupted() )
					return false;

				const uint indexFirstNewFrame = molecule->getFrameCount() == 1 ? 0 : molecule->getFrameCount();
				molecule->resizeAtomPositionFrames( indexFirstNewFrame + p_dynamicTrajectory.nsteps() );

				try
				{
					// First frame already read to know framecount.
					fillTrajectoryFrame( *molecule, indexFirstNewFrame, positions );
				}
				catch ( const std::exception & p_e )
				{
					_logError( "Error when reading trajectory : " + std::string( p_e.what() ) );

					// If an issue happened when reading trajectory, we pop back all non-filled frames
					std::vector<Model::Molecule::AtomPositionsFrame> & frames = molecule->getAtomPositionFrames();
					std::vector<Model::Molecule::AtomPositionsFrame>::reverse_iterator itFrame = frames.rbegin();

					while ( itFrame != frames.rend() && itFrame->size() == 0 )
					{
						frames.pop_back();
						itFrame = frames.rbegin();
					}

					continue;
				}

				validTargets.emplace_back( molecule, indexFirstNewFrame + 1 );
				it++;
			}
			else
			{
				it = p_potentialTargets.erase( it );
			}
		}

		p_potentialTargets.shrink_to_fit();

		if ( validTargets.size() > 0 )
		{
			_readTrajectoryFrames( p_dynamicTrajectory, validTargets, 1 );

			for ( const std::pair<Model::Molecule *, uint> & pairMoleculeFirstFrame : validTargets )
			{
				if ( _isThreadInterrupted() )
					return false;

				pairMoleculeFirstFrame.first->forceNotifyTrajectoryChanged();
			}
		}

		return validTargets.size() > 0;
	}

	// http://chemfiles.org/chemfiles/latest/formats.html#list-of-supported-formats
	const std::string LibChemfiles::_getFormat( const IO::FilePath & p_path )
	{
		std::string extension = p_path.extension();
		std::transform( extension.begin(), extension.end(), extension.begin(), tolower );
		if ( extension == "nc" )
		{
			return "Amber NetCDF";
		}
		else if ( extension == "cif" )
		{
			return "mmCIF"; // Workaround.
		}
		else if ( extension == "cml" )
		{
			return "CML";
		}
		else if ( extension == "cssr" )
		{
			return "CSSR";
		}
		else if ( extension == "dcd" )
		{
			return "DCD";
		}
		else if ( extension == "gro" )
		{
			return "GRO";
		}
		else if ( extension == "lammpstrj" )
		{
			return "LAMMPS";
		}
		else if ( extension == "mmcif" )
		{
			return "mmCIF";
		}
		else if ( extension == "mmtf" )
		{
			return "MMTF";
		}
		else if ( extension == "mol2" )
		{
			return "MOL2";
		}
		else if ( extension == "molden" )
		{
			return "Molden";
		}
		else if ( extension == "pdb" )
		{
			return "PDB";
		}
		else if ( extension == "sdf" )
		{
			return "SDF";
		}
		else if ( extension == "smi" )
		{
			return "SMI";
		}
		else if ( extension == "arc" )
		{
			return "Tinker";
		}
		else if ( extension == "tng" )
		{
			return "TNG";
		}
		else if ( extension == "trj" )
		{
			return "TRJ";
		}
		else if ( extension == "trr" )
		{
			return "TRR";
		}
		else if ( extension == "xtc" )
		{
			return "XTC";
		}
		else if ( extension == "xyz" )
		{
			return "XYZ";
		}
		else
		{
			assert( false );
			return "Unknown";
		}
	}

	const bool LibChemfiles::_needToRecomputeBonds( const std::string & p_format ) const { return p_format == "mmCIF"; }

} // namespace VTX::IO::Reader
