

#include <archive.h>
#include <archive_entry.h>
#include <fstream>
#include <unordered_map>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <vector>

namespace fs = std::filesystem;

namespace pdb100
{
	struct Atom
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		char  name[ 4 ] { '\0' };
		char  symbol[ 2 ] { '\0' };
	};
	struct Residue
	{
		char	 res_3letterCode[ 3 ] { '\0' };
		uint32_t num = 0;
		char	 chain_name[ 3 ] { '\0' };
	};
	struct Helix
	{
		uint16_t resIdx_begin = 0xffffui16;
		uint16_t resIdx_end	  = 0xffffui16;
	};
	struct Strand
	{
		uint16_t resIdx_begin = 0xffffui16;
		uint16_t resIdx_end	  = 0xffffui16;
	};
	struct System
	{
		char				code[ 4 ] { '\0' };
		std::vector<Atom>	atoms;
		std::vector<Helix>	helixes;
		std::vector<Strand> strands;
	};
	using SystemMap		 = std::unordered_map<char[ 4 ], System>;
	using FileCollection = std::list<std::string>;

	struct Context
	{
		fs::path	   dbDir;
		FileCollection tested_structs;
		SystemMap	   results;
	};

	/**
	 * @brief Use libarchive to decompress the directory , take the first file and write it at destination.
	 * @param src
	 * @param dest
	 */
	void decompressFile( const VTX::FilePath & src, const VTX::FilePath & dest );

	/**
	 * @brief Open each directory from the dbDir and list the files in the tested_structs collection
	 * @param contextData
	 */
	void enumerateFiles( Context & contextData );
	/**
	 * @brief Recursive function. If file, add it to collection. If dir, call itself on it. The directory depth is
	 * supposed to be fixed at 2 so the stack size shouldn't explose.
	 * @param contextData
	 * @param dir
	 */
	void walkDir( FileCollection & contextData, const std::filesystem::path & dir );

} // namespace pdb100

int main() { return 0; }

namespace pdb100
{

	/**
	 * @brief Recursive function. If file, add it to collection. If dir, call itself on it. The directory depth is
	 * supposed to be fixed at 2 so the stack size souldn't explose.
	 * @param contextData
	 * @param dir
	 */
	void walkDir( FileCollection & contextData, const std::filesystem::path & dir )
	{
		for ( auto & it_fsItem : fs::directory_iterator( dir ) )
		{
			if ( fs::is_directory( it_fsItem ) )
				walkDir( contextData, it_fsItem.path() );
			if ( fs::is_regular_file( it_fsItem ) )
				contextData.push_back( it_fsItem.path().string() );
		}
	}
	/**
	 * @brief Open each directory from the dbDir and list the files in the tested_structs collection
	 * @param contextData
	 */
	void enumerateFiles( Context & contextData ) { walkDir( contextData.tested_structs, contextData.dbDir ); }

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
} // namespace pdb100
