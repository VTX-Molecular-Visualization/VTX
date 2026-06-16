#include <app/ecs.hpp>
#include <app/services.hpp>
#include <app/system/visibility.hpp>

namespace VTX::App::System
{
	bool isAnythingVisible() noexcept
	{
		bool somethingVisible = false;
		for ( auto entity : App::REG().view<Visibility>() )
		{
			const Visibility & vis = App::REG().get<Visibility>( entity );
			somethingVisible	   = vis.atoms.any();
			if ( somethingVisible )
			{
				return true;
			}
		}
		return somethingVisible;
	}

} // namespace VTX::App::System
