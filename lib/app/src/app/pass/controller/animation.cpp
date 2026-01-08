#include "app/pass/controller/animation.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <util/math/transform.hpp>

namespace VTX::App::Pass::Controller
{
	Animation::Animation(
		const ECS::Entity &		   p_ent,
		const AnimationData &	   p_dataStart,
		const AnimationData &	   p_dataEnd,
		const float				   p_duration,
		const InterpPositionFunc & p_interpPosition,
		const InterpRotationFunc & p_interRotation
	) :
		_cameraEntity( p_ent ), _animationDataStart( p_dataStart ), _animationDataEnd( p_dataEnd ),
		_duration( p_duration ), _interpPosition( p_interpPosition ), _interpRotation( p_interRotation )
	{
		using namespace Util;

		_time = 0.f;

		// Set initial position and rotation.
		REG().patch<Math::Transform>(
			_cameraEntity,
			[ & ]( Math::Transform & p_transform )
			{
				p_transform.setPosition( _animationDataStart.position );
				p_transform.setRotation( _animationDataStart.rotation );
			}
		);
	}

	/**
	 * @brief Called each frame.
	 */
	void Animation::update( const float p_delta, const float p_elapsed )
	{
		using namespace Util;

		REG().patch<Math::Transform>(
			_cameraEntity,
			[ this, p_delta ]( Math::Transform & p_transform )
			{
				_time += p_delta;

				// Lerp.
				const float t = Math::clamp( _time / _duration, 0.f, 1.f );
				p_transform.setPosition(
					_interpPosition( _animationDataStart.position, _animationDataEnd.position, t )
				);
				p_transform.setRotation(
					_interpRotation( _animationDataStart.rotation, _animationDataEnd.rotation, t )
				);

				// Flag as deleted and trigger end event.
				if ( t >= 1.f )
				{
					deleted = true;
					HUB().trigger<Events::CameraAnimationEnd>();
				}
			}
		);
	}
} // namespace VTX::App::Pass::Controller
