#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <core/struct/system.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	Core::Struct::System loadMolecule( const FilePath & p_filename );
	Core::Struct::System downloadMolecule( const std::string & p_pdb );

	// Skybox tests.
	/*
	const FilePath				  pathSkybox( std::filesystem::current_path() / "assets/skybox" );
	const std::array<FilePath, 6> pathImages
		= { pathSkybox / "right.jpg",  pathSkybox / "left.jpg",	 pathSkybox / "top.jpg",
			pathSkybox / "bottom.jpg", pathSkybox / "front.jpg", pathSkybox / "back.jpg" };
	std::array<unsigned char *, 6> images;
	int							   width, height, nrChannels;
	try
	{
		for ( size_t i = 0; i < pathImages.size(); ++i )
		{
			images[ i ] = stbi_load( pathImages[ i ].string().c_str(), &width, &height, &nrChannels, 0 );
		}
		renderer.loadSkybox( images, width, height );
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "Skybox not found: {}", p_e.what() );
	}
	for ( size_t i = 0; i < images.size(); ++i )
	{
		stbi_image_free( images[ i ] );
	}
	*/

} // namespace VTX::Bench

#endif
