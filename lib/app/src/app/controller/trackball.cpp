#include "app/controller/trackball.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include <util/event_hub.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace
{
	constexpr float _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;
} // namespace

namespace VTX::App::Controller
{

	bool Trackball::update(
		const float					p_delta,
		const Setting::Controller & p_settings,
		Util::Math::Transform &		p_transform,
		Vec3f &						p_target
	)
	{
		using namespace Util;
		auto & input = INPUT();

		float deltaTime = p_delta * 1e-3f;

		// Wheel.
		float deltaDistance = 0.f;
		if ( input.zoom() != 0 )
		{
			deltaDistance = input.zoom() * 0.00001f * Math::distance( p_transform.getPosition(), p_target );
		}

		// Mouse left.
		Vec3f deltaVelocity = VEC3F_ZERO;

		Vec2i deltaRotate = input.rotate();
		deltaVelocity.x	  = -deltaRotate.x * 15.f;
		deltaVelocity.y	  = -deltaRotate.y * 15.f;

		// Mouse right.
		Vec2i deltaRotateAlt = input.rotateAlt();
		deltaVelocity.z		 = deltaRotateAlt.x * 15.f;

		// Pan target with wheel button.
		Vec2i deltaPan = input.pan();
		if ( deltaPan != VEC2I_ZERO )
		{
			float deltaX = -deltaPan.x * 0.1f;
			float deltaY = deltaPan.y * 0.1f;
			p_target += p_transform.getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
			_needUpdate = true;
		}

		// Keyboard.
		const Vec3i translationAxis = input.translationAxis();
		const int	rotationAxis	= input.rotationAxis();
		if ( translationAxis.z != 0 )
		{
			deltaDistance = -1.5f * deltaTime * float( translationAxis.z );
		}
		if ( translationAxis.x != 0 )
		{
			deltaVelocity.x = -1e4f * deltaTime * float( translationAxis.x );
		}
		if ( translationAxis.y != 0 )
		{
			deltaVelocity.y = 1e4f * deltaTime * float( translationAxis.y );
		}
		if ( rotationAxis != 0 )
		{
			deltaVelocity.z = 1e4f * deltaTime * float( rotationAxis );
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

			_velocity.x += p_settings.rotationSpeed * deltaVelocity.x;
			_velocity.y += p_settings.rotationSpeed * deltaVelocity.y * ( p_settings.invertY ? -1.f : 1.f );
			_velocity.z += p_settings.rotationSpeed * deltaVelocity.z;
		}

		_needUpdate |= _velocity != VEC3F_ZERO;

		// Update if needed.
		if ( _needUpdate )
		{
			float distance = Math::distance( p_transform.getPosition(), p_target );
			distance	   = Math::clamp( distance - deltaDistance, 0.1f, 10000.f );

			const Quatf rotation = Quatf(
				Vec3f( _velocity.y, _velocity.x, _velocity.z ) * ( p_settings.elasticityActive ? deltaTime : 0.2f )
			);

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

			Vec3f::bool_type res
				= Util::Math::lessThan( Util::Math::abs( _velocity ), Vec3f( _CONTROLLER_ELASTICITY_THRESHOLD ) );

			if ( Util::Math::all( res ) )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}
} // namespace VTX::App::Controller
