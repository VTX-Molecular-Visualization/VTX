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

				// Reserve memory for vectors to avoid pointer loss.
				p_molecule.addChains( p_data.numChains );
				p_molecule.addResidues( p_data.numGroups );
				p_molecule.addAtoms( p_data.numAtoms );

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
						residue.setIdFirstBond( bondGlobalIdx );
						residue.setBondCount( uint( group.bondAtomList.size() ) / 2u ); // 2 index by bond.
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
							p_molecule.addAtomRadius( atomRadius );

							// Extends bounding box along atom position.
							p_molecule.extendAABB( atomPosition, atomRadius );
						}

						// For each bond in the residue.
						uint bondCount = residue.getBondCount();
#ifdef _DEBUG
						p_molecule.bondCount += bondCount * 2;
#endif
						for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, bondGlobalIdx += 2 )
						{
							p_molecule.addBond( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx ] );
							p_molecule.addBond( residue.getIdFirstAtom() + group.bondAtomList[ boundIdx + 1u ] );
						}
					}
				}

				// Add all atom's bonds.
				p_molecule.addBonds( p_data.bondAtomList );

#ifdef _DEBUG
				p_molecule.bondCount += (uint)p_data.bondAtomList.size();
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
				if ( p_molecule.getBondCount() != p_molecule.bondCount
					 || p_molecule.getBondCount() != p_data.numBonds * 2 )
				{
					VTX_ERROR( "Bond count error: " + std::to_string( p_molecule.getBondCount() ) + " / "
							   + std::to_string( p_molecule.bondCount ) + " / "
							   + std::to_string( p_data.numBonds * 2 ) );
				}
#endif

				// Display unknown symbols.
				const std::unordered_set<std::string> & unknownResidueSymbols = p_molecule.getUnknownResidueSymbols();
				if ( unknownResidueSymbols.empty() == false )
				{
					std::string unknownResidueSymbolsStr = "";
					for ( std::string symbol : unknownResidueSymbols )
					{
						unknownResidueSymbolsStr += symbol + " ";
					}
					VTX_WARNING( "Unknown residue symbols : " + unknownResidueSymbolsStr );
				}

				const std::unordered_set<std::string> & unknownAtomSymbols = p_molecule.getUnknownAtomSymbols();
				if ( unknownAtomSymbols.empty() == false )
				{
					std::string unknownAtomSymbolsStr = "";
					for ( std::string symbol : unknownAtomSymbols )
					{
						unknownAtomSymbolsStr += symbol + " ";
					}
					VTX_WARNING( "Unknown atom symbols : " + unknownAtomSymbolsStr );
				}

				VTX_INFO( "Models created" );

				return true;
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
