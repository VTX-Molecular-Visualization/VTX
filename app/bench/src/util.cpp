#include "util.hpp"
#include <core/chemdb/category.hpp>
#include <io/reader.hpp>
#include <util/filesystem.hpp>
#include <util/network.hpp>
#include <util/thread.hpp>

namespace VTX::Bench
{
	LoadedSystem loadSystem( const FilePath & p_filename )
	{
		Util::StopToken	 t;
		IO::SystemReader reader( VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, t );
		LoadedSystem	 system;
		auto			 d = Core::ChemDB::Category::createDefaultDictionary();
		reader.get( d, system.topology );
		reader.get( system.positions );

		return system;
	}

	LoadedSystem downloadSystem( const std::string & p_pdb )
	{
		IO::MemoryBuffer text;
		VTX::Util::Network::httpRequestGet( "https://files.rcsb.org/download/" + p_pdb + ".pdb", text );

		Util::StopToken	 t;
		IO::SystemReader reader( text, p_pdb + ".pdb", t );
		LoadedSystem	 system;
		auto			 d = Core::ChemDB::Category::createDefaultDictionary();
		reader.get( d, system.topology );
		reader.get( system.positions );
		return system;
	}

} // namespace VTX::Bench
