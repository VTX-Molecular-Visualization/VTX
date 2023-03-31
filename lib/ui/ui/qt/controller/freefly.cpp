#include "freefly.hpp"
#include "qt/style.hpp"
#include <old/action/action_manager.hpp>
#include <old/object3d/scene.hpp>
#include <old/setting.hpp>

namespace VTX::UI::QT::Controller
{
	void Freefly::_updateInputs( const float & p_deltaTime )
	{
		// Rotation.
		if ( _mouseLeftPressed )
		{
			_camera.rotate( Vec3f( -VTX_SETTING().getRotationSpeed() * _deltaMousePosition.y
									   * ( VTX_SETTING().getYAxisInverted() ? -1.f : 1.f ),
								   -VTX_SETTING().getRotationSpeed() * _deltaMousePosition.x,
								   0.f ) );
		}
		if ( _mouseRightPressed )
		{
			_camera.rotateRoll( VTX_SETTING().getRotationSpeed() * _deltaMousePosition.x );
		}

		_deltaMousePosition.x = 0;
		_deltaMousePosition.y = 0;

		// Translation.
		Vec3f translation = VEC3F_ZERO;

		if ( _isKeyPressed( getKeyboardLayout() == KeyboardLayout::QWERTY ? Qt::Key::Key_W : Qt::Key::Key_Z )
			 || _isKeyPressed( Qt::Key::Key_Up ) )
		{
			translation.z++;
		}
		if ( _isKeyPressed( Qt::Key::Key_S ) || _isKeyPressed( Qt::Key::Key_Down ) )
		{
			translation.z--;
		}
		if ( _isKeyPressed( getKeyboardLayout() == KeyboardLayout::QWERTY ? Qt::Key::Key_A : Qt::Key::Key_Q )
			 || _isKeyPressed( Qt::Key::Key_Left ) )
		{
			translation.x--;
		}
		if ( _isKeyPressed( Qt::Key::Key_D ) || _isKeyPressed( Qt::Key::Key_Right ) )
		{
			translation.x++;
		}
		if ( _isKeyPressed( Qt::Key::Key_R ) )
		{
			translation.y++;
		}
		if ( _isKeyPressed( Qt::Key::Key_F ) )
		{
			translation.y--;
		}

		if ( translation == VEC3F_ZERO )
		{
			return;
		}

		translation *= VTX_SETTING().getTranslationSpeed();
		translation *= p_deltaTime;

		if ( _isModifierExclusive( ModifierFlag::Shift ) )
		{
			translation *= VTX_SETTING().getAccelerationSpeedFactor();
		}
		if ( _isModifierExclusive( ModifierFlag::Alt ) )
		{
			translation /= VTX_SETTING().getDecelerationSpeedFactor();
		}

		_camera.move( translation );
	}

	void Freefly::_computeOrientPositions( const Object3D::Helper::AABB & p_aabb )
	{
		_orientStartingPosition = _camera.getPosition();
		const float targetDistance
			= p_aabb.radius()
			  / (float)( tan( VTX::Util::Math::radians( _camera.getFov() ) * Style::ORIENT_ZOOM_FACTOR ) );
		_orientTargetPosition = p_aabb.centroid() - _camera.getFront() * targetDistance;

		_orientStartingRotation = _camera.getRotation();
		_orientTargetRotation	= _orientStartingRotation;

		_isOrienting = VTX::Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;
	}
	void Freefly::_computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation )
	{
		_orientStartingPosition = _camera.getPosition();
		_orientTargetPosition	= p_position;

		_orientStartingRotation = _camera.getRotation();
		_orientTargetRotation	= p_orientation;

		_isOrienting = VTX::Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;
	}

	void Freefly::_updateOrient( const float & p_deltaTime )
	{
		_camera.setPosition(
			VTX::Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime ) );

		_camera.setRotation(
			VTX::Util::Math::easeInOutInterpolation( _orientStartingRotation, _orientTargetRotation, p_deltaTime ) );
	}

} // namespace VTX::UI::QT::Controller
