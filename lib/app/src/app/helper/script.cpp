#include "app/helper/script.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <algorithm>
#include <filesystem>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <utility>

namespace VTX::App::Helper::Script
{
	bool isReadOnly( const FilePath & p_path )
	{
		const FilePath builtIn = std::filesystem::weakly_canonical( SESSION().getDefaultScriptsDir() );
		const FilePath path	   = std::filesystem::weakly_canonical( p_path );
		const std::pair<FilePath::const_iterator, FilePath::const_iterator> mismatch
			= std::mismatch( builtIn.begin(), builtIn.end(), path.begin(), path.end() );
		return mismatch.first == builtIn.end();
	}

	namespace
	{
		FilePath _copyTo( const FilePath & p_path, const FilePath & p_directory )
		{
			std::filesystem::create_directories( p_directory );
			FilePath destination = p_directory / p_path.filename();
			Util::Filesystem::generateUniqueFileName( destination );
			std::filesystem::copy_file( p_path, destination );
			VTX_INFO( "Script copied to {}", destination.string() );
			return destination;
		}
	} // namespace

	FilePath copyToUserDirectory( const FilePath & p_path ) { return _copyTo( p_path, SESSION().getScriptsDir() ); }

	FilePath duplicate( const FilePath & p_path )
	{ return _copyTo( p_path, isReadOnly( p_path ) ? SESSION().getScriptsDir() : p_path.parent_path() ); }
} // namespace VTX::App::Helper::Script
