#include "app/pass/controller/trackball.hpp"
#include "app/input/input_manager.hpp"
#include "app/input/key_mapping.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace
{
	using namespace VTX::App;

	enum Keys
	{
		MOVE_FRONT,
		MOVE_BACK,
		ROTATE_LEFT,
		ROTATE_RIGHT,
		ROTATE_UP,
		ROTATE_DOWN,
		ROLL_LEFT,
		ROLL_RIGHT,
	};

	auto _MAPPING = Input::KeyMapping(
		{
			{ Keys::MOVE_FRONT, { Input::Key::Key_Up, Input::InputManager::getKeyFromQwerty( Input::Key::Key_W ) } },
			{ Keys::MOVE_BACK, { Input::Key::Key_Down, Input::InputManager::getKeyFromQwerty( Input::Key::Key_S ) } },
			{ Keys::ROTATE_LEFT, { Input::Key::Key_Left, Input::InputManager::getKeyFromQwerty( Input::Key::Key_A ) } },
			{ Keys::ROTATE_RIGHT,
			  { Input::Key::Key_Right, Input::InputManager::getKeyFromQwerty( Input::Key::Key_D ) } },
			{ Keys::ROTATE_UP, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_R ) } },
			{ Keys::ROTATE_DOWN, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_F ) } },
			{ Keys::ROLL_LEFT, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_Q ) } },
			{ Keys::ROLL_RIGHT, { Input::InputManager::getKeyFromQwerty( Input::Key::Key_E ) } },
		}
	);

	constexpr float _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;
} // namespace

namespace VTX::App::Pass::Controller
{
	Trackball::Trackball( const ECS::Entity & p_ent, const Vec3f & p_target ) :
		_cameraEntity( p_ent ), _target( p_target )
	{
		// TODO: use setting object?
		auto & settings	   = SETTINGS();
		translationSpeed   = settings.getValue<float>( Settings::Controller::TRANSLATION_SPEED_KEY );
		accelerationFactor = settings.getValue<float>( Settings::Controller::ACCELERATION_FACTOR_KEY );
		decelerationFactor = settings.getValue<float>( Settings::Controller::DECELERATION_FACTOR_KEY );
		rotationSpeed	   = settings.getValue<float>( Settings::Controller::ROTATION_SPEED_KEY );
		invertY			   = settings.getValue<bool>( Settings::Controller::INVERT_Y_KEY );
		elasticityActive   = settings.getValue<bool>( Settings::Controller::ELASTICITY_ACTIVE_KEY );
		elasticityFactor   = settings.getValue<float>( Settings::Controller::ELASTICITY_FACTOR_KEY );
	}

	void Trackball::update( const float p_delta, const float p_elapsed )
	{
		using namespace Util;
		auto & input = INPUT();

		auto & transform = REG().get<Math::Transform>( _cameraEntity );

		float deltaTime = p_delta * 1e-3f;

		// Wheel.
		float deltaDistance = 0.f;
		if ( input.getDeltaMouseWheel() != 0 )
		{
			deltaDistance
				= input.consumeDeltaMouseWheel() * 0.00001f * Math::distance( transform.getPosition(), _target );
		}

		// Mouse left.
		Vec3f deltaVelocity		 = VEC3F_ZERO;
		Vec2i deltaVelocityInput = input.consumeDeltaMousePosition();
		if ( input.isMouseLeftPressed() )
		{
			deltaVelocity.x = -deltaVelocityInput.x * 15.f;
			deltaVelocity.y = -deltaVelocityInput.y * 15.f;
		}
		// Mouse right.
		else if ( input.isMouseRightPressed() )
		{
			deltaVelocity.z = deltaVelocityInput.x * 15.f;
		}
		// Pan target with wheel button.
		else if ( input.isMouseMiddlePressed() )
		{
			float deltaX = -deltaVelocityInput.x * 0.1f;
			float deltaY = deltaVelocityInput.y * 0.1f;
			_target += transform.getRotation() * ( VEC3F_X * deltaX + VEC3F_Y * deltaY );
			_needUpdate = true;
		}

		// Keyboard.
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_FRONT ] ) )
		{
			deltaDistance = 1.5f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::MOVE_BACK ] ) )
		{
			deltaDistance = -1.5f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROTATE_RIGHT ] ) )
		{
			deltaVelocity.x = 1e4f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROTATE_LEFT ] ) )
		{
			deltaVelocity.x = -1e4f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROTATE_UP ] ) )
		{
			deltaVelocity.y = 1e4f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROTATE_DOWN ] ) )
		{
			deltaVelocity.y = -1e4f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROLL_RIGHT ] ) )
		{
			deltaVelocity.z = 1e4f * deltaTime;
		}
		if ( input.isAnyKeyPressed( _MAPPING[ Keys::ROLL_LEFT ] ) )
		{
			deltaVelocity.z = -1e4f * deltaTime;
		}

		// Set values from settings.
		if ( deltaDistance != 0.f )
		{
			deltaDistance *= translationSpeed;

			if ( input.isModifierExclusive( Input::Modifier::Shift ) )
			{
				deltaDistance *= accelerationFactor;
			}
			if ( input.isModifierExclusive( Input::Modifier::Alt ) )
			{
				deltaDistance /= decelerationFactor;
			}

			_needUpdate = true;
		}

		if ( deltaVelocity != VEC3F_ZERO )
		{
			if ( input.isModifierExclusive( Input::Modifier::Shift ) )
			{
				deltaVelocity *= accelerationFactor;
			}
			if ( input.isModifierExclusive( Input::Modifier::Alt ) )
			{
				deltaVelocity /= decelerationFactor;
			}

			_velocity.x += rotationSpeed * deltaVelocity.x;
			_velocity.y += rotationSpeed * deltaVelocity.y * ( invertY ? -1.f : 1.f );
			_velocity.z += rotationSpeed * deltaVelocity.z;
		}

		_needUpdate |= _velocity != VEC3F_ZERO;

		// Update if needed.
		if ( _needUpdate )
		{
			float distance = Math::distance( transform.getPosition(), _target );
			distance	   = Math::clamp( distance - deltaDistance, 0.1f, 10000.f );

			const Quatf rotation
				= Quatf( Vec3f( _velocity.y, _velocity.x, _velocity.z ) * ( elasticityActive ? deltaTime : 0.2f ) );

			REG().patch<Math::Transform>(
				_cameraEntity,
				[ &rotation, &distance, this ]( Math::Transform & p_transform )
				{ p_transform.rotateAround( rotation, _target, distance ); }
			);

			_needUpdate = false;
		}

		// Handle elasticity.
		if ( elasticityActive )
		{
			_updateElasticity( deltaTime );
		}
		else
		{
			_velocity = VEC3F_ZERO;
		}
	}

	void Trackball::_updateElasticity( const float & p_deltaTime )
	{
		if ( _velocity != VEC3F_ZERO )
		{
			_velocity = Util::Math::lerp( _velocity, VEC3F_ZERO, p_deltaTime * elasticityFactor );

			Vec3f::bool_type res
				= Util::Math::lessThan( Util::Math::abs( _velocity ), Vec3f( _CONTROLLER_ELASTICITY_THRESHOLD ) );

			if ( not INPUT().isMouseLeftPressed() && res.x && res.y && res.z )
			{
				_velocity = VEC3F_ZERO;
			}
		}
	}
} // namespace VTX::App::Pass::Controller
