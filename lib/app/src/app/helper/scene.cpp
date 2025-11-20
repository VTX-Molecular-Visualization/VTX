#include "app/helper/scene.hpp"
#include "app/system/uid.hpp"

namespace VTX::App::Helper::Scene
{

	std::optional<ECS::Entity> findSystemByRootUID( const RootUID p_uid )
	{
		auto view = REG().view<System::UID>();

		for ( auto it = view.begin(); it != view.end(); ++it )
		{
			const auto & uidComponent = view.get<System::UID>( *it );
			if ( uidComponent.system == p_uid )
			{
				return *it;
			}
		}

		return std::nullopt;
	}

} // namespace VTX::App::Helper::Scene
