#include "app/helper/io.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/metadata.hpp"
#include "app/system/trajectory.hpp"
#include <io/reader.hpp>

namespace VTX::App::Helper::IO
{
	FileDropHandling whatToDoWithThisFile( const FilePath & p_filePath ) noexcept
	{
		// std::string ext = p_filePath.extension().string();
		if ( VTX::IO::isTrajectoryFileFormat( p_filePath ) )
		{
			auto view = REG().view<System::TrajectorySingleFrame, System::Metadata>();
			if ( std::distance( view.begin(), view.end() ) > 0 )
				return FileDropHandling::associateTrajectory;
		}
		return FileDropHandling::actionOpen;
	}

} // namespace VTX::App::Helper::IO
