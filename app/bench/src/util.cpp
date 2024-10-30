#include "util.hpp"
#include <io/reader/system.hpp>
#include <util/network.hpp>

namespace VTX::Bench
{
	Core::Struct::System loadSystem( const FilePath & p_filename )
	{
		IO::Reader::System	 reader;
		Core::Struct::System system;

		reader.readFile( VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, system );

		return system;
	}

	Core::Struct::System downloadSystem( const std::string & p_pdb )
	{
		IO::Reader::System	 reader;
		Core::Struct::System system;
		std::string			 data;

		VTX::Util::Network::httpRequestGet( "https://files.rcsb.org/download/" + p_pdb + ".pdb", &data );
		reader.readBuffer( data, p_pdb + ".pdb", system );

		return system;
	}

} // namespace VTX::Bench
