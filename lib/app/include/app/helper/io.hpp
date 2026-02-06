#ifndef __VTX_APP_HELPER_IO__
#define __VTX_APP_HELPER_IO__

#include <util/filesystem.hpp>

namespace VTX::App::Helper::IO
{
	enum class FileDropHandling
	{
		actionOpen,
		associateTrajectory
	};

	FileDropHandling whatToDoWithThisFile( const FilePath & ) noexcept;
} // namespace VTX::App::Helper::IO

#endif
