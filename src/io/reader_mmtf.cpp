#include "reader_mmtf.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
#include "../lib/magic_enum.hpp"
#include "../lib/mmtf/mmtf.hpp"
#include "../model/atom.hpp"
#include "../model/chain.hpp"
#include "../model/residue.hpp"
#include "../util/logger.hpp"

namespace VTX
{
	namespace IO
	{
		void ReaderMMTF::readFile( const Path &			  p_path,
								   Model::ModelMolecule & p_molecule )
		{
			VTX_INFO( "Loading " + p_path.getFileName() + "..." );

			// Decode MMTF.
			mmtf::StructureData data;
			mmtf::decodeFromFile( data, p_path.c_str() );

			// Check for consistency.
			if ( data.hasConsistentData( true ) ) { VTX_INFO( "File loaded" ); }
			else
			{
				throw Exception::MMTFException( "Inconsistent file: "
												+ p_path.getFileName() );
			}

			VTX_INFO( "Creating models..." );

			// Set molecule properties.
			p_molecule.setName( data.title );

			float x, y, z;
			uint  residueGlobalIdx = 0;
			uint  atomGlobalIdx	   = 0;
			uint  bondGlobalIdx	   = 0;

			// For each chain in the model 0.
			uint chainCount = data.chainsPerModel[ 0 ];
			uint chainIdx	= 0;
			for ( ; chainIdx < chainCount; ++chainIdx )
			{
				// New chain.
				Model::Chain & chain = p_molecule.addChain();
				chain.setId( chainIdx );
				chain.setName( data.chainNameList[ chainIdx ] );

				// For each residue in the chain.
				uint residueCount = data.groupsPerChain[ chainIdx ];
				for ( uint residueLocalIdx = 0; residueLocalIdx < residueCount;
					  ++residueLocalIdx, ++residueGlobalIdx )
				{
					const mmtf::GroupType & group
						= data.groupList
							  [ data.groupTypeList[ residueGlobalIdx ] ];

					// New residue.
					Model::Residue & residue = p_molecule.addResidue();
					residue.setId( residueGlobalIdx );
					const std::string & residueSymbol = group.groupName;
					auto				symbol
						= magic_enum::enum_cast<Model::Residue::RESIDUE_SYMBOL>(
							residueSymbol );
					residue.setSymbol(
						symbol.has_value()
							? symbol.value()
							: Model::Residue::RESIDUE_SYMBOL::UNKNOWN );

					// For each atom in the residue.
					uint atomCount = uint( group.atomNameList.size() );
					for ( uint atomIdx = 0; atomIdx < atomCount;
						  ++atomIdx, ++atomGlobalIdx )
					{
						// New atom.
						Model::Atom & atom = p_molecule.addAtom();
						atom.setId( atomGlobalIdx );
						const std::string & atomSymbol
							= group.elementList[ atomIdx ];
						auto symbol
							= magic_enum::enum_cast<Model::Atom::ATOM_SYMBOL>(
								atomSymbol );
						atom.setSymbol(
							symbol.has_value()
								? symbol.value()
								: Model::Atom::ATOM_SYMBOL::UNKNOWN );

						x = data.xCoordList[ atomGlobalIdx ];
						y = data.yCoordList[ atomGlobalIdx ];
						z = data.zCoordList[ atomGlobalIdx ];

						p_molecule.addAtomPosition( Vec3f( x, y, z ) );
						p_molecule.addAtomRadius( atom.getVdwRadius() );
						p_molecule.addAtomColor( *atom.getColor() );
					}
				}
			}
			VTX_INFO( "Models created" );
			VTX_INFO( "Chains: " + std::to_string( chainIdx ) );
			VTX_INFO( "Residues: " + std::to_string( residueGlobalIdx ) );
			VTX_INFO( "Atoms: " + std::to_string( atomGlobalIdx ) );
			// std::cout << atomGlobalIdx << " atoms" << std::endl;
		}

	} // namespace IO
} // namespace VTX
