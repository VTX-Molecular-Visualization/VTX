#ifndef __VTX_APP_ECS_SCENE_UTILITY__
#define __VTX_APP_ECS_SCENE_UTILITY__

#include <functional>

namespace VTX::App::ECS::SceneUtility
{
	using UpdateCallback = std::function<void( float )>;
} // namespace VTX::App::ECS::Sceneutility
#endif
