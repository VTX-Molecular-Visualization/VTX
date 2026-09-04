#include "app/helper/io.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/trajectory/player.hpp"
#include <core/struct/trajectory.hpp>
#include <io/metadata.hpp>
#include <io/reader.hpp>

namespace VTX::App::Helper::IO
{
	FileDropHandling whatToDoWithThisFile( const FilePath & p_filePath )
	{
		// std::string ext = p_filePath.extension().string();
		if ( VTX::IO::isTrajectoryFileFormat( p_filePath ) )
		{
			for ( const Entity entity : REG().view<Core::Struct::Trajectory, VTX::IO::Metadata>() )
			{
				if ( not REG().all_of<App::Trajectory::Player>( entity ) )
				{
					return FileDropHandling::associateTrajectory;
				}
			}
		}
		return FileDropHandling::actionOpen;
	}

} // namespace VTX::App::Helper::IO
