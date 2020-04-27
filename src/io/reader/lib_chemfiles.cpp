#include "lib_chemfiles.hpp"
#undef INFINITE
#include "util/color.hpp"
#include <chemfiles.hpp>
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
				chemfiles::Trajectory trajectory( p_path, 'r' );

				VTX_INFO( std::to_string( trajectory.nsteps() ) + " frames found" );

				if ( trajectory.nsteps() == 0 ) { throw Exception::IOException( "Trajectory is empty" ); }

				chemfiles::Frame						frame	 = trajectory.read();
				const chemfiles::Topology &				topology = frame.topology();
				const std::vector<chemfiles::Residue> & residues = topology.residues();
				const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();

				VTX_INFO( std::to_string( frame.size() ) + " atoms" );
				VTX_INFO( std::to_string( residues.size() ) + " residues" );
				VTX_INFO( std::to_string( bonds.size() ) + " bonds" );

				if ( frame.size() != topology.size() ) { throw Exception::IOException( "Data count missmatch" ); }

				// Set molecule properties.
				if ( frame.get( "name" ) ) { p_molecule.setName( frame.get( "name" )->as_string() ); }
				p_molecule.setColor( Util::Color::randomPastelColor() );

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

				// Create models.
				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & modelFrame = p_molecule.getAtomPositionFrame( 0 );

				uint chainModelId = -1;
				// uint atomModelId  = 0;

				p_molecule.getAtoms().resize( frame.size() );
				p_molecule.getAtomPositionFrame( 0 ).resize( frame.size() );
				for ( uint i = 0; i < uint( frame.size() ); ++i )
				{
					p_molecule.getAtoms()[ i ] = new Model::Atom();
				}

				// If no residue, create a fake one.
				if ( residues.size() == 0 )
				{
					chemfiles::Residue residue = chemfiles::Residue( "" );
					for ( uint i = 0; i < frame.size(); ++i )
					{
						residue.add_atom( i );
					}
					frame.add_residue( residue );
				}

				Model::Chain * modelChain;
				std::string	   lastChainName = "";
				for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
				{
					const chemfiles::Residue & residue = residues[ residueIdx ];

					// Check if chain name changed.
					std::string chainName = "";
					std::string chainId	  = "";

					try
					{
						chainName = residue.properties().get( "chainname" ).value().as_string();
					}
					catch ( const std::exception & )
					{
						chainName = "";
					}
					try
					{
						chainId = residue.properties().get( "chainid" ).value().as_string();
					}
					catch ( const std::exception & )
					{
						chainId = "";
					}

					if ( chainName != lastChainName || p_molecule.getChainCount() == 0 )
					{
						VTX_INFO( chainName + " / " + chainId );
						// Create chain.
						p_molecule.addChain();
						chainModelId++;
						modelChain = &p_molecule.getChain( chainModelId );
						modelChain->setIndex( chainModelId );
						if ( chainName != "" ) { modelChain->setName( chainName ); }
						modelChain->setMoleculePtr( &p_molecule );
						modelChain->setIdFirstResidue( residueIdx );
						modelChain->setResidueCount( 0 );

						lastChainName = chainName;
					}

					modelChain = &p_molecule.getChain( chainModelId );
					modelChain->setResidueCount( modelChain->getResidueCount() + 1 );

					// Create residue.
					p_molecule.addResidue();
					Model::Residue & modelResidue = p_molecule.getResidue( residueIdx );
					modelResidue.setIndex( residueIdx );
					modelResidue.setMoleculePtr( &p_molecule );
					modelResidue.setChainPtr( modelChain );
					modelResidue.setIdFirstAtom( uint( *residue.begin() ) );
					modelResidue.setAtomCount( uint( residue.size() ) );
					const std::string & residueSymbol = residue.name();
					std::optional		symbol = magic_enum::enum_cast<Model::Residue::RESIDUE_SYMBOL>( residueSymbol );
					symbol.has_value() ? modelResidue.setSymbol( symbol.value() )
									   : p_molecule.addUnknownResidueSymbol( residueSymbol );

					try
					{
						std::string secondaryStructure
							= residue.properties().get( "secondary_structure" ).value().as_string();
						if ( secondaryStructure == "extended" )
						{ modelResidue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::STRAND ); }
						// else if ( secondaryStructure == "turn" )
						//{
						// modelResidue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::TURN );
						//}
						else if ( secondaryStructure == "alpha helix" )
						{
							modelResidue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
						}
						else if ( secondaryStructure == "pi helix" )
						{
							modelResidue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
						}
						else if ( secondaryStructure == "3-10 helix" )
						{
							modelResidue.setSecondaryStructure( Model::Residue::SECONDARY_STRUCTURE::HELIX );
						}

						if ( p_molecule.secondaryStructureLoadedFromFile() == false )
						{ p_molecule.setSecondaryStructureLoadedFromFile( true ); }
					}
					catch ( const std::exception & )
					{
					}

					for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
					{
						const uint				atomId	 = uint( *it );
						const chemfiles::Atom & atom	 = topology[ atomId ];
						uint					atomType = -1;
						try
						{
							atomType = uint( atom.properties().get( "atom_type" ).value().as_double() );
						}
						catch ( const std::exception & )
						{
						}

						// Create atom.
						Model::Atom & modelAtom = p_molecule.getAtom( atomId );
						modelAtom.setIndex( atomId );
						modelAtom.setMoleculePtr( &p_molecule );
						modelAtom.setChainPtr( modelChain );
						modelAtom.setResiduePtr( &modelResidue );
						std::string atomSymbol = atom.type();

						// VTX_INFO( atom.name() + " " + atom.type() );

						std::optional symbol = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );

						symbol.has_value() ? modelAtom.setSymbol( symbol.value() )
										   : p_molecule.addUnknownAtomSymbol( atom.type() );
						const uint * const colorStatic = Model::Atom::SYMBOL_COLOR[ (int)modelAtom.getSymbol() ];
						const float		   color[ 3 ]  = { float( colorStatic[ 0 ] ) / 100.f,
												   float( colorStatic[ 1 ] ) / 100.f,
												   float( colorStatic[ 1 ] ) / 100.f };

						modelAtom.setName( atom.name() );
						modelAtom.setColor( Vec3f( *color, *( color + 1 ), *( color + 2 ) ) );

						const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
						const chemfiles::Vector3D &					 position  = positions[ atomId ];
						Vec3f atomPosition	 = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
						modelFrame[ atomId ] = atomPosition;

						// Check PRM.
						if ( std::find( p_molecule.getPRM().solventIds.begin(),
										p_molecule.getPRM().solventIds.end(),
										atomType )
							 != p_molecule.getPRM().solventIds.end() )
						{ modelAtom.setType( Model::Atom::ATOM_TYPE::SOLVENT ); }
						else if ( std::find(
									  p_molecule.getPRM().ionIds.begin(), p_molecule.getPRM().ionIds.end(), atomType )
								  != p_molecule.getPRM().ionIds.end() )
						{
							modelAtom.setType( Model::Atom::ATOM_TYPE::ION );
						}

						// Check PSF.
						if ( std::find( p_molecule.getPSF().solventResidueSymbols.begin(),
										p_molecule.getPSF().solventResidueSymbols.end(),
										residueSymbol )
							 != p_molecule.getPSF().solventResidueSymbols.end() )
						{ modelAtom.setType( Model::Atom::ATOM_TYPE::SOLVENT ); }
						else if ( std::find( p_molecule.getPSF().ionResidueSymbols.begin(),
											 p_molecule.getPSF().ionResidueSymbols.end(),
											 residueSymbol )
								  != p_molecule.getPSF().ionResidueSymbols.end() )
						{
							modelAtom.setType( Model::Atom::ATOM_TYPE::ION );
						}
					}
				}

				// Fill other frames.
				// TODO: user resize()
				for ( uint frameIdx = 1; frameIdx < trajectory.nsteps(); ++frameIdx )
				{
					p_molecule.addAtomPositionFrame();
					VTX_INFO( "Frame " + std::to_string( frameIdx ) );
					Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( frameIdx );

					frame												   = trajectory.read_step( frameIdx );
					const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();

					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame.emplace_back( position[ 0 ], position[ 1 ], position[ 2 ] );
					}
				}

				// Bonds.
				for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
				{
					const chemfiles::Bond & bond = bonds[ boundIdx ];
					p_molecule.addBond();
					Model::Bond & modelBond = p_molecule.getBond( boundIdx );

					modelBond.setIndexFirstAtom( uint( bond[ 0 ] ) );
					modelBond.setIndexSecondAtom( uint( bond[ 1 ] ) );
				}
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
