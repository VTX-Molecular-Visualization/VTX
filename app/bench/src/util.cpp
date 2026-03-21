#include "util.hpp"
#include <io/reader.hpp>
#include <util/filesystem.hpp>
#include <util/network.hpp>
#include <util/thread.hpp>

namespace VTX::Bench
{
	Core::Struct::System loadSystem( const FilePath & p_filename )
	{
		Util::StopToken		 t;
		IO::SystemReader	 reader( VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, t );
		Core::Struct::System system;
		reader.get( system );

		return system;
	}

	Core::Struct::System downloadSystem( const std::string & p_pdb )
	{
		IO::MemoryBuffer text;
		VTX::Util::Network::httpRequestGet( "https://files.rcsb.org/download/" + p_pdb + ".pdb", text );

		Util::StopToken		 t;
		IO::SystemReader	 reader( text, p_pdb + ".pdb", t );
		Core::Struct::System system;
		reader.get( system );
		return system;
	}

} // namespace VTX::Bench
