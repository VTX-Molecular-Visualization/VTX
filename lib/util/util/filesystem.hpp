#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "types.hpp"
#include <set>

namespace VTX::Util::Filesystem
{
	const std::string readPath( const FilePath & p_filePath );
	void			  removeAll( const FilePath & p_filePath );
	void			  generateUniqueFileName( FilePath & p_filePath );
} // namespace VTX::Util::Filesystem

#endif
