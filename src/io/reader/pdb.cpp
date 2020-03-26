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
				p_molecule.setFileName( p_path.getFileNameWithoutExtension() );
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

				// Get residue by chain.
				using vectorResidueId					= std::vector<uint64_t>;
				using mapChainIdToResidueId				= std::unordered_map<std::string, vectorResidueId>;
				mapChainIdToResidueId mapChainToResidue = mapChainIdToResidueId();
				size_t				  atomsInResidues	= 0;
				for ( uint residueIdx = 0; residueIdx < residues.size(); ++residueIdx )
				{
					const chemfiles::Residue & residue = residues[ residueIdx ];
					const std::string &		   chain   = residue.properties().get( "chainname" ).value().as_string();
					if ( mapChainToResidue.find( chain ) == mapChainToResidue.end() )
					{ mapChainToResidue.emplace( chain, vectorResidueId() ); }
					mapChainToResidue.at( chain ).push_back( residueIdx );
					atomsInResidues += residue.size();
				}

				VTX_DEBUG( "Atoms in residues: " + std::to_string( atomsInResidues ) );
				VTX_DEBUG( "Total atoms: " + std::to_string( frame.size() ) );

				if ( frame.size() != atomsInResidues ) { VTX_WARNING( "Some atoms are not in a residue" ); }

				// Create models.
				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & modelFrame = p_molecule.getAtomPositionFrame( 0 );

				uint chainModelId	= 0;
				uint residueModelId = 0;
				uint atomModelId	= 0;

				for ( std::pair<std::string, vectorResidueId> pair : mapChainToResidue )
				{
					const std::string & chainId	   = pair.first;
					vectorResidueId &	residueIds = pair.second;

					// Create chain.
					p_molecule.addChain();
					Model::Chain & modelChain = p_molecule.getChain( chainModelId );
					modelChain.setId( chainModelId );
					modelChain.setName( chainId );
					modelChain.setMoleculePtr( &p_molecule );
					modelChain.setIdFirstResidue( residueModelId );
					modelChain.setResidueCount( uint( residueIds.size() ) );
					modelChain.setColor( Util::Color::randomPastelColor() );

					for ( size_t residueId : residueIds )
					{
						const chemfiles::Residue & residue = residues[ residueId ];

						// Create residue.
						p_molecule.addResidue();
						Model::Residue & modelResidue = p_molecule.getResidue( residueModelId );
						modelResidue.setId( residueModelId );
						modelResidue.setMoleculePtr( &p_molecule );
						modelResidue.setChainPtr( &modelChain );
						modelResidue.setIdFirstAtom( atomModelId );
						modelResidue.setAtomCount( uint( residue.size() ) );
						modelResidue.setColor( Util::Color::randomPastelColor() );

						for ( std::vector<size_t>::const_iterator it = residue.begin(); it != residue.end(); it++ )
						{
							const size_t			atomId = *it;
							const chemfiles::Atom & atom   = topology[ atomId ];

							// Create atom.
							p_molecule.addAtom();
							Model::Atom & modelAtom = p_molecule.getAtom( atomModelId );
							modelAtom.setId( atomModelId );
							modelAtom.setMoleculePtr( &p_molecule );
							modelAtom.setChainPtr( &modelChain );
							modelAtom.setResiduePtr( &modelResidue );
							const float * const color = Model::Atom::SYMBOL_COLOR[ (int)modelAtom.getSymbol() ];
							modelAtom.setColor( Vec3f( *color, *( color + 1 ), *( color + 2 ) ) );
							const float atomRadius = modelAtom.getVdwRadius();
							p_molecule.addAtomRadius( atomRadius );
							const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
							const chemfiles::Vector3D &					 position  = positions[ atomId ];
							modelFrame.push_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );

							atomModelId++;
						}
						residueModelId++;
					}
					chainModelId++;
				}

				// Isolated atoms?
				std::vector<size_t> isolatedAtom = std::vector<size_t>();
				for ( size_t i = 0; i < topology.size(); ++i )
				{
					std::experimental::optional<const chemfiles::Residue &> residue = topology.residue_for_atom( i );
					if ( residue == std::experimental::nullopt ) { VTX_WARNING( "Atom without residue found" ); }
				}

				// Bonds.
				for ( uint boundIdx = 0; boundIdx < uint( bonds.size() ); ++boundIdx )
				{
					const chemfiles::Bond & bond = bonds[ boundIdx ];

					p_molecule.addBond( uint( bond[ 0 ] ) );
					p_molecule.addBond( uint( bond[ 1 ] ) );
				}

				return true;
			}

			bool PDB::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
