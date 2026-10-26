#ifndef VTX_APP_HELPER_IO_HPP
#define VTX_APP_HELPER_IO_HPP

#include <util/filesystem.hpp>

namespace VTX::App::Helper::IO
{
	enum class FileDropHandling : uint8_t
	{
		actionOpen,
		associateTrajectory
	};

	FileDropHandling whatToDoWithThisFile( const FilePath & );
} // namespace VTX::App::Helper::IO

#endif
