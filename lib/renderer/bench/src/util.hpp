#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <core/chemdb/atom.hpp>
#include <core/chemdb/color.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <io/reader/molecule.hpp>
#include <io/util/secondary_structure.hpp>
#include <numeric>
#include <util/constants.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/network.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Bench
{
	Core::Struct::Molecule loadMolecule( const FilePath & p_filename )
	{
		IO::Reader::Molecule   reader;
		Core::Struct::Molecule molecule;

		reader.readFile( VTX::Util::Filesystem::getExecutableDir() / "data" / p_filename, molecule );

		return molecule;
	}

	Core::Struct::Molecule downloadMolecule( const std::string & p_pdb )
	{
		IO::Reader::Molecule   reader;
		Core::Struct::Molecule molecule;
		std::string			   data;

		VTX::Util::Network::httpRequestGet( "https://mmtf.rcsb.org/v1.0/full/" + p_pdb, data );
		reader.readBuffer( data, p_pdb + ".mmtf", molecule );

		return molecule;
	}

	// Skybox.
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
