#include <core/chemdb/secondary_structure.hpp>
#include <thread>
//
#include <fmt/format.h>
#include <io/reader/system.hpp>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/tests.hpp>

namespace pdb100
{

	void testSystem( const fs::path & p_systemPath, System & p_system )
	{
		std::stringstream strm;
		strm << "vtx_file_" << std::this_thread::get_id() << ".cif";
		fs::path decompressedFile = fs::temp_directory_path() / strm.str();
		if ( fs::exists( decompressedFile ) )
			fs::remove( decompressedFile );
		decompressFile( p_systemPath, decompressedFile );

		VTX::IO::Reader::System reader;

		reader.readFile( decompressedFile, p_system.system );
		size_t resIdx = 0;
		for ( auto & resSsType : p_system.system.residueSecondaryStructureTypes )
		{
			if ( resSsType != VTX::Core::ChemDB::SecondaryStructure::TYPE::UNKNOWN )
				throw std::exception(
					fmt::format(
						"System <{}> has already secondary structure informations for residue <{}>",
						p_system.code,
						resIdx
					)
						.c_str()
				);
			resIdx++;
		}

		parse( p_systemPath, p_system.strands );
		parse( p_systemPath, p_system.helixes );
	}
	void testSystem( const fs::path & p_systemPath, SystemMap & p_systemMap )
	{
		std::string systemName = p_systemPath.filename().string();
		uint32_t	idx		   = *reinterpret_cast<uint32_t *>( systemName.data() );
		if ( systemName.size() == 4 )
			throw std::exception( std::format( "System name <{}> larger than 4 char", systemName ).c_str() );
		if ( p_systemMap.contains( idx ) )
			throw std::exception( std::format( "System <{}> already in map", systemName ).c_str() );
		p_systemMap.emplace( idx, System {} );
		System & newSystem = p_systemMap.at( idx );
		testSystem( p_systemPath, newSystem );
	}
} // namespace pdb100
