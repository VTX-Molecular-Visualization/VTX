#ifndef VTX_APP_SCENE_VIEWPOINT_HPP
#define VTX_APP_SCENE_VIEWPOINT_HPP

#include <util/types.hpp>

namespace VTX::App::Scene
{
	/**
	 * @brief Saved viewpoint data.
	 */
	struct ViewPoint
	{
		Vec3f target;
	};
} // namespace VTX::App::Scene

#endif
