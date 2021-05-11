#include "lib_chemfiles.hpp"
#include "color/rgb.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "worker/loader.hpp"
#include <QDir>
#include <QFileInfo>
#include <algorithm>
#include <iostream>
#include <magic_enum.hpp>
#include <thread>
#include <unordered_map>
#include <vector>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void LibChemfiles::readFile( const FilePath & p_path, Model::Molecule & p_molecule )
			{
				_prepareChemfiles();
				chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string() );
				_readTrajectory( trajectory, p_path, p_molecule );
			}

			void LibChemfiles::readBuffer( const std::string & p_buffer,
										   const FilePath &	   p_path,
										   Model::Molecule &   p_molecule )
			{
				std::string extension = p_path.extension().string().substr( 1, p_path.extension().string().size() );
				std::transform( extension.begin(), extension.end(), extension.begin(), toupper );
				_prepareChemfiles();
				chemfiles::Trajectory trajectory
					= chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), extension );
				_readTrajectory( trajectory, p_path, p_molecule );
			}

			void LibChemfiles::_prepareChemfiles() const
			{
#ifdef _DEBUG
				chemfiles::warning_callback_t callback;
				callback = [ & ]( const std::string & p_log ) { _logWarning( p_log ); };

#else
				chemfiles::warning_callback_t callback = []( const std::string & p_log ) { /*VTX_WARNING( p_log );*/ };
#endif
				chemfiles::set_warning_callback( callback );
			}

			void LibChemfiles::fillTrajectoryFrames( chemfiles::Trajectory & p_trajectory,
													 Model::Molecule &		 p_molecule ) const
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
					moleculeFrame.resize( positions.size() );
					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame[ positionIdx ]		 = { position[ 0 ], position[ 1 ], position[ 2 ] };
					}
#ifdef _DEBUG
					if ( frameIdx % 100 == 0 )
					{
						_logDebug( "Frames from " + std::to_string( startingFrame ) + " to "
								   + std::to_string( frameIdx )
								   + " read in: " + std::to_string( timeReadingFrames.intervalTime() ) + "s" );
						startingFrame = frameIdx;
					}
