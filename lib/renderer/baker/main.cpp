#include <filesystem>
#include <iostream>
#include <renderer/baker.hpp>
#include <util/filesystem.hpp>

int main( const int argc, char ** argv )
{
	using namespace VTX;

	if ( argc != 2 )
	{
		std::cerr << "Usage: baker-cli <rootPath>\n";
		return EXIT_FAILURE;
	}

	const FilePath rootPath = std::filesystem::absolute( argv[ 1 ] );
	const FilePath hdrRoot	= rootPath / "hdr";
	if ( not std::filesystem::is_directory( hdrRoot ) )
	{
		std::cerr << "HDR directory not found: " << hdrRoot << "\n";
		return EXIT_FAILURE;
	}

	return Renderer::Baker::bakeEnvironmentDirectoryToKtx( hdrRoot, Renderer::Baker::DEFAULT_ENVIRONMENT_FACE_SIZE );
}
