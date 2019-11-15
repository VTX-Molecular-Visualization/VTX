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
		void ReaderMMTF::readFile( const Path & p_path, Model::ModelMolecule & p_molecule )
		{
			VTX_INFO( "Loading " + p_path.getFileName() + "..." );

			// Decode MMTF.
			mmtf::StructureData data;
			mmtf::decodeFromFile( data, p_path.c_str() );

			// Check for consistency.
			if ( data.hasConsistentData( true ) ) { VTX_INFO( "File loaded" ); }
			else
			{
				throw Exception::MMTFException( "Inconsistent file: " + p_path.getFileName() );
			}

			VTX_INFO( "Creating models..." );

			// Set molecule properties.
			p_molecule.setName( data.title );

			float x, y, z;
			uint  chainGlobalIdx   = 0;
			uint  residueGlobalIdx = 0;
			uint  atomGlobalIdx	   = 0;
			uint  bondGlobalIdx	   = 0;

			// For each chain in the model 0.
			uint chainCount = data.chainsPerModel[ 0 ];
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
					residue.setSymbol( symbol.has_value() ? symbol.value()
														  : Model::ModelResidue::RESIDUE_SYMBOL::UNKNOWN );
					residue.setIdFirstAtom( atomGlobalIdx );
					residue.setAtomCount( uint( group.atomNameList.size() ) );
					residue.setIdFirstBond( bondGlobalIdx );
					residue.setBondCount( uint( group.bondAtomList.size() ) / 2u ); // 2 index by bond.
					if ( group.bondAtomList.size() % 2 != 0 ) { VTX_WARNING( "Incorrect number of bonds index" ); }

					// For each atom in the residue.
					uint atomCount = uint( group.atomNameList.size() );
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
						atom.setSymbol( symbol.has_value() ? symbol.value() : Model::ModelAtom::ATOM_SYMBOL::UNKNOWN );

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
		}

	} // namespace IO
} // namespace VTX
