#ifndef __VTX_APP_HELPER_SCENE__
#define __VTX_APP_HELPER_SCENE__

#include "app/ecs.hpp"
#include <optional>
#include <util/types.hpp>

namespace VTX::App::Helper::Scene
{

	std::optional<ECS::Entity> findSystemByRootUID( const RootUID );

}

#endif
