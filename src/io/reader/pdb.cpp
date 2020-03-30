#include "pdb.hpp"
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
			bool PDB::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				VTX_INFO( "Loading " + p_path.getFileName() + "..." );

				chemfiles::Trajectory trajectory( p_path, 'r' );

				VTX_DEBUG( std::to_string( trajectory.nsteps() ) + " frames" );

				if ( trajectory.nsteps() == 0 )
				{
					VTX_WARNING( "Empty trajectory" );
					return false;
				}

				chemfiles::Frame						frame	 = trajectory.read();
				const chemfiles::Topology &				topology = frame.topology();
				const std::vector<chemfiles::Residue> & residues = topology.residues();
				const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();

				VTX_DEBUG( std::to_string( frame.size() ) + " atoms" );
				VTX_DEBUG( std::to_string( residues.size() ) + " residues" );
				VTX_DEBUG( std::to_string( bonds.size() ) + " bonds" );

				if ( frame.size() != topology.size() )
				{
					VTX_ERROR( "Data count missmatch : " + std::to_string( frame.size() )
							   + " != " + std::to_string( topology.size() ) );
					return false;
				}

				VTX_INFO( "Creating models..." );

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
				uint atomModelId  = 0;

				Model::Chain * modelChain;
				std::string	   lastChainName = "";
				for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
				{
					const chemfiles::Residue & residue = residues[ residueIdx ];

					// Check if chain name changed.
					// TODO: check if optional exist.
					const std::string chainName = residue.properties().get( "chainname" ).value().as_string();

					if ( chainName != lastChainName )
					{
						VTX_DEBUG( chainName );
						// Create chain.
						p_molecule.addChain();
						chainModelId++;
						modelChain = &p_molecule.getChain( chainModelId );
						modelChain->setIndex( chainModelId );
						modelChain->setName( chainName );
						modelChain->setMoleculePtr( &p_molecule );
						modelChain->setIdFirstResidue( residueIdx );
						modelChain->setResidueCount( 0 );
						modelChain->setColor( Util::Color::randomPastelColor() );

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
					modelResidue.setIdFirstAtom( atomModelId );
					modelResidue.setAtomCount( uint( residue.size() ) );
					modelResidue.setColor( Util::Color::randomPastelColor() );
					const std::string & residueSymbol = residue.name();
					std::optional		symbol = magic_enum::enum_cast<Model::Residue::RESIDUE_SYMBOL>( residueSymbol );
					symbol.has_value() ? modelResidue.setSymbol( symbol.value() )
									   : p_molecule.addUnknownResidueSymbol( residueSymbol );

					for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
					{
						const size_t			atomId = *it;
						const chemfiles::Atom & atom   = topology[ atomId ];

						// Create atom.
						p_molecule.addAtom();
						Model::Atom & modelAtom = p_molecule.getAtom( atomModelId );
						modelAtom.setIndex( atomModelId );
						modelAtom.setMoleculePtr( &p_molecule );
						modelAtom.setChainPtr( modelChain );
						modelAtom.setResiduePtr( &modelResidue );
						std::string	  atomSymbol = atom.type();
						std::optional symbol	 = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );
						// while ( symbol.has_value() == false && atomSymbol.size() > 0 )
						//{
						//	atomSymbol = atomSymbol.substr( 0, atomSymbol.size() - 1 );
						//	symbol	   = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );
						//}
						// std::optional symbol
						//	= magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol.substr( 0, 1 ) );
						symbol.has_value() ? modelAtom.setSymbol( symbol.value() )
										   : p_molecule.addUnknownAtomSymbol( atom.name() );
						const float * const color = Model::Atom::SYMBOL_COLOR[ (int)modelAtom.getSymbol() ];
						modelAtom.setColor( Vec3f( *color, *( color + 1 ), *( color + 2 ) ) );

						const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
						const chemfiles::Vector3D &					 position  = positions[ atomId ];
						Vec3f atomPosition = Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
						modelFrame.push_back( atomPosition );

						atomModelId++;
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

				return true;
			}

			bool PDB::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
