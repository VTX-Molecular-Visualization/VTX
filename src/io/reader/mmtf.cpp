#include "mmtf.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "util/color.hpp"
#include "util/logger.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool MMTF::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				mmtf::StructureData data;

				VTX_INFO( "Loading " + p_path.getFileName() + "..." );
				try
				{
					mmtf::decodeFromFile( data, p_path.c_str() );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Could not decode file: " + p_path.getFileName() );
					VTX_ERROR( p_e.what() );
					return false;
				}

				return _readStructureData( data, p_molecule );
			}

			bool MMTF::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule )
			{
				mmtf::StructureData data;
				mmtf::decodeFromBuffer( data, p_buffer.c_str(), p_buffer.size() );

				return _readStructureData( data, p_molecule );
			}

			bool MMTF::_readStructureData( const mmtf::StructureData & p_data, Model::Molecule & p_molecule )
			{
				// Check for consistency.
				if ( p_data.hasConsistentData( true ) ) { VTX_INFO( "File loaded" ); }
				else
				{
					VTX_ERROR( "Inconsistent file" );
					return false;
				}

				VTX_INFO( std::to_string( p_data.numModels ) + " models found" );
				VTX_INFO( "Creating models..." );

				// Set molecule properties.
				p_molecule.setName( p_data.title );
				p_molecule.setColor( Util::Color::randomPastelColor() );

				float x, y, z;
				uint  chainGlobalIdx   = 0;
				uint  residueGlobalIdx = 0;
				uint  atomGlobalIdx	   = 0;
				uint  bondGlobalIdx	   = 0;

				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & frame = p_molecule.getAtomPositionFrame( 0 );

				uint chainCount = p_data.chainsPerModel[ 0 ];
#ifdef _DEBUG
				p_molecule.chainCount = chainCount;
#endif
				// For each chain in the model 0.
				for ( ; chainGlobalIdx < chainCount; ++chainGlobalIdx )
				{
					// New chain.
					p_molecule.addChain();
					Model::Chain & chain = p_molecule.getChain( chainGlobalIdx );
					chain.setMoleculePtr( &p_molecule );
					chain.setIndex( chainGlobalIdx );
					chain.setName( p_data.chainNameList[ chainGlobalIdx ] );
					chain.setIdFirstResidue( residueGlobalIdx );
					chain.setResidueCount( p_data.groupsPerChain[ chainGlobalIdx ] );
					chain.setColor( Util::Color::randomPastelColor() );

					// For each residue in the chain.
					uint residueCount = p_data.groupsPerChain[ chainGlobalIdx ];
					if ( residueCount == 0 ) { VTX_WARNING( "No residues" ); }
#ifdef _DEBUG
					p_molecule.residueCount += residueCount;
#endif
					for ( uint residueLocalIdx = 0; residueLocalIdx < residueCount;
						  ++residueLocalIdx, ++residueGlobalIdx )
					{
						const mmtf::GroupType & group = p_data.groupList[ p_data.groupTypeList[ residueGlobalIdx ] ];

						// New residue.
						p_molecule.addResidue();
						Model::Residue & residue = p_molecule.getResidue( residueGlobalIdx );
						residue.setMoleculePtr( &p_molecule );
						residue.setChainPtr( &chain );
						residue.setIndex( residueGlobalIdx );
						const std::string & residueSymbol = group.groupName;
						std::optional symbol = magic_enum::enum_cast<Model::Residue::RESIDUE_SYMBOL>( residueSymbol );
						symbol.has_value() ? residue.setSymbol( symbol.value() )
										   : p_molecule.addUnknownResidueSymbol( residueSymbol );
						residue.setIdFirstAtom( atomGlobalIdx );
						residue.setAtomCount( uint( group.atomNameList.size() ) );
						// residue.setIdFirstBond( bondGlobalIdx );
						// residue.setBondCount( uint( group.bondAtomList.size() ) / 2u ); // 2 index by bond.
						if ( group.bondAtomList.size() % 2 != 0 ) { VTX_WARNING( "Incorrect number of bonds" ); }

						residue.setColor( Util::Color::randomPastelColor() );

						// For each atom in the residue.
						uint atomCount = uint( group.atomNameList.size() );
						if ( atomCount == 0 ) { VTX_WARNING( "No atoms" ); }
#ifdef _DEBUG
						p_molecule.atomCount += atomCount;
#endif

						for ( uint atomIdx = 0; atomIdx < atomCount; ++atomIdx, ++atomGlobalIdx )
						{
							// New atom.
							p_molecule.addAtom();
							Model::Atom & atom = p_molecule.getAtom( atomGlobalIdx );
							atom.setMoleculePtr( &p_molecule );
							atom.setChainPtr( &chain );
							atom.setResiduePtr( &residue );
							atom.setIndex( atomGlobalIdx );
							const std::string & atomSymbol = group.elementList[ atomIdx ];
							std::optional symbol = magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>( "A_" + atomSymbol );
							symbol.has_value() ? atom.setSymbol( symbol.value() )
											   : p_molecule.addUnknownAtomSymbol( atomSymbol );
							const float * const color = Model::Atom::SYMBOL_COLOR[ (int)atom.getSymbol() ];
							atom.setColor( Vec3f( *color, *( color + 1 ), *( color + 2 ) ) );

							x = p_data.xCoordList[ atomGlobalIdx ];
							y = p_data.yCoordList[ atomGlobalIdx ];
							z = p_data.zCoordList[ atomGlobalIdx ];

							frame.emplace_back( Vec3f( x, y, z ) );
							const Vec3f & atomPosition = frame[ atomGlobalIdx ];
							const float	  atomRadius   = atom.getVdwRadius();
						}

						// For each bond in the residue.
						uint bondCount = uint( group.bondAtomList.size() ) / 2u;
#ifdef _DEBUG
						p_molecule.bondCount += bondCount;
#endif
						for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, bondGlobalIdx++ )
						{
							p_molecule.addBond();
							Model::Bond & bond = p_molecule.getBond( bondGlobalIdx );

							bond.setIndexFirstAtom( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx ] );
							bond.setIndexSecondAtom( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx + 1 ] );
						}
					}
				}

				// Add all atom's bonds.
				uint bondCount = uint( p_data.bondAtomList.size() ) / 2u;
				for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, ++bondGlobalIdx )
				{
					p_molecule.addBond();
					Model::Bond & bond = p_molecule.getBond( bondGlobalIdx );

					bond.setIndexFirstAtom( p_data.bondAtomList[ boundIdx ] );
					bond.setIndexSecondAtom( p_data.bondAtomList[ boundIdx + 1 ] );
				}

