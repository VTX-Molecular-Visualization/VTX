#ifndef __VTX_APP_ANIMATION_ORIENT__
#define __VTX_APP_ANIMATION_ORIENT__

#include "app/component/render/camera.hpp"
#include "app/core/animation/base_animation.hpp"
#include <util/hashing.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Animation
{

	class Orient
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

		void  update( const float p_delta, const float p_elasped );
		void  play();
		void  stop();
		float getRatio() const;

		inline Util::Callback<const Vec3f &, const Quatf &> & onProgress() { return _animation.onProgress; }
		inline Util::Callback<const Vec3f &> &				  onEnd() { return _animation.onEnd; }

	  private:
		Core::Animation::BaseAnimation _animation;
	};
} // namespace VTX::App::Animation
#endif
