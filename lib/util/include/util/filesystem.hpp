#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "types.hpp"
#include <set>

namespace VTX::Util::Filesystem
{
	FilePath getExecutableDir();

	const std::string readPath( const FilePath & p_filePath );
	void			  removeAll( const FilePath & p_filePath );
	void			  generateUniqueFileName( FilePath & p_filePath );
	void			  copyDirectory( const FilePath & p_filePathSrc, const FilePath & p_filePathDestination );

} // namespace VTX::Util::Filesystem

#endif
