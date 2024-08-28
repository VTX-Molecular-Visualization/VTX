#ifndef __VTX_IO_INTERNAL_FILESYSTEM__
#define __VTX_IO_INTERNAL_FILESYSTEM__

#include <string>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::IO::Internal::Filesystem
{
	// JSon save fail when size > 192
	static const int		 MAX_FILE_LENGTH	  = 180;
	static const std::string REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

	enum class FILE_TYPE_ENUM : int
	{
		SCENE,
		CONFIGURATION,
		MOLECULE,
		TRAJECTORY,
		MESH,
		SCRIPT,
		UNKNOWN,
		COUNT
	};

	inline FILE_TYPE_ENUM getFileTypeFromFilePath( const FilePath & p_path )
	{
		const FilePath extension = p_path.extension();

		if ( extension == ".vtx" )
		{
			return FILE_TYPE_ENUM::SCENE;
		}
		else if ( extension == ".prm" || extension == ".psf" )
		{
			return FILE_TYPE_ENUM::CONFIGURATION;
		}
		else if ( extension == ".cif" || extension == ".cml" || extension == ".cssr" || extension == ".gro"
				  || extension == ".mmcif" || extension == ".mmtf" || extension == ".mol2" || extension == ".molden"
				  || extension == ".pdb" || extension == ".sdf" || extension == ".smi" || extension == ".mmtf"
				  || extension == ".xyz" )
		{
			return FILE_TYPE_ENUM::MOLECULE;
		}
		else if ( extension == ".obj" )
		{
			return FILE_TYPE_ENUM::MESH;
		}
		else if ( extension == ".nc" || extension == ".dcd" || extension == ".lammpstrj" || extension == ".arc"
				  || extension == ".trr" || extension == ".xtc" || extension == ".tng" || extension == ".trj" )
		{
			return FILE_TYPE_ENUM::TRAJECTORY;
		}
		else if ( extension == ".py" )
		{
			return FILE_TYPE_ENUM::SCRIPT;
		}
		else
		{
			return FILE_TYPE_ENUM::UNKNOWN;
		}
	}

	inline void fillFilepathPerMode(
		std::vector<FilePath>				 p_filepaths,
		std::vector<std::vector<FilePath>> & p_filepathPerMode
	)
	{
		p_filepathPerMode.resize( int( FILE_TYPE_ENUM::COUNT ) );

		for ( const FilePath & path : p_filepaths )
		{
			const FILE_TYPE_ENUM filetype = getFileTypeFromFilePath( path );
			p_filepathPerMode[ int( filetype ) ].emplace_back( path );
		}
	}

} // namespace VTX::IO::Internal::Filesystem

#endif
