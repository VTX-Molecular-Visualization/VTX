#include "io/file_format.hpp"
#include <util/filesystem.hpp>

namespace VTX::IO
{
	bool isScriptFileFormat( const FilePath & p_path )
	{ return VTX::Util::Filesystem::hasExtension( p_path, { ".py", ".vtx" } ); }

	bool isTrajectoryFileFormat( const FilePath & p_path )
	{ return VTX::Util::Filesystem::hasExtension( p_path, { ".xtc", ".dcd", ".lammpstrj", ".trj" } ); }
} // namespace VTX::IO
