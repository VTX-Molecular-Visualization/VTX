#include <app/ecs.hpp>
#include <app/services.hpp>
#include <app/system/visibility.hpp>
//
#include <app/action/visibility.hpp>

namespace VTX::App::Action::Visibility
{
	void HideEverything::execute()
	{
		for ( auto entity : App::REG().view<System::Visibility>() )
		{
			App::REG().patch<System::Visibility>( entity, []( System::Visibility & p_vis ) { p_vis.atoms.clear(); } );
		}
	}
} // namespace VTX::App::Action::Visibility
