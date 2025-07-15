#include <core/chemdb/secondary_structure.hpp>
#include <iostream>
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

		parse( decompressedFile, p_system.strands );
		parse( decompressedFile, p_system.helixes );

		if ( p_system.strands.empty() and p_system.helixes.empty() )
		{
			std::cout << "Not secondary struct for <" << std::string_view(p_system.code, sizeof(p_system.code)) << ">\n";
			return;
		}

		VTX::IO::Reader::System reader;

		reader.readFile( decompressedFile, p_system.system );
		size_t resIdx = 0;
		for ( auto & resSsType : p_system.system.residueSecondaryStructureTypes )
		{
			if ( resSsType != VTX::Core::ChemDB::SecondaryStructure::TYPE::UNKNOWN )
				throw std::runtime_error(
					fmt::format(
						"Chemfile System <{}> has already secondary structure informations for residue <{}>",
						p_system.code,
						resIdx
					)
				);
			resIdx++;
		}
	}
	void testSystem( const fs::path & p_systemPath, SystemMap & p_systemMap )
	{
		std::string systemName = p_systemPath.stem().string();
		systemName			   = std::string( systemName.data(), systemName.find( '.' ) );

		uint32_t idx = *reinterpret_cast<uint32_t *>( systemName.data() );
		if ( systemName.size() > 4 )
			throw std::exception( std::format( "System name <{}> larger than 4 char", systemName ).c_str() );
		if ( p_systemMap.contains( idx ) )
			throw std::exception( std::format( "System <{}> already in map", systemName ).c_str() );
		p_systemMap.emplace( idx, System {} );

		System & newSystem = p_systemMap.at( idx );
		memcpy_s( newSystem.code, sizeof( newSystem.code ), systemName.data(), systemName.size() );

		testSystem( p_systemPath, newSystem );
	}
} // namespace pdb100
