#include "util.hpp"
#include <core/chemdb/category.hpp>
#include <io/reader.hpp>
#include <util/filesystem.hpp>
#include <util/network.hpp>
#include <util/thread/base_thread.hpp>

namespace VTX::Bench
{
	LoadedSystem loadSystem( const FilePath & p_filename )
	{
		Util::Thread::StopToken t;
		IO::SystemReader		reader(
			   VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, IO::READER_OPTION::ALL, t
		   );
		LoadedSystem system;
		auto		 d = Core::ChemDB::Category::createDefaultDictionary();
		reader.get( d, system.topology, system.metadata );
		reader.get( system.positions );

		return system;
	}

	LoadedSystem downloadSystem( const std::string & p_pdb )
	{
		IO::MemoryBuffer text;
		VTX::Util::Network::httpRequestGet( "https://files.rcsb.org/download/" + p_pdb + ".pdb", text );

		Util::Thread::StopToken t;
		IO::SystemReader		reader( std::move( text ), p_pdb + ".pdb", IO::READER_OPTION::ALL, t );
		LoadedSystem			system;
		auto					d = Core::ChemDB::Category::createDefaultDictionary();
		reader.get( d, system.topology, system.metadata );
		reader.get( system.positions );
		return system;
	}

} // namespace VTX::Bench
