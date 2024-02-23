#ifndef __VTX_BENCH_UTIL__
#define __VTX_BENCH_UTIL__

#include <core/chemdb/atom.hpp>
#include <core/chemdb/color.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <io/reader/molecule.hpp>
#include <io/util/secondary_structure.hpp>
#include <numeric>
#include <renderer/proxy/molecule.hpp>
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

		reader.readFile( VTX::Util::Filesystem::getExecutableDir() / p_filename, molecule );

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

	Renderer::Proxy::Molecule proxify( const Core::Struct::Molecule & p_molecule )
	{
		const size_t									sizeAtoms	= p_molecule.trajectory.frames.front().size();
		const std::vector<Core::ChemDB::Atom::SYMBOL> & symbols		= p_molecule.atomSymbols;
		const size_t									sizeResidue = p_molecule.residueOriginalIds.size();

		// Generate.
		static std::vector<std::unique_ptr<const std::vector<float>>> vecRadii;
		static std::vector<std::unique_ptr<const std::vector<uchar>>> vecColors;
		static std::vector<std::unique_ptr<const std::vector<bool>>>  vecVisibilities;
		static std::vector<std::unique_ptr<const std::vector<bool>>>  vecSelections;
		static std::vector<std::unique_ptr<const std::vector<uint>>>  vecIdAtoms;
		static std::vector<std::unique_ptr<const std::vector<uint>>>  vecIdResidues;
		static std::vector<std::unique_ptr<const std::vector<uchar>>> vecColorResidues;

		auto   colorAtoms = std::make_unique<std::vector<uchar>>( sizeAtoms );
		size_t i		  = 0;
		std::generate(
			colorAtoms->begin(),
			colorAtoms->end(),
			[ & ] { return Core::ChemDB::Color::getColorIndex( symbols[ i++ ] ); }
		);
		vecColors.emplace_back( std::move( colorAtoms ) );

		vecVisibilities.emplace_back( std::make_unique<const std::vector<bool>>( sizeAtoms, true ) );
		vecSelections.emplace_back( std::make_unique<const std::vector<bool>>( sizeAtoms, false ) );

		auto radii = std::make_unique<std::vector<float>>( sizeAtoms );
		i		   = 0;
		std::generate(
			radii->begin(),
			radii->end(),
			[ & ] { return Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ]; }
		);
		vecRadii.emplace_back( std::move( radii ) );

		auto idAtoms = std::make_unique<std::vector<uint>>( sizeAtoms );
		std::iota( idAtoms->begin(), idAtoms->end(), 0 );
		vecIdAtoms.emplace_back( std::move( idAtoms ) );

		auto idResidues = std::make_unique<std::vector<uint>>( sizeResidue );
		std::iota( idResidues->begin(), idResidues->end(), 0 );
		vecIdResidues.emplace_back( std::move( idResidues ) );

		auto colorResidues = std::make_unique<std::vector<uchar>>( sizeResidue );
		i				   = 0;
		std::generate(
			colorResidues->begin(),
			colorResidues->end(),
			[ & ] { return Core::ChemDB::Color::getColorIndex( p_molecule.residueSecondaryStructureTypes[ i++ ] ); }
		);
		vecColorResidues.emplace_back( std::move( colorResidues ) );

		return { &p_molecule.transform,
				 &p_molecule.trajectory.frames.front(),
				 vecColors.back().get(),
				 vecRadii.back().get(),
				 vecVisibilities.back().get(),
				 vecSelections.back().get(),
				 vecIdAtoms.back().get(),
				 &p_molecule.bondPairAtomIndexes,
				 &p_molecule.atomNames,
				 vecIdResidues.back().get(),
				 reinterpret_cast<const std::vector<uchar> *>( &p_molecule.residueSecondaryStructureTypes ),
				 vecColorResidues.back().get(),
				 &p_molecule.residueFirstAtomIndexes,
				 &p_molecule.residueAtomCounts,
				 &p_molecule.chainFirstResidues,
				 &p_molecule.chainResidueCounts };
	}

	// Grid.
	/*
	Molecule generateAtomGrid( int p_size )
	{
		if ( p_size % 2 == 0 )
		{
			++p_size;
		}

		const size_t realSize = p_size * p_size * p_size;

		std::vector<Vec3f> positions( realSize );
		std::vector<uchar> colors( realSize );
		std::vector<uint>  bonds( ( realSize - 1 ) * 2 );

		size_t		counter = 0;
		const float offset	= 2.f;

		for ( int i = -p_size / 2; i <= p_size / 2; ++i )
		{
			for ( int j = -p_size / 2; j <= p_size / 2; ++j )
			{
				for ( int k = -p_size / 2; k <= p_size / 2; ++k )
				{
					positions[ counter ] = Vec3f( i * offset, j * offset, k * offset );
					colors[ counter ]	 = uchar( rand() % 256 );
					if ( counter < realSize - 1 )
					{
						bonds[ counter * 2 ]	 = uint( counter );
						bonds[ counter * 2 + 1 ] = uint( counter + 1 );
					}
					++counter;
				}
			}
		}

		assert( counter == realSize );

		std::vector<uint> ids( realSize );
		std::iota( ids.begin(), ids.end(), 0 );

		VTX_INFO( "{} atoms and {} bonds generated", realSize, bonds.size() / 2 );

		return { MAT4F_ID,
				 positions,
				 colors,
				 std::vector<float>( realSize, 0.5f ),
				 std::vector<bool>( realSize, true ),
				 std::vector<bool>( realSize, false ),
				 ids,
				 bonds };
	}
	*/

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
