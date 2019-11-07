#include "reader_mmtf.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
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
			for ( uint chainIdx = 0; chainIdx < chainCount; ++chainIdx )
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
					// const std::string & residueSymbol = group.groupName;
					// residue.setType( Residue::getTypeFromSymbol( resSymbol )
					// );

					// For each atom in the residue.
					uint atomCount = uint( group.atomNameList.size() );
					for ( uint atomIdx = 0; atomIdx < atomCount;
						  ++atomIdx, ++atomGlobalIdx )
					{
						// New atom.
						Model::Atom & atom = p_molecule.addAtom();
						atom.setId( atomGlobalIdx );
						// const std::string & atomSymbol = group.elementList[ k
						// ]; atom.m_type = Atom::getTypeFromSymbol( atomSymbol
						// );

						x = data.xCoordList[ atomGlobalIdx ];
						y = data.yCoordList[ atomGlobalIdx ];
						z = data.zCoordList[ atomGlobalIdx ];
					}
				}
			}
			VTX_INFO( "Models created" );
			std::cout << atomGlobalIdx << " atoms" << std::endl;
		}

	} // namespace IO
} // namespace VTX
