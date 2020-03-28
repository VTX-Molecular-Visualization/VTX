#include "arc.hpp"
#undef INFINITE
#include "define.hpp"
#include "util/color.hpp"
#include <algorithm>
#include <chemfiles.hpp>
#include <magic_enum.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool ARC::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				try
				{
					chemfiles::Trajectory trajectory( p_path, 'r' );
					VTX_INFO( std::to_string( trajectory.nsteps() ) + " frames" );

					if ( trajectory.nsteps() == 0 ) { return false; }

					// Read first frame
					chemfiles::Frame					   frame	 = trajectory.read();
					chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();

					// TODO: check that
					// As Matthieu said: we assume that bonds and residues do not change during simulation
					// Angles and dihedrals change but this information is not used for rendering
					// So we just capture topology of the first frame
					const chemfiles::Topology &				topology = frame.topology();
					const std::vector<chemfiles::Residue> & residues = topology.residues();
					const std::vector<chemfiles::Bond> &	bonds	 = topology.bonds();

					if ( frame.get( "name" ) ) { p_molecule.setName( frame.get( "name" )->as_string() ); }

					if ( frame.size() > 0 )
					{
						std::string propAtom
							= std::to_string( frame[ 0 ].properties().size() ) + " properties in atoms:";
						for ( chemfiles::property_map::iterator it = frame[ 0 ].properties().begin();
							  it != frame[ 0 ].properties().end();
							  ++it )
						{
							propAtom += " " + it->first;
						}
						VTX_DEBUG( propAtom );
					}

					// Seems that .arc does not provide information about chains or residues...
					p_molecule.addChains( 1 );
					Model::Chain & chain = p_molecule.getChain( 0 );
					chain.setMoleculePtr( &p_molecule );
					chain.setIdFirstResidue( 0 );
					chain.setResidueCount( 1 );
					chain.setColor( Util::Color::randomPastelColor() );

					p_molecule.addResidues( 1 );
					Model::Residue & residue = p_molecule.getResidue( 0 );
					residue.setMoleculePtr( &p_molecule );
					residue.setChainPtr( &chain );
					residue.setIdFirstAtom( 0 );
					residue.setAtomCount( uint( positions.size() ) );
					residue.setIdFirstBond( 0 );
					residue.setBondCount( uint( bonds.size() ) );
					residue.setColor( Util::Color::randomPastelColor() );

					p_molecule.addAtoms( uint( positions.size() ) );

					// First frame.
					VTX_INFO( "Frame 0" );
					p_molecule.addAtomPositionFrame();
					Model::Molecule::AtomPositionsFrame & firstMoleculeFrame = p_molecule.getAtomPositionFrame( 0 );
					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Atom & atom = frame[ positionIdx ];
						uint		  atomType		 = uint( atom.properties().get( "atom_type" ).value().as_double() );
						Model::Atom & modelAtom		 = p_molecule.getAtom( positionIdx );
						modelAtom.setMoleculePtr( &p_molecule );
						modelAtom.setChainPtr( &chain );
						modelAtom.setResiduePtr( &residue );
						modelAtom.setIndex( positionIdx );
						const std::string & atomSymbol = atom.name();
						std::optional symbol = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );
						symbol.has_value() ? modelAtom.setSymbol( symbol.value() )
										   : p_molecule.addUnknownAtomSymbol( atomSymbol );
						const float * const color = Model::Atom::SYMBOL_COLOR[ (int)modelAtom.getSymbol() ];
						modelAtom.setColor( Vec3f( *color, *( color + 1 ), *( color + 2 ) ) );

						const chemfiles::Vector3D & position = positions[ positionIdx ];
						firstMoleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
						const Vec3f & atomPosition = firstMoleculeFrame[ positionIdx ];
						const float	  atomRadius   = modelAtom.getVdwRadius();
						p_molecule.addAtomRadius( atomRadius );

						p_molecule.extendAABB( atomPosition, atomRadius );

						if ( p_molecule.getIdFirstAtomSolvant() == INT_MAX )
						{
							if ( std::find( p_molecule.getPRM().solvantIds.begin(),
											p_molecule.getPRM().solvantIds.end(),
											atomType )
								 != p_molecule.getPRM().solvantIds.end() )
							{
								VTX_DEBUG( "First solvant atom found: " + std::to_string( positionIdx ) );
								p_molecule.setIdFirstAtomSolvant( positionIdx );
							}
						}
					}
					// Fill other frames.
					for ( uint frameIdx = 1; frameIdx < trajectory.nsteps(); ++frameIdx )
					{
						p_molecule.addAtomPositionFrame();
						VTX_INFO( "Frame " + std::to_string( frameIdx ) );
						Model::Molecule::AtomPositionsFrame & moleculeFrame
							= p_molecule.getAtomPositionFrame( frameIdx );

						frame	  = trajectory.read_step( frameIdx );
						positions = frame.positions();

						for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
						{
							const chemfiles::Vector3D & position = positions[ positionIdx ];
							moleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
						}
					}

					// Fill bonds
					for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
					{
						const chemfiles::Bond & bond = bonds[ boundIdx ];

						p_molecule.addBond( uint( bond[ 0 ] ) );
						p_molecule.addBond( uint( bond[ 1 ] ) );

						if ( uint( bond[ 0 ] ) == p_molecule.getIdFirstAtomSolvant()
							 || uint( bond[ 1 ] ) == p_molecule.getIdFirstAtomSolvant() )
						{ p_molecule.setIdFirstBondSolvant( boundIdx ); }
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Could not decode file: " + p_path.getFileName() );
					VTX_ERROR( p_e.what() );
					return false;
				}
				return true;
			}

			bool ARC::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
