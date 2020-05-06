#include "lib_chemfiles.hpp"
#include "color/rgb.hpp"
#include <algorithm>
#include <magic_enum.hpp>
#include <unordered_map>
#include <vector>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void LibChemfiles::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				prepareChemfiles();
				chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path );
				readTrajectory( trajectory, p_molecule, p_path.getExtension() );
			}

			void LibChemfiles::readBuffer( const std::string & p_buffer,
										   const std::string & p_extension,
										   Model::Molecule &   p_molecule )
			{
				std::string extension = p_extension;
				std::transform( extension.begin(), extension.end(), extension.begin(), toupper );

				prepareChemfiles();
				chemfiles::Trajectory trajectory
					= chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), extension );
				readTrajectory( trajectory, p_molecule, p_extension );
			}

			void LibChemfiles::prepareChemfiles() const
			{
#ifdef _DEBUG
				chemfiles::warning_callback_t callback = []( const std::string & p_log ) { VTX_WARNING( p_log ); };
#else
				chemfiles::warning_callback_t callback = []( const std::string & p_log ) { VTX_WARNING( p_log ); };
#endif
				chemfiles::set_warning_callback( callback );
			}

			void LibChemfiles::readTrajectory( chemfiles::Trajectory & p_trajectory,
											   Model::Molecule &	   p_molecule,
											   const std::string &	   p_extension ) const
			{
				VTX_INFO( std::to_string( p_trajectory.nsteps() ) + " frames found" );

				if ( p_trajectory.nsteps() == 0 )
				{
					throw Exception::IOException( "Trajectory is empty" );
				}

				chemfiles::Frame						frame	 = p_trajectory.read();
				const chemfiles::Topology &				topology = frame.topology();
				const std::vector<chemfiles::Residue> & residues = topology.residues();
				const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();
				Model::Configuration::Molecule			config	 = p_molecule.getConfiguration();

				if ( frame.size() != topology.size() )
				{
					throw Exception::IOException( "Data count missmatch" );
				}

				// Set molecule properties.
				if ( frame.get( "name" ) )
				{
					p_molecule.setName( frame.get( "name" )->as_string() );
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
					VTX_DEBUG( propAtom );
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
					VTX_DEBUG( propResidue );
				}

				// If no residue, create a fake one.
				if ( residues.size() == 0 )
				{
					VTX_INFO( "No residues found" );
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
				modelFrame.resize( frame.size() );

				Model::Chain * modelChain;
				std::string	   lastChainName = "";
				uint		   chainModelId	 = -1;
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
						modelChain = &p_molecule.getChain( chainModelId );
						modelChain->setIndex( chainModelId );
						if ( chainName != "" )
						{
							modelChain->setName( chainName );
						}
						modelChain->setMoleculePtr( &p_molecule );
						modelChain->setIdFirstResidue( residueIdx );
						modelChain->setResidueCount( 0 );
						modelChain->setColor( Model::Chain::getChainIdColor( chainId ) );

						lastChainName = chainName;
					}

					modelChain = &p_molecule.getChain( chainModelId );
					modelChain->setResidueCount( modelChain->getResidueCount() + 1 );

					// Create residue.
					Model::Residue * modelResidue		   = new Model::Residue();
					p_molecule.getResidues()[ residueIdx ] = modelResidue;
					modelResidue->setIndex( residueIdx );
					modelResidue->setMoleculePtr( &p_molecule );
					modelResidue->setChainPtr( modelChain );
					modelResidue->setIdFirstAtom( uint( *residue.begin() ) );
					modelResidue->setAtomCount( uint( residue.size() ) );
					const std::string & residueSymbol = residue.name();
					std::optional		symbol = magic_enum::enum_cast<Model::Residue::RESIDUE_SYMBOL>( residueSymbol );
					symbol.has_value() ? modelResidue->setSymbol( symbol.value() )
									   : p_molecule.addUnknownResidueSymbol( residueSymbol );

					modelResidue->setColor( Model::Residue::SYMBOL_COLOR[ int( modelResidue->getSymbol() ) ] );

					bool isStandard = residue.properties().get( "is_standard_pdb" ).value_or( true ).as_bool();
					modelResidue->setType( isStandard ? Model::Residue::RESIDUE_TYPE::NORMAL
													  : Model::Residue::RESIDUE_TYPE::LIGAND );

					// PDB only.
					// TODO: modify chemfiles to load handedness!
					if ( p_extension == "pdb" )
					{
						std::string secondaryStructure
							= residue.properties().get( "secondary_structure" ).value_or( "" ).as_string();
						if ( secondaryStructure != "" )
						{
							if ( secondaryStructure == "extended" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::STRAND );
							}
							else if ( secondaryStructure == "turn" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::COIL );
							}
							else if ( secondaryStructure == "alpha helix" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
							}
							else if ( secondaryStructure == "omega helix" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
							}
							else if ( secondaryStructure == "gamma helix" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
							}
							else if ( secondaryStructure == "pi helix" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
							}
							else if ( secondaryStructure == "3-10 helix" )
							{
								modelResidue->setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
							}

							if ( config.isSecondaryStructureLoadedFromFile == false )
							{
								config.isSecondaryStructureLoadedFromFile = true;
							}
						}
					}

					for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
					{
						const uint				atomId = uint( *it );
						const chemfiles::Atom & atom   = topology[ atomId ];
						uint					atomType;
						atomType = uint( atom.properties().get( "atom_type" ).value_or( -1 ).as_double() );

						// Create atom.
						Model::Atom * modelAtom			= new Model::Atom();
						p_molecule.getAtoms()[ atomId ] = modelAtom;
						modelAtom->setIndex( atomId );
						modelAtom->setMoleculePtr( &p_molecule );
						modelAtom->setChainPtr( modelChain );
						modelAtom->setResiduePtr( modelResidue );
						std::string atomSymbol = atom.type();

						// VTX_INFO( atom.name() + " " + atom.type() );

						std::optional symbol = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );

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
								modelAtom->setType( Model::Atom::ATOM_TYPE::SOLVENT );
							}
							else if ( std::find( config.ionAtomIds.begin(), config.ionAtomIds.end(), atomType )
									  != config.ionAtomIds.end() )
							{
								modelAtom->setType( Model::Atom::ATOM_TYPE::ION );
							}
						}

						// Check PSF.
						if ( std::find( config.solventResidueSymbols.begin(),
										config.solventResidueSymbols.end(),
										residueSymbol )
							 != config.solventResidueSymbols.end() )
						{
							modelAtom->setType( Model::Atom::ATOM_TYPE::SOLVENT );
						}
						else if ( std::find(
									  config.ionResidueSymbols.begin(), config.ionResidueSymbols.end(), residueSymbol )
								  != config.ionResidueSymbols.end() )
						{
							modelAtom->setType( Model::Atom::ATOM_TYPE::ION );
						}
					}
				}

				// Fill other frames.
				for ( uint frameIdx = 1; frameIdx < p_trajectory.nsteps(); ++frameIdx )
				{
					VTX_INFO( "Frame " + std::to_string( frameIdx ) );
					Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( frameIdx );

					frame												   = p_trajectory.read_step( frameIdx );
					const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
					moleculeFrame.resize( positions.size() );
					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame[ positionIdx ]		 = { position[ 0 ], position[ 1 ], position[ 2 ] };
					}
				}

				// Bonds.
				p_molecule.getBonds().resize( bonds.size() );
				for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
				{
					const chemfiles::Bond & bond	  = bonds[ boundIdx ];
					Model::Bond *			modelBond = new Model::Bond();
					p_molecule.getBonds()[ boundIdx ] = modelBond;

					modelBond->setIndexFirstAtom( uint( bond[ 0 ] ) );
					modelBond->setIndexSecondAtom( uint( bond[ 1 ] ) );
				}
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
