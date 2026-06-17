#ifndef __VTX_APP_SCENE_VIEWPOINT__
#define __VTX_APP_SCENE_VIEWPOINT__

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
