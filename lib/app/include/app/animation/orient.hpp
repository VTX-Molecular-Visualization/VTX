#ifndef __VTX_APP_ANIMATION_ORIENT__
#define __VTX_APP_ANIMATION_ORIENT__

#include "app/component/render/camera.hpp"
#include "app/core/animation/concepts.hpp"
#include "app/core/animation/travel_manager.hpp"
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

		inline void subscribe( Core::Animation::ProgressCallback p_ ) { _animation.onProgress += std::move( p_ ); }
		inline void subscribe( Core::Animation::EndCallback p_ ) { _animation.onEnd += std::move( p_ ); }

	  private:
		Core::Animation::TravelManager _animation;
	};
} // namespace VTX::App::Animation
#endif
