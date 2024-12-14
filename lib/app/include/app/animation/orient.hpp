#ifndef __VTX_APP_ANIMATION_ORIENT__
#define __VTX_APP_ANIMATION_ORIENT__

#include "app/component/render/camera.hpp"
#include "app/core/animation/base_animation.hpp"
#include <util/hashing.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Animation
{

	class Orient : public Core::Animation::BaseAnimation
	{
	  public:
		inline static const float ORIENT_ZOOM_FACTOR = 0.666f;

		Orient( const App::Component::Render::Camera &, const Util::Math::AABB & );

		static Vec3f computeCameraOrientPosition(
			const Vec3f				 p_forward,
			const float				 p_fov,
			const Util::Math::AABB & p_target,
			const float				 p_orientZoomFactor = ORIENT_ZOOM_FACTOR
		);

		void update( const float, const float );
	};
} // namespace VTX::App::Animation
#endif
