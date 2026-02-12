#include "app/controller/freefly.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/constants.hpp>
#include <util/event_hub.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Controller
{
	Freefly::Freefly()
	{
		auto & settings		= SETTINGS();
		_translationSpeed	= settings.getValue<float>( Settings::Controller::TRANSLATION_SPEED_KEY );
		_accelerationFactor = settings.getValue<float>( Settings::Controller::ACCELERATION_FACTOR_KEY );
		_decelerationFactor = settings.getValue<float>( Settings::Controller::DECELERATION_FACTOR_KEY );
		_rotationSpeed		= settings.getValue<float>( Settings::Controller::ROTATION_SPEED_KEY );
		_invertY			= settings.getValue<bool>( Settings::Controller::INVERT_Y_KEY );
	}

	bool Freefly::update( const float p_deltaTime, Util::Math::Transform & p_transform, Vec3f & p_target )
	{
		using namespace Util;
		auto & input = INPUT();

		//  Rotation.
		Vec2i rotation		= input.rotate();
		Vec3f localRotation = VEC3F_ZERO;
		if ( rotation.x || rotation.y )
		{
			localRotation
				= Vec3f( -_rotationSpeed * rotation.y * ( _invertY ? -1.f : 1.f ), -_rotationSpeed * rotation.x, 0.f );
		}

		Vec2i rotationAlt  = input.rotateAlt();
		float rollRotation = 0.f;
		if ( rotationAlt.x || rotationAlt.y )
		{
			rollRotation = -_rotationSpeed * rotationAlt.x;
		}

		// Translation.
		Vec3f translation = VEC3F_ZERO;
		if ( input.moveFront() )
		{
			translation.z--;
		}
		if ( input.moveBack() )
		{
			translation.z++;
		}
		if ( input.moveLeft() )
		{
			translation.x--;
		}
		if ( input.moveRight() )
		{
			translation.x++;
		}
		if ( input.moveUp() )
		{
			translation.y++;
		}
		if ( input.moveDown() )
		{
			translation.y--;
		}

		if ( translation != VEC3F_ZERO )
		{
			translation *= _translationSpeed;
			translation *= p_deltaTime * 1e-3f;

			if ( input.accelerate() )
			{
				translation *= _accelerationFactor;
			}
			if ( input.decelerate() )
			{
				translation /= _decelerationFactor;
			}
		}

		if ( localRotation != VEC3F_ZERO || rollRotation != 0.f || translation != VEC3F_ZERO )
		{
			if ( localRotation != VEC3F_ZERO )
			{
				p_transform.rotate( localRotation );
			}
			if ( rollRotation != 0.f )
			{
				p_transform.rotateRoll( rollRotation );
			}
			if ( translation != VEC3F_ZERO )
			{
				p_transform.translate( translation );
			}

			HUB().trigger<Events::CameraTransformChange>();
		}

		return true;
	}

} // namespace VTX::App::Controller
