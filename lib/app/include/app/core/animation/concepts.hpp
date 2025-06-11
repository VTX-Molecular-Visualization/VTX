#ifndef __VTX_APP_CORE_ANIMATION_CONCEPTS__
#define __VTX_APP_CORE_ANIMATION_CONCEPTS__

#include "app/core/animation/travel_manager.hpp"
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/math.hpp>

namespace VTX::App::Core::Animation
{

	using ProgressCallback = std::function<void( const Vec3f &, const Quatf & )>;
	using EndCallback	   = std::function<void( const Vec3f & )>;

	template<typename A>
	concept ConceptAnimation = std::is_base_of<TravelManager, A>::value
							   && requires( A p_animation, const float p_deltaTime, const float p_elapsedTime ) {
									  // { p_animation.play() } -> std::same_as<void>;
									  { p_animation.update( p_deltaTime, p_elapsedTime ) } -> std::same_as<void>;
									  //
									  // { p_animation.enter() } -> std::same_as<void>;
									  // { p_animation.exit() } -> std::same_as<void>;
								  };

} // namespace VTX::App::Core::Animation

#endif
