#include <algorithm>
#include <archive.h>
#include <archive_entry.h>
#include <fstream>
#include <iostream>
#include <util/exceptions.hpp>
#include <vtx/secondary_structure/files.hpp>

namespace pdb100
{
	namespace
	{
		std::vector<const char *> g_exceptionList {
			"5zkl", // chemfiles crashed
			"5zkm", // chemfiles crashed
			"2zi0", // chemfiles crashed
			"7zgr", // chemfiles crashed
			"7zgp", // chemfiles crashed
			"3zgz", // chemfiles crashed
			"5ze0", // chemfiles crashed
			"3zc0", // chemfiles crashed
		};
		bool notAnException( const fs::path & f )
		{
			std::string code( f.stem().string().data(), 4 );
			return std::find_if(
					   g_exceptionList.begin(), g_exceptionList.end(), [ &code ]( const char * e ) { return code == e; }
				   )
				   == std::end( g_exceptionList );
		}
	} // namespace

	/**
	 * @brief Recursive function. If file, add it to collection. If dir, call itself on it. The directory depth is
	 * supposed to be fixed at 2 so the stack size souldn't explose.
	 * @param contextData
	 * @param dir
	 */
	void walkDir( FileCollection & contextData, const std::filesystem::path & dir )
	{
		static const uint64_t skippy = 220650;
		for ( auto & it_fsItem : fs::directory_iterator( dir ) )
		{
			if ( contextData.size() > skippy )
				break;
			if ( fs::is_directory( it_fsItem ) )
				walkDir( contextData, it_fsItem.path() );
			if ( fs::is_regular_file( it_fsItem ) and notAnException( it_fsItem ) )
				contextData.push_back( it_fsItem.path().string() );
		}
	}
	/**
	 * @brief Open each directory from the dbDir and list the files in the pdb100_system collection
	 * @param contextData
	 */
	void enumerateFiles( Context & contextData ) { walkDir( *contextData.pdb100_system.open(), contextData.dbDir ); }

	void decompressFile( const fs::path & src, const fs::path & dest )
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
} // namespace pdb100
