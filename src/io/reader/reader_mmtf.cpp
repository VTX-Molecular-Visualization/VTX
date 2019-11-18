#include "reader_mmtf.hpp"
#include "../../defines.hpp"
#include "../../exceptions.hpp"
#include "../../lib/magic_enum.hpp"
#include "../../lib/mmtf/mmtf.hpp"
#include "../../util/logger.hpp"

namespace VTX
{
	namespace IO
	{
		bool ReaderMMTF::readFile( const Path & p_path, Model::ModelMolecule & p_molecule )
		{
			VTX_INFO( "Loading " + p_path.getFileName() + "..." );

			// Decode MMTF.
			mmtf::StructureData data;

			try
			{
				mmtf::decodeFromFile( data, p_path.c_str() );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not decode file: " + p_path.getFileName() );
				VTX_ERROR( p_e.what() );
				return false;
			}

			// Check for consistency.
			if ( data.hasConsistentData( true ) ) { VTX_INFO( "File loaded" ); }
			else
			{
				VTX_ERROR( "Inconsistent file: " + p_path.getFileName() );
				return false;
			}

			VTX_INFO( "Creating models..." );

			// Set molecule properties.
			p_molecule.setName( data.title );

			float		 x, y, z;
			Math::AABB & aabb			  = p_molecule.AABB();
			uint		 chainGlobalIdx	  = 0;
			uint		 residueGlobalIdx = 0;
			uint		 atomGlobalIdx	  = 0;
			uint		 bondGlobalIdx	  = 0;

			// For each chain in the model 0.
			uint chainCount = data.chainsPerModel[ 0 ];
#ifdef _DEBUG
			p_molecule.chainCount = chainCount;
#endif
			for ( ; chainGlobalIdx < chainCount; ++chainGlobalIdx )
			{
				// New chain.
				Model::ModelChain & chain = p_molecule.addChain();
				chain.setMoleculePtr( &p_molecule );
				chain.setId( chainGlobalIdx );
				chain.setName( data.chainNameList[ chainGlobalIdx ] );
				chain.setIdFirstResidue( residueGlobalIdx );
				chain.setResidueCount( data.groupsPerChain[ chainGlobalIdx ] );

				// For each residue in the chain.
				uint residueCount = data.groupsPerChain[ chainGlobalIdx ];
#ifdef _DEBUG
				p_molecule.residueCount += residueCount;
#endif
				for ( uint residueLocalIdx = 0; residueLocalIdx < residueCount; ++residueLocalIdx, ++residueGlobalIdx )
				{
					const mmtf::GroupType & group = data.groupList[ data.groupTypeList[ residueGlobalIdx ] ];

					// New residue.
					Model::ModelResidue & residue = p_molecule.addResidue();
					residue.setMoleculePtr( &p_molecule );
					residue.setChainPtr( &chain );
					residue.setId( residueGlobalIdx );
					const std::string & residueSymbol = group.groupName;
					std::optional symbol = magic_enum::enum_cast<Model::ModelResidue::RESIDUE_SYMBOL>( residueSymbol );
					symbol.has_value() ? residue.setSymbol( symbol.value() )
									   : p_molecule.addUnknownResidueSymbol( residueSymbol );
					residue.setIdFirstAtom( atomGlobalIdx );
					residue.setAtomCount( uint( group.atomNameList.size() ) );
					residue.setIdFirstBond( bondGlobalIdx );
					residue.setBondCount( uint( group.bondAtomList.size() ) / 2u ); // 2 index by bond.
					if ( group.bondAtomList.size() % 2 != 0 ) { VTX_WARNING( "Incorrect number of bonds index" ); }

					// For each atom in the residue.
					uint atomCount = uint( group.atomNameList.size() );
#ifdef _DEBUG
					p_molecule.atomCount += atomCount;
#endif
					for ( uint atomIdx = 0; atomIdx < atomCount; ++atomIdx, ++atomGlobalIdx )
					{
						// New atom.
						Model::ModelAtom & atom = p_molecule.addAtom();
						atom.setMoleculePtr( &p_molecule );
						atom.setChainPtr( &chain );
						atom.setResiduePtr( &residue );
						atom.setId( atomGlobalIdx );
						const std::string & atomSymbol = group.elementList[ atomIdx ];
						std::optional		symbol = magic_enum::enum_cast<Model::ModelAtom::ATOM_SYMBOL>( atomSymbol );
						symbol.has_value() ? atom.setSymbol( symbol.value() )
										   : p_molecule.addUnknownAtomSymbol( atomSymbol );

						x = data.xCoordList[ atomGlobalIdx ];
						y = data.yCoordList[ atomGlobalIdx ];
						z = data.zCoordList[ atomGlobalIdx ];

						Vec3f & atomPosition = p_molecule.addAtomPosition( Vec3f( x, y, z ) );
						p_molecule.addAtomRadius( atom.getVdwRadius() );
						p_molecule.addAtomColor( *atom.getColor() );

						// Extends bounding box along atom position.
						aabb.extend( atomPosition );
					}

					// For each bond in the residue.
					uint bondCount = residue.getBondCount();
#ifdef _DEBUG
					p_molecule.bondCount += bondCount * 2;
#endif
					for ( uint boundIdx = 0; boundIdx < bondCount * 2; boundIdx += 2, bondGlobalIdx += 2 )
					{
						p_molecule.addBond( bondGlobalIdx + group.bondAtomList[ boundIdx ] );
						p_molecule.addBond( bondGlobalIdx + group.bondAtomList[ boundIdx + 1u ] );
					}
				}
			}

			// Add all atom's bonds.
			p_molecule.addBonds( data.bondAtomList );

#ifdef _DEBUG
			p_molecule.bondCount += data.bondAtomList.size();
#endif

#ifdef _DEBUG
			if ( p_molecule.getChainCount() != p_molecule.chainCount )
			{
				VTX_ERROR( "p_molecule.getChainCount() != p_molecule.chainCount : "
						   + std::to_string( p_molecule.getChainCount() )
						   + " != " + std::to_string( p_molecule.chainCount ) );
			}
			if ( p_molecule.getResidueCount() != p_molecule.residueCount )
			{
				VTX_ERROR( "p_molecule.getResidueCount() != p_molecule.residueCount : "
						   + std::to_string( p_molecule.getResidueCount() )
						   + " != " + std::to_string( p_molecule.residueCount ) );
			}
			if ( p_molecule.getAtomCount() != p_molecule.atomCount )
			{
				VTX_ERROR( "p_molecule.getAtomCount() != p_molecule.atomCount : "
						   + std::to_string( p_molecule.getAtomCount() )
						   + " != " + std::to_string( p_molecule.atomCount ) );
			}
			if ( p_molecule.getBondCount() != p_molecule.bondCount )
			{
				VTX_ERROR( "p_molecule.getBondCount() != p_molecule.bondCount : "
						   + std::to_string( p_molecule.getBondCount() )
						   + " != " + std::to_string( p_molecule.bondCount ) );
			}
#endif

			// Display unknowm symbols.
			std::set<std::string> & unknownResidueSymbols = p_molecule.getUnknownResidueSymbols();
			if ( unknownResidueSymbols.empty() == false )
			{
				std::string unknownResidueSymbolsStr = "";
				for ( std::string symbol : unknownResidueSymbols )
				{
					unknownResidueSymbolsStr += symbol + " ";
				}
				VTX_WARNING( "Unknown residue symbols : " + unknownResidueSymbolsStr );
			}

			std::set<std::string> & unknownAtomSymbols = p_molecule.getUnknownAtomSymbols();
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

	} // namespace IO
} // namespace VTX
