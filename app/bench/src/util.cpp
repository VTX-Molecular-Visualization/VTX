#include "util.hpp"
#include <io/reader/molecule.hpp>
#include <util/network.hpp>

namespace VTX::Bench
{
	Core::Struct::Molecule loadMolecule( const FilePath & p_filename )
	{
		IO::Reader::Molecule   reader;
		Core::Struct::Molecule molecule;

		reader.readFile( VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, molecule );

		return molecule;
	}

	Core::Struct::Molecule downloadMolecule( const std::string & p_pdb )
	{
		IO::Reader::Molecule   reader;
		Core::Struct::Molecule molecule;
		std::string			   data;

		VTX::Util::Network::httpRequestGet( "https://files.rcsb.org/download/" + p_pdb + ".pdb", data );
		reader.readBuffer( data, p_pdb + ".pdb", molecule );

		return molecule;
	}

} // namespace VTX::Bench
