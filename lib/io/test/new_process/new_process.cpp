#include "test/process/shared.hpp"
#include <archive.h>
#include <archive_entry.h>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <filesystem>
#include <fstream>
#include <io/reader/system.hpp>
#include <io/writer/molecule.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>

namespace fs = std::filesystem;

namespace
{
	/**
	 * @brief Use libarchive to decompress the directory , take the first file and write it at destination.
	 * @param src
	 * @param dest
	 */
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
	/**
	 * @brief Try read, write, and re-read the input file and give back the results. May crash the entire process beyond
	 * repair.
	 * @param structureFile
	 * @param rslt
	 */
	void testFile( const VTX::FilePath & structureFile, VTX::IO::test::RereadResult & rslt )
	{
		using namespace VTX;
		using namespace VTX::IO;
		using namespace VTX::IO::Writer;
		using namespace VTX::IO::test;

		const std::string moleculeName	   = structureFile.stem().string();
		const std::string moleculePathname = moleculeName + structureFile.extension().string();

		VTX::Core::Struct::System molecule = VTX::Core::Struct::System();
		{
			IO::Reader::System moleculeReader = IO::Reader::System();

			moleculeReader.readFile( structureFile, molecule );
		}
		uint64_t init_atomCount	 = molecule.getAtomCount();
		uint64_t init_chainCount = molecule.getChainCount();
		uint64_t init_frameCount = molecule.trajectory.getFrameCount();
		uint64_t init_bondCount	 = molecule.getBondCount();
		uint64_t init_resCount	 = molecule.getResidueCount();

		fs::remove( structureFile );

		writeFile( WriteArgs {
			.destination = structureFile,
			.format		 = E_FILE_FORMATS::none,
			.molecule	 = &molecule,
		} );

		VTX::Core::Struct::System molecule_reread		  = VTX::Core::Struct::System();
		IO::Reader::System		moleculeReader_reread = IO::Reader::System();

		moleculeReader_reread.readFile( structureFile, molecule_reread );

		bool numMismatch_atom  = init_atomCount != molecule_reread.getAtomCount();
		bool numMismatch_chain = init_chainCount != molecule_reread.getChainCount();
		bool numMismatch_frame = init_frameCount != molecule_reread.trajectory.getFrameCount();
		bool numMismatch_bond  = init_bondCount != molecule_reread.getBondCount();
		bool numMismatch_res   = init_resCount != molecule_reread.getResidueCount();

		rslt = ( numMismatch_atom * RereadResult::atom_mismatch ) | ( numMismatch_chain * RereadResult::chain_mismatch )
			   | ( numMismatch_res * RereadResult::residue_mismatch )
			   | ( numMismatch_frame * RereadResult::frame_mismatch );

		// Bond are not reliably written in files so we won't check them.
		// e.g. 2qwo has disulfide bond that is not retrieved when reloading the file
	}

	/**
	 * @brief Mark the input structure as a crashing structure. If the process ends up processing the structure without
	 * crash, this entry will be used to fill results.
	 * @param str
	 * @param rslt
	 */
	void writeResultEntry( const std::string & str, const VTX::IO::test::RereadResult & rslt )
	{
		using namespace boost::interprocess;
		using namespace VTX::IO::test;
		named_mutex									  mutex( open_or_create, SHM_REREADRSLT_MUTEX );
		scoped_lock<boost::interprocess::named_mutex> lock( mutex );

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::open_only, VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME
		);
		auto rsltMapPair = sharedSegment.find<RereadResultMap>( SHM_REREADRSLT_MAP_OBJNAME );
		if ( rsltMapPair.first == nullptr )
			throw;

		size_t key = std::hash<std::string>()( str );
		if ( not rsltMapPair.first->contains( key ) )
			rsltMapPair.first->emplace( key, rslt );
		else
			rsltMapPair.first->at( key ) = rslt;
	}
	/**
	 * @brief Fill the input string with the content of the new structure file path acquired.
	 * @param out
	 */
	void acquireNewStructure( std::string & out )
	{
		using namespace boost::interprocess;
		using namespace VTX::IO::test;

		named_mutex									  mutex( open_or_create, SHM_FILESTR_MUTEX );
		scoped_lock<boost::interprocess::named_mutex> lock( mutex );

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::open_only, VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME
		);
		auto dequePair = sharedSegment.find<StringDeque>( SHM_FILESTR_DEQUE_OBJNAME );
		if ( dequePair.first == nullptr )
			throw;

		if ( dequePair.first->empty() )
			return;

		out.assign( dequePair.first->front().begin(), dequePair.first->front().end() );
		dequePair.first->pop_front();
	}
} // namespace

int main( int argc, char * argv[] )
{
	if ( argc != 2 )
		return 0;

	const fs::path outDir  = VTX::Util::Filesystem::getExecutableDir() / "out" / "pdb100";
	const fs::path tmpFile = outDir / ( std::string( "tmp_" ) + argv[ 1 ] + ".mmcif" );
	if ( not fs::exists( outDir ) )
		fs::create_directories( outDir );

	std::string					filePathStr;
	VTX::IO::test::RereadResult result = VTX::IO::test::RereadResult::crashed;
	acquireNewStructure( filePathStr );
	while ( not filePathStr.empty() )
	{
		result = VTX::IO::test::RereadResult::crashed;
		writeResultEntry( filePathStr, result );
		if ( fs::exists( tmpFile ) )
			fs::remove( tmpFile );

		decompressFile( filePathStr, tmpFile );
		try
		{
			testFile( tmpFile, result );
		}
		catch ( ... )
		{
		}
		writeResultEntry( filePathStr, result );

		filePathStr.clear();
		acquireNewStructure( filePathStr );
	}
	return 0;
}
