#include "ui/qt/controller/freefly.hpp"
#include "ui/internal/all_settings.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/style.hpp"
#include <app/application/settings.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::QT::Controller
{
	Freefly::Freefly() : BaseCameraController() {}

	std::unique_ptr<BaseController> Freefly::clone() const { return std::make_unique<Freefly>( *this ); }

	void Freefly::_updateInputs( const float & p_deltaTime )
	{
		if ( !_widgetTarget->hasFocus() )
			return;

		// Rotation.
		if ( INPUT_MANAGER().isMouseLeftPressed() )
		{
			getCamera().rotate( Vec3f(
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().y * ( invertY ? -1.f : 1.f ),
				-rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x,
				0.f
			) );
		}
		if ( INPUT_MANAGER().isMouseRightPressed() )
		{
			getCamera().rotateRoll( rotationSpeed * INPUT_MANAGER().getDeltaMousePosition().x );
		}

		// Translation.
		Vec3f translation = VEC3F_ZERO;

		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_FRONT ) ] ) )
		{
			translation.z++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_BACK ) ] ) )
		{
			translation.z--;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_LEFT ) ] ) )
		{
			translation.x--;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_RIGHT ) ] ) )
		{
			translation.x++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_UP ) ] ) )
		{
			translation.y++;
		}
		if ( INPUT_MANAGER().isAnyKeyPressed( _mapping[ int( Keys::MOVE_DOWN ) ] ) )
		{
			translation.y--;
		}

		if ( translation == VEC3F_ZERO )
		{
			return;
		}

		translation *= translationSpeed;
		translation *= p_deltaTime;

		if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierFlag::Shift ) )
		{
			translation *= accelerationFactor;
		}
		if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierFlag::Alt ) )
		{
			translation /= decelerationFactor;
		}

		getCamera().move( translation );
	}

} // namespace VTX::UI::QT::Controller
