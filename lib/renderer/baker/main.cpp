#include <cstdlib>
#include <filesystem>
#include <renderer/baker.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace
{
	using namespace VTX;

	int _bakeEnvironmentDirectoryToKtx( const VTX::FilePath & p_hdrRoot, const VTX::uint p_faceSize )
	{
		int failures = 0;
		int baked	 = 0;

		for ( const std::filesystem::directory_entry & entry :
			  std::filesystem::recursive_directory_iterator( p_hdrRoot ) )
		{
			if ( not entry.is_regular_file() || not VTX::Renderer::Baker::isEnvironmentMapFile( entry.path() ) )
			{
				continue;
			}

			const VTX::FilePath outputPath = VTX::FilePath( entry.path() ).replace_extension( ".ktx2" );
			if ( VTX::Renderer::Baker::bakeEnvironmentMapToKtx( entry.path(), outputPath, p_faceSize ) )
			{
				++baked;
			}
			else
			{
				++failures;
			}
		}

		if ( failures > 0 )
		{
			VTX_ERROR( "Baked {} HDR file(s), {} failure(s)", baked, failures );
		}
		else
		{
			VTX_INFO( "Baked {} HDR file(s)", baked );
		}

		return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	}
} // namespace

int main( const int argc, char ** argv )
{
	using namespace VTX;

	if ( argc != 2 )
	{
		VTX_ERROR( "Usage: baker-cli <rootPath>" );
		return EXIT_FAILURE;
	}

	const FilePath rootPath = std::filesystem::absolute( argv[ 1 ] );
	const FilePath hdrRoot	= rootPath / "hdr";
	if ( not std::filesystem::is_directory( hdrRoot ) )
	{
		VTX_ERROR( "HDR directory not found: {}", hdrRoot.string() );
		return EXIT_FAILURE;
	}

	return _bakeEnvironmentDirectoryToKtx( hdrRoot, Renderer::Baker::DEFAULT_ENVIRONMENT_FACE_SIZE );
}
