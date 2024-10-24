#include "test/process/shared.hpp"
#include <archive.h>
#include <archive_entry.h>
#include <filesystem>
#include <fstream>
#include <io/reader/molecule.hpp>
#include <io/writer/molecule.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>

namespace fs = std::filesystem;

namespace
{
	void decompressFile( const VTX::FilePath & src, const VTX::FilePath & dest )
	{
		std::vector<char> cpp_buffer;
		cpp_buffer.resize( fs::file_size( src ) );

		struct archive *	   a;
		struct archive_entry * entry;
		int					   r;

		a = archive_read_new();
		archive_read_support_filter_gzip( a );
		archive_read_support_format_raw( a );

		r = archive_read_open_file( a, src.string().c_str(), 10240 );
		if ( r != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Opening error.", src.string() );
		if ( archive_read_next_header( a, &entry ) != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Header readin error.", src.string() );

		size_t		  redBytes = 0;
		std::ofstream outStrm( dest );
		do
		{
			redBytes = archive_read_data( a, cpp_buffer.data(), cpp_buffer.size() );
			outStrm.write( cpp_buffer.data(), redBytes );
		} while ( redBytes != 0 );

		r = archive_read_free( a );
		if ( r != ARCHIVE_OK )
			throw VTX::VTXException( "Issue with archive <{}>. Freeing error.", src.string() );
	}

	void testFile( const VTX::FilePath & actualFileLocation, const std::string & testFile )
	{
		using namespace VTX;
		using namespace VTX::IO;
		using namespace VTX::IO::Writer;

		const std::string moleculeName	   = actualFileLocation.stem().string();
		const std::string moleculePathname = moleculeName + actualFileLocation.extension().string();

		VTX::Core::Struct::Molecule molecule = VTX::Core::Struct::Molecule();
		{
			IO::Reader::Molecule moleculeReader = IO::Reader::Molecule();

			moleculeReader.readFile( actualFileLocation, molecule );
		}
		uint64_t init_atomCount	 = molecule.getAtomCount();
		uint64_t init_chainCount = molecule.getChainCount();
		uint64_t init_frameCount = molecule.trajectory.getFrameCount();
		uint64_t init_bondCount	 = molecule.getBondCount();
		uint64_t init_resCount	 = molecule.getResidueCount();

		fs::remove( actualFileLocation );

		writeFile( WriteArgs {
			.destination = actualFileLocation,
			.format		 = E_FILE_FORMATS::none,
			.molecule	 = &molecule,
		} );

		VTX::Core::Struct::Molecule molecule_reread		  = VTX::Core::Struct::Molecule();
		IO::Reader::Molecule		moleculeReader_reread = IO::Reader::Molecule();

		moleculeReader_reread.readFile( actualFileLocation, molecule_reread );

		bool numMatch_atom	= init_atomCount == molecule_reread.getAtomCount();
		bool numMatch_chain = init_chainCount == molecule_reread.getChainCount();
		bool numMatch_frame = init_frameCount == molecule_reread.trajectory.getFrameCount();
		bool numMatch_bond	= init_bondCount == molecule_reread.getBondCount();
		bool numMatch_res	= init_resCount == molecule_reread.getResidueCount();

		if ( numMatch_atom && numMatch_chain && numMatch_frame && numMatch_bond && numMatch_res )
			contextData.fullyWorkingStructs.push_back( &testFile );
		else
		{
			if ( not numMatch_atom )
				contextData.atomNumNotMatch.push_back( &testFile );
			if ( not numMatch_chain )
				contextData.chainNumNotMatch.push_back( &testFile );
			if ( not numMatch_frame )
				contextData.frameNumNotMatch.push_back( &testFile );
			if ( not numMatch_res )
				contextData.residuesNumNotMatch.push_back( &testFile );
		}
		// Bond are not reliably written in files so we won't check them.
		// e.g. 2qwo has disulfide bond that is not retrieved when reloading the file
	}
} // namespace
int main( int argc, char argv[] ) {}
