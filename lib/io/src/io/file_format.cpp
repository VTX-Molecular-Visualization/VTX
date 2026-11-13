#include "io/file_format.hpp"
#include <algorithm>
#include <functional>
#include <string>
#include <util/filesystem.hpp>
#include <util/string.hpp>

#ifdef _MSC_VER
#pragma warning( push, 0 )
#endif
#include <chemfiles.hpp>
#ifdef _MSC_VER
#pragma warning( pop )
#endif

namespace VTX::IO
{
	bool isScriptFileFormat( const FilePath & p_path )
	{ return VTX::Util::Filesystem::hasExtension( p_path, { ".py", ".vtx" } ); }

	bool isTrajectoryFileFormat( const FilePath & p_path )
	{ return VTX::Util::Filesystem::hasExtension( p_path, { ".xtc", ".dcd", ".lammpstrj", ".trj" } ); }
} // namespace VTX::IO
