#include "app/controller/trackball.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace
{
	constexpr float WHEEL_DISTANCE_FACTOR	  = 1e-5f;
	constexpr float MOUSE_ROTATION_FACTOR	  = 15.f;
	constexpr float PAN_FACTOR				  = 0.1f;
	constexpr float KEYBOARD_DISTANCE_FACTOR = 1.5f;
	constexpr float KEYBOARD_ROTATION_FACTOR = 1e4f;
	constexpr float DISTANCE_MIN			  = 0.1f;
	constexpr float DISTANCE_MAX			  = 10000.f;
	constexpr float NON_ELASTIC_DELTA_TIME	  = 0.2f;
	constexpr float ELASTICITY_THRESHOLD	  = 1e-4f;
} // namespace

namespace VTX::App::Controller
{

	bool Trackball::update( const float					p_delta,
							const Setting::Controller & p_settings,
							Util::Math::Transform &		p_transform,
							Vec3f &						p_target )
	{
		using namespace Util;
		auto & input = INPUT();

		const float deltaTime	  = p_delta * MS_TO_S;
		const float rotationSpeed = p_settings.rotationSpeed * Setting::ROTATION_SPEED_MULTIPLIER;

		// Wheel.
		float deltaDistance = 0.f;
		if ( input.zoom() != 0 )
		{
			deltaDistance
				= input.zoom() * WHEEL_DISTANCE_FACTOR * Math::distance( p_transform.getPosition(), p_target );
		}

		// Mouse left.
		Vec3f deltaVelocity = VEC3F_ZERO;

		const Vec2i deltaRotate = input.rotate();
		deltaVelocity.x			= -deltaRotate.x * MOUSE_ROTATION_FACTOR;
		deltaVelocity.y			= -deltaRotate.y * MOUSE_ROTATION_FACTOR;

		// Mouse right.
		const Vec2i deltaRotateAlt = input.rotateAlt();
		deltaVelocity.z			   = deltaRotateAlt.x * MOUSE_ROTATION_FACTOR;

		// Pan target with wheel button.
		const Vec2i deltaPan = input.pan();
		if ( deltaPan != VEC2I_ZERO )
		{
			const float deltaX = -deltaPan.x * PAN_FACTOR;
			const float deltaY = deltaPan.y * PAN_FACTOR;
			p_target += p_transform.getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
			_needUpdate = true;
		}

		// Keyboard.
		const Vec3i translationAxis = input.translationAxis();
		const int	rotationAxis	= input.rotationAxis();
		if ( translationAxis.z != 0 )
		{
			deltaDistance = -KEYBOARD_DISTANCE_FACTOR * deltaTime * static_cast<float>( translationAxis.z );
		}
		if ( translationAxis.x != 0 )
		{
			deltaVelocity.x = -KEYBOARD_ROTATION_FACTOR * deltaTime * static_cast<float>( translationAxis.x );
		}
		if ( translationAxis.y != 0 )
		{
			deltaVelocity.y = KEYBOARD_ROTATION_FACTOR * deltaTime * static_cast<float>( translationAxis.y );
		}
		if ( rotationAxis != 0 )
		{
			deltaVelocity.z = KEYBOARD_ROTATION_FACTOR * deltaTime * static_cast<float>( rotationAxis );
		}

		// Set values from settings.
		if ( deltaDistance != 0.f )
		{
			deltaDistance *= p_settings.translationSpeed;

			if ( input.accelerate() )
			{
				deltaDistance *= p_settings.accelerationFactor;
			}
			if ( input.decelerate() )
			{
				deltaDistance /= p_settings.decelerationFactor;
			}

			_needUpdate = true;
		}

		if ( deltaVelocity != VEC3F_ZERO )
		{
			if ( input.accelerate() )
			{
				deltaVelocity *= p_settings.accelerationFactor;
			}
			if ( input.decelerate() )
			{
				deltaVelocity /= p_settings.decelerationFactor;
			}

			_velocity.x += rotationSpeed * deltaVelocity.x;
			_velocity.y += rotationSpeed * deltaVelocity.y * ( p_settings.invertY ? -1.f : 1.f );
			_velocity.z += rotationSpeed * deltaVelocity.z;
		}

		_needUpdate |= _velocity != VEC3F_ZERO;

		// Update if needed.
		if ( _needUpdate )
		{
			float distance = Math::distance( p_transform.getPosition(), p_target );
			distance	   = Math::clamp( distance - deltaDistance, DISTANCE_MIN, DISTANCE_MAX );

			const Quatf rotation = Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z )
										  * ( p_settings.elasticityActive ? deltaTime : NON_ELASTIC_DELTA_TIME ) );

			p_transform.rotateAround( rotation, p_target, distance );
			HUB().trigger<Events::CameraTransformChange>();

			_needUpdate = false;
		}

		// Handle elasticity.
		if ( p_settings.elasticityActive )
		{
			_updateVelocity( deltaTime, p_settings.elasticityFactor );
		}
		else
		{
			_velocity = VEC3F_ZERO;
		}

		return true;
	}

	void Trackball::_updateVelocity( const float p_deltaTime, const float p_elasticityFactor )
	{
		if ( _velocity != VEC3F_ZERO )
		{
			_velocity = Util::Math::lerp( _velocity, VEC3F_ZERO, p_deltaTime * p_elasticityFactor );

			const Vec3f::bool_type res
				= Util::Math::lessThan( Util::Math::abs( _velocity ), Vec3f( ELASTICITY_THRESHOLD ) );

			if ( Util::Math::all( res ) )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}
} // namespace VTX::App::Controller
