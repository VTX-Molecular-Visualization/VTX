#ifndef __VTX_APP_APPLICATION_SCENE_UTILITY__
#define __VTX_APP_APPLICATION_SCENE_UTILITY__

#include <functional>

namespace VTX::App::Application::SceneUtility
{
	using UpdateCallback = std::function<void( float )>;
} // namespace VTX::App::Application::SceneUtility
#endif