#ifdef _DEBUG
				p_molecule.bondCount += (uint)p_data.bondAtomList.size() / 2u;
#endif

#ifdef _DEBUG
				if ( p_molecule.getChainCount() != p_molecule.chainCount
					 || p_molecule.getChainCount() != p_data.numChains )
				{
					VTX_ERROR( "Chain count error: " + std::to_string( p_molecule.getChainCount() ) + " / "
							   + std::to_string( p_molecule.chainCount ) + " / " + std::to_string( p_data.numChains ) );
				}
				if ( p_molecule.getResidueCount() != p_molecule.residueCount
					 || p_molecule.getResidueCount() != p_data.numGroups )
				{
					VTX_ERROR( "Residue count error: " + std::to_string( p_molecule.getResidueCount() ) + " / "
							   + std::to_string( p_molecule.residueCount ) + " / "
							   + std::to_string( p_data.numGroups ) );
				}
				if ( p_molecule.getAtomCount() != p_molecule.atomCount || p_molecule.getAtomCount() != p_data.numAtoms )
				{
					VTX_ERROR( "Atom count error: " + std::to_string( p_molecule.getAtomCount() ) + " / "
							   + std::to_string( p_molecule.atomCount ) + " / " + std::to_string( p_data.numAtoms ) );
				}
				if ( p_molecule.getBondCount() != p_molecule.bondCount || p_molecule.getBondCount() != p_data.numBonds )
				{
					VTX_ERROR( "Bond count error: " + std::to_string( p_molecule.getBondCount() ) + " / "
							   + std::to_string( p_molecule.bondCount ) + " / " + std::to_string( p_data.numBonds ) );
				}
#endif

				VTX_INFO( "Models created" );

				return true;
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
