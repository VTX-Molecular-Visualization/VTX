#include "app/controller/trackball.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/event_hub.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace
{
	constexpr float _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;
} // namespace

namespace VTX::App::Controller
{
	Trackball::Trackball( const Vec3f & p_target ) : _target( p_target )
	{
		// TODO: use setting object?
		auto & settings		= SETTINGS();
		_translationSpeed	= settings.getValue<float>( Settings::Controller::TRANSLATION_SPEED_KEY );
		_accelerationFactor = settings.getValue<float>( Settings::Controller::ACCELERATION_FACTOR_KEY );
		_decelerationFactor = settings.getValue<float>( Settings::Controller::DECELERATION_FACTOR_KEY );
		_rotationSpeed		= settings.getValue<float>( Settings::Controller::ROTATION_SPEED_KEY );
		_invertY			= settings.getValue<bool>( Settings::Controller::INVERT_Y_KEY );
		_elasticityActive	= settings.getValue<bool>( Settings::Controller::ELASTICITY_ACTIVE_KEY );
		_elasticityFactor	= settings.getValue<float>( Settings::Controller::ELASTICITY_FACTOR_KEY );
	}

	void Trackball::update( const float p_delta, Util::Math::Transform & p_transform )
	{
		using namespace Util;
		auto & input = INPUT();

		float deltaTime = p_delta * 1e-3f;

		// Wheel.
		float deltaDistance = 0.f;
		if ( input.zoom() != 0 )
		{
			deltaDistance = input.zoom() * 0.00001f * Math::distance( p_transform.getPosition(), _target );
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
		float deltaX   = -deltaPan.x * 0.1f;
		float deltaY   = deltaPan.y * 0.1f;
		_target += p_transform.getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
		_needUpdate = true;

		// Keyboard.
		if ( input.moveFront() )
		{
			deltaDistance = 1.5f * deltaTime;
		}
		if ( input.moveBack() )
		{
			deltaDistance = -1.5f * deltaTime;
		}
		if ( input.moveRight() )
		{
			deltaVelocity.x = 1e4f * deltaTime;
		}
		if ( input.moveLeft() )
		{
			deltaVelocity.x = -1e4f * deltaTime;
		}
		if ( input.moveUp() )
		{
			deltaVelocity.y = 1e4f * deltaTime;
		}
		if ( input.moveDown() )
		{
			deltaVelocity.y = -1e4f * deltaTime;
		}
		if ( input.rotateRight() )
		{
			deltaVelocity.z = 1e4f * deltaTime;
		}
		if ( input.rotateLeft() )
		{
			deltaVelocity.z = -1e4f * deltaTime;
		}

		// Set values from settings.
		if ( deltaDistance != 0.f )
		{
			deltaDistance *= _translationSpeed;

			if ( input.accelerate() )
			{
				deltaDistance *= _accelerationFactor;
			}
			if ( input.decelerate() )
			{
				deltaDistance /= _decelerationFactor;
			}

			_needUpdate = true;
		}

		if ( deltaVelocity != VEC3F_ZERO )
		{
			if ( input.accelerate() )
			{
				deltaVelocity *= _accelerationFactor;
			}
			if ( input.decelerate() )
			{
				deltaVelocity /= _decelerationFactor;
			}

			_velocity.x += _rotationSpeed * deltaVelocity.x;
			_velocity.y += _rotationSpeed * deltaVelocity.y * ( _invertY ? -1.f : 1.f );
			_velocity.z += _rotationSpeed * deltaVelocity.z;
		}

		_needUpdate |= _velocity != VEC3F_ZERO;

		// Update if needed.
		if ( _needUpdate )
		{
			float distance = Math::distance( p_transform.getPosition(), _target );
			distance	   = Math::clamp( distance - deltaDistance, 0.1f, 10000.f );

			const Quatf rotation
				= Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z ) * ( _elasticityActive ? deltaTime : 0.2f ) );

			p_transform.rotateAround( rotation, _target, distance );
			HUB().trigger<Events::CameraTransformChange>();

			_needUpdate = false;
		}

		// Handle elasticity.
		if ( _elasticityActive )
		{
			_updateElasticity( deltaTime );
		}
		else
		{
			_velocity = VEC3F_ZERO;
		}

		input.consume();
	}

	void Trackball::_updateElasticity( const float & p_deltaTime )
	{
		if ( _velocity != VEC3F_ZERO )
		{
			_velocity = Util::Math::lerp( _velocity, VEC3F_ZERO, p_deltaTime * _elasticityFactor );

			Vec3f::bool_type res
				= Util::Math::lessThan( Util::Math::abs( _velocity ), Vec3f( _CONTROLLER_ELASTICITY_THRESHOLD ) );

			if ( Util::Math::all( res ) )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}
} // namespace VTX::App::Controller
