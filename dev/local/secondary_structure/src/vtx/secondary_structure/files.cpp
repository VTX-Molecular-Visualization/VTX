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
		static const uint64_t maxFiles = 55;
		static const uint64_t skippy   = 250;
		uint64_t			  count	   = 0;
		for ( auto & it_fsItem : fs::directory_iterator( dir ) )
		{
			count++;
			if ( count < skippy )
				continue;
			if ( contextData.size() > maxFiles )
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

} // namespace pdb100
