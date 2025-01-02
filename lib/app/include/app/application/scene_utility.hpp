#ifndef __VTX_APP_APPLICATION_SCENE_UTILITY__
#define __VTX_APP_APPLICATION_SCENE_UTILITY__

#include "app/core/ecs/base_entity.hpp"
#include "app/core/uid/uid.hpp"
#include <functional>

namespace VTX::App::Application::SceneUtility
{
	// using UpdateCallback = std::function<void( float )>;

	Core::ECS::BaseEntity findItemByUID( const Core::UID::uid & p_uid );
} // namespace VTX::App::Application::SceneUtility
#endif