#endif // DEBUG
				}
				timeReadingFrames.stop();
				_logInfo( "Frames read in: " + std::to_string( timeReadingFrames.elapsedTime() ) + "s" );
			}

			void LibChemfiles::_readTrajectory( chemfiles::Trajectory & p_trajectory,
												const FilePath &		p_path,
												Model::Molecule &		p_molecule ) const
			{
				_logInfo( std::to_string( p_trajectory.nsteps() ) + " frames found" );

				if ( p_trajectory.nsteps() == 0 )
				{
					throw Exception::IOException( "Trajectory is empty" );
				}

				// if opening a DCD file check if a topology file is present in the same folder
				QFileInfo fileInfo( QString::fromStdString( p_path.string() ) );
				if ( fileInfo.suffix().toStdString() == "dcd" )
				{
					std::string filePathWithoutExt
						= QString( fileInfo.path() + QDir::separator() + fileInfo.baseName() ).toStdString();
					std::vector<std::string> topExtensions	= { ".xyz", ".pdb", ".mol2" };
					std::string				 foundExtension = "";
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

				_logInfo( "Trajectory read in: " + std::to_string( chrono.elapsedTime() ) + "s" );

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
					chemfiles::Residue residue = chemfiles::Residue( "" );
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
				p_molecule.setColor( Color::Rgb::randomPastel() );

				// Check properties, same for all atoms/residues?
				if ( frame.size() > 0 )
				{
					std::string propAtom = std::to_string( frame[ 0 ].properties().size() ) + " properties in atoms:";
					for ( chemfiles::property_map::iterator it = frame[ 0 ].properties().begin();
						  it != frame[ 0 ].properties().end();
						  ++it )
					{
						propAtom += " " + it->first;
					}
					_logDebug( propAtom );
				}

				if ( residues.size() > 0 )
				{
					std::string propResidue
						= std::to_string( residues[ 0 ].properties().size() ) + " properties in residues:";
					for ( chemfiles::property_map::iterator it = residues[ 0 ].properties().begin();
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
					chemfiles::Residue residue = chemfiles::Residue( "" );
					for ( uint i = 0; i < frame.size(); ++i )
					{
						residue.add_atom( i );
					}
					frame.add_residue( residue );
				}

				// Create models.
				p_molecule.getFrames().resize( p_trajectory.nsteps() );
				Model::Molecule::AtomPositionsFrame & modelFrame = p_molecule.getAtomPositionFrame( 0 );
				p_molecule.getResidues().resize( topology.residues().size() );
				p_molecule.getAtoms().resize( frame.size() );
				p_molecule.getBufferAtomRadius().resize( frame.size() );
				p_molecule.getBufferAtomVisibilities().resize( frame.size(), 1u );
				p_molecule.getBufferAtomSelection().resize( frame.size(), 0u );
				modelFrame.resize( frame.size() );

				Model::Chain * modelChain;
				std::string	   lastChainName   = "";
				uint		   chainModelId	   = -1;
				bool		   chainIsStandard = true;

				std::map<uint, std::vector<const chemfiles::Bond *>> mapResidueBonds
					= std::map<uint, std::vector<const chemfiles::Bond *>>();

				int oldIndexInChain = INT_MIN;
				for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
				{
					const chemfiles::Residue & residue = residues[ residueIdx ];

					// Check if chain name changed.
					const std::string chainName = residue.properties().get( "chainname" ).value_or( "" ).as_string();
					const std::string chainId	= residue.properties().get( "chainid" ).value_or( "" ).as_string();

					if ( chainName != lastChainName || p_molecule.getChainCount() == 0 )
					{
						// Create chain.
						p_molecule.addChain();
						chainModelId++;
						modelChain = p_molecule.getChain( chainModelId );
						modelChain->setIndex( chainModelId );
						if ( chainName != "" )
						{
							modelChain->setName( chainName );
						}
						modelChain->setMoleculePtr( &p_molecule );
						modelChain->setIndexFirstResidue( residueIdx );
						modelChain->setResidueCount( 0 );
						modelChain->setColor( Model::Chain::getChainIdColor( chainId ) );
						lastChainName	= chainName;
						oldIndexInChain = INT_MIN;
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
					std::string residueSymbol = residue.name();
					std::transform( residueSymbol.begin(),
									residueSymbol.end(),
									residueSymbol.begin(),
									[]( unsigned char c ) { return std::toupper( c ); } );
					std::optional symbol = magic_enum::enum_cast<Model::Residue::SYMBOL>( residueSymbol );
					symbol.has_value() ? modelResidue->setSymbol( symbol.value() )
									   : p_molecule.addUnknownResidueSymbol( residueSymbol );

					modelResidue->setColor( Model::Residue::SYMBOL_COLOR[ int( modelResidue->getSymbol() ) ] );

					bool isStandard = residue.properties().get( "is_standard_pdb" ).value_or( true ).as_bool();
					modelResidue->setType( isStandard ? Model::Residue::TYPE::STANDARD
													  : Model::Residue::TYPE::NON_STANDARD );

					// Check residue index in chain.
					int indexInChain = (int)residue.id().value_or( INT_MIN );
					assert( oldIndexInChain <= indexInChain );
					modelResidue->setIndexInOriginalChain( indexInChain );
					oldIndexInChain = indexInChain;

					mapResidueBonds.emplace( modelResidue->getIndex(), std::vector<const chemfiles::Bond *>() );

					// PDB only.
					// TODO: modify chemfiles to load handedness!
					if ( p_path.extension() == ".pdb" )
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
								modelResidue->setSecondaryStructure(
									Model::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT );
							}
							else if ( secondaryStructure == "left-handed alpha helix" )
							{
								modelResidue->setSecondaryStructure(
									Model::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT );
							}
							else if ( secondaryStructure == "right-handed 3-10 helix" )
							{
								modelResidue->setSecondaryStructure(
									Model::SecondaryStructure::TYPE::HELIX_3_10_RIGHT );
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
						const uint				atomId = uint( *it );
						const chemfiles::Atom & atom   = topology[ atomId ];
						uint					atomType;
						atomType = uint( atom.properties().get( "atom_type" ).value_or( -1 ).as_double() );

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
						modelAtom->setColor( Model::Atom::SYMBOL_COLOR[ int( modelAtom->getSymbol() ) ] );

						const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
						const chemfiles::Vector3D &					 position  = positions[ atomId ];
						Vec3f atomPosition	 = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
						modelFrame[ atomId ] = atomPosition;

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
						if ( std::find( config.solventResidueSymbols.begin(),
										config.solventResidueSymbols.end(),
										residueSymbol )
							 != config.solventResidueSymbols.end() )
						{
							solventCounter++;
							modelAtom->setType( Model::Atom::TYPE::SOLVENT );
						}
						else if ( std::find(
									  config.ionResidueSymbols.begin(), config.ionResidueSymbols.end(), residueSymbol )
								  != config.ionResidueSymbols.end() )
						{
							ionCounter++;
							modelAtom->setType( Model::Atom::TYPE::ION );
						}

						// Radius.
						p_molecule.getBufferAtomRadius()[ atomId ] = modelAtom->getVdwRadius();
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
					fillTrajectoryFrames( p_trajectory, p_molecule );
				}

				// Bonds.
				// Sort by residus.
				// Map with residue index to keep the order.
				std::vector<const chemfiles::Bond *> bondsExtraResidues = std::vector<const chemfiles::Bond *>();
				for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
				{
					const chemfiles::Bond & bond = bonds[ boundIdx ];

					Model::Residue * residueStart = p_molecule.getAtom( uint( bond[ 0 ] ) )->getResiduePtr();
					Model::Residue * residueEnd	  = p_molecule.getAtom( uint( bond[ 1 ] ) )->getResiduePtr();

					if ( residueStart == residueEnd )
					{
						// Create vector if needed.
						mapResidueBonds[ residueStart->getIndex() ].emplace_back( &bond );
					}
					else
					{
						bondsExtraResidues.emplace_back( &bond );
					}
				}

				// Create models.
				uint counter = 0;
				p_molecule.getBonds().resize( bonds.size() );
				p_molecule.getBufferBonds().resize( bonds.size() * 2 );
				for ( const std::pair<uint, std::vector<const chemfiles::Bond *>> & pair : mapResidueBonds )
				{
					Model::Residue * const						 residue	 = p_molecule.getResidue( pair.first );
					const std::vector<const chemfiles::Bond *> & vectorBonds = pair.second;

					residue->setIndexFirstBond( counter );
					residue->setBondCount( uint( vectorBonds.size() ) );

					for ( uint i = 0; i < vectorBonds.size(); ++i, ++counter )
					{
						const chemfiles::Bond & bond	  = *vectorBonds[ i ];
						Model::Bond *			modelBond = MVC::MvcManager::get().instantiateModel<Model::Bond>();
						p_molecule.getBonds()[ counter ]  = modelBond;

						modelBond->setMoleculePtr( &p_molecule );
						modelBond->setIndexFirstAtom( uint( bond[ 0 ] ) );
						modelBond->setIndexSecondAtom( uint( bond[ 1 ] ) );

						p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
						p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
					}
				}

				// Bonds between residues.
				for ( uint i = 0; i < bondsExtraResidues.size(); ++i, ++counter )
				{
					const chemfiles::Bond & bond	  = *bondsExtraResidues[ i ];
					Model::Bond *			modelBond = MVC::MvcManager::get().instantiateModel<Model::Bond>();
					p_molecule.getBonds()[ counter ]  = modelBond;

					Model::Residue * residueStart = p_molecule.getAtom( uint( bond[ 0 ] ) )->getResiduePtr();
					Model::Residue * residueEnd	  = p_molecule.getAtom( uint( bond[ 1 ] ) )->getResiduePtr();

					modelBond->setMoleculePtr( &p_molecule );
					modelBond->setIndexFirstAtom( uint( bond[ 0 ] ) );
					modelBond->setIndexSecondAtom( uint( bond[ 1 ] ) );

					residueStart->getIndexExtraBondStart().emplace_back( counter );
					residueEnd->getIndexExtraBondEnd().emplace_back( counter );

					p_molecule.getBufferBonds()[ counter * 2u ]		 = uint( bond[ 0 ] );
					p_molecule.getBufferBonds()[ counter * 2u + 1u ] = uint( bond[ 1 ] );
				}
			}

			void LibChemfiles::_logError( const std::string & p_log ) const
			{
				if ( _loader != nullptr )
					emit _loader->logDebug( p_log );
				else
					VTX_ERROR( p_log );
			}

			void LibChemfiles::_logWarning( const std::string & p_log ) const
			{
				if ( _loader != nullptr )
					emit _loader->logWarning( p_log );
				else
					VTX_WARNING( p_log );
			}
			void LibChemfiles::_logInfo( const std::string & p_log ) const
			{
				if ( _loader != nullptr )
					emit _loader->logInfo( p_log );
				else
					VTX_INFO( p_log );
			}
			void LibChemfiles::_logDebug( const std::string & p_log ) const
			{
				if ( _loader != nullptr )
					emit _loader->logDebug( p_log );
				else
					VTX_DEBUG( p_log );
			}
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
