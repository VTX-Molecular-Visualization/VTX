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
			// Decode MMTF.
			mmtf::StructureData data;
			mmtf::decodeFromFile( data, p_path.c_str() );

			// Check for consistency.
			if ( data.hasConsistentData( true ) )
			{ VTX_INFO( "MMTF successfully read " + p_path.getFileName() ); }
			else
			{
				throw Exception::MMTFException( "Inconsistent file: "
												+ p_path.getFileName() );
			}

			// Set molecule properties.
			p_molecule.setName( data.title );
			uint chainCount = data.chainsPerModel[ 0 ];

			float x, y, z;
			uint  residueGlobalIdx = 0;
			uint  atomGlobalIdx	   = 0;
			uint  bondGlobalIdx	   = 0;

			// For each chain in the model 0.
			for ( uint chainIdx = 0; chainIdx < chainCount; ++chainIdx )
			{
				// New chain.
				Model::Chain & chain = p_molecule.addChain();
				// chain.m_refMolecule= &mol;
				// chain.m_idFirstResidue = residueId;
				// chain.m_nbResidues	   = data.groupsPerChain[ chainId ];
				chain.setId( chainIdx );
				chain.setName( data.chainNameList[ chainIdx ] );
				uint residueCount = data.groupsPerChain[ chainIdx ];

				// For each residue in the chain
				for ( uint residueLocalIdx = 0; residueLocalIdx < residueCount;
					  ++residueLocalIdx, ++residueGlobalIdx )
				{
					const mmtf::GroupType & group
						= data.groupList
							  [ data.groupTypeList[ residueGlobalIdx ] ];

					// New residue.
					Model::Residue & residue = p_molecule.addResidue();
					residue.setId( residueGlobalIdx );
				}
			}
		}

	} // namespace IO
} // namespace VTX
