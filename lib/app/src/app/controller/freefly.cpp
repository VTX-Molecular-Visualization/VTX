#include "app/controller/freefly.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/services.hpp"
#include <util/constants.hpp>
#include <util/event_hub.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Controller
{

	bool Freefly::update(
		const float					p_deltaTime,
		const Setting::Controller & p_settings,
		Util::Math::Transform &		p_transform,
		Vec3f &						p_target
	)
	{
		using namespace Util;
		auto & input = INPUT();

		const float rotationSpeed = p_settings.rotationSpeed * Setting::ROTATION_SPEED_MULTIPLIER;

		//  Rotation.
		Vec2i rotation		= input.rotate();
		Vec3f localRotation = VEC3F_ZERO;
		if ( rotation.x || rotation.y )
		{
			localRotation = Vec3f(
				-rotationSpeed * rotation.y * ( p_settings.invertY ? -1.f : 1.f ), -rotationSpeed * rotation.x, 0.f
			);
		}

		Vec2i rotationAlt  = input.rotateAlt();
		float rollRotation = 0.f;
		if ( rotationAlt.x || rotationAlt.y )
		{
			rollRotation = -rotationSpeed * rotationAlt.x;
		}

		// Translation.
		const Vec3i translationAxis = input.translationAxis();
		Vec3f		translation		= VEC3F_ZERO;
		translation.x				= float( translationAxis.x );
		translation.y				= float( translationAxis.y );
		translation.z				= float( translationAxis.z );

		if ( translation != VEC3F_ZERO )
		{
			translation *= p_settings.translationSpeed;
			translation *= p_deltaTime * _MS_TO_S;

			if ( input.accelerate() )
			{
				translation *= p_settings.accelerationFactor;
			}
			if ( input.decelerate() )
			{
				translation /= p_settings.decelerationFactor;
			}
		}

		// Apply.
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

			// Keep target forward.
			const float distance = Math::length( p_target - p_transform.getPosition() );
			const Vec3f front	 = p_transform.getFront();
			p_target			 = p_transform.getPosition() + front * distance;

			HUB().trigger<Events::CameraTransformChange>();
		}

		return true;
	}

} // namespace VTX::App::Controller
