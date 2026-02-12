#include "app/controller/animation.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Controller
{
	Animation::Animation(
		const AnimationData &	   p_dataStart,
		const AnimationData &	   p_dataEnd,
		const float				   p_duration,
		const InterpPositionFunc & p_interpPosition,
		const InterpRotationFunc & p_interRotation
	) :
		_animationDataStart( p_dataStart ), _animationDataEnd( p_dataEnd ), _duration( p_duration ),
		_interpPosition( p_interpPosition ), _interpRotation( p_interRotation ), _time( 0.f )
	{
	}

	/**
	 * @brief Called each frame.
	 */
	bool Animation::update( const float p_delta, Util::Math::Transform & p_transform, Vec3f & )
	{
		using namespace Util;

		_time += p_delta;

		// Lerp.
		const float t = Math::clamp( _time / _duration, 0.f, 1.f );
		p_transform.setPosition( _interpPosition( _animationDataStart.position, _animationDataEnd.position, t ) );
		p_transform.setRotation( _interpRotation( _animationDataStart.rotation, _animationDataEnd.rotation, t ) );
		HUB().trigger<Events::CameraTransformChange>();

		return t < 1.f;
	}
} // namespace VTX::App::Controller
