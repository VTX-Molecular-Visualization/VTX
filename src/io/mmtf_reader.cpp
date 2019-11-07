#include "mmtf_reader.hpp"
#include "../exceptions.hpp"
#include "../lib/mmtf/mmtf.hpp"
#include "../util/logger.hpp"

namespace VTX
{
	namespace IO
	{
		void MMTFReader::readFile( const Path &			  p_path,
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

			// Set properties.
			p_molecule.setName( data.title );
		}

	} // namespace IO
} // namespace VTX
