#include "freefly.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "object3d/scene.hpp"

namespace VTX
{
	namespace Controller
	{
		void Freefly::_updateInputs( const float & p_deltaTime )
		{
			// Deselect
			if ( _mouseLeftClick )
			{
				VTX_ACTION(
					new Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
			}

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

			if ( _getExclusiveModifier( ModifierFlag::Shift ) )
			{
				translation *= VTX_SETTING().getTranslationSpeedFactor();
			}
			if ( _getExclusiveModifier( ModifierFlag::Alt ) )
			{
				translation /= VTX_SETTING().getTranslationSpeedFactor();
			}

			_camera.move( translation );
		}

		void Freefly::reset()
		{
			const Vec3f defaultPos = -CAMERA_FRONT_DEFAULT * VTXApp::get().getScene().getAABB().radius()
									 / (float)( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );

			_camera.setPosition( defaultPos );
			_camera.setRotation( Vec3f( 0.f, 0.f, 0.f ) );
			_isOrienting = false;
		}

		void Freefly::_computeOrientPositions( const Math::AABB & p_aabb )
		{
			_orientStartingPosition = _camera.getPosition();
			const float targetDistance
				= p_aabb.radius() / (float)( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			_orientTargetPosition = p_aabb.centroid() - _camera.getFront() * targetDistance;
			_isOrienting = Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;
		}

		void Freefly::_updateOrient( const float & p_deltaTime )
		{
			_camera.setPosition(
				Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime ) );
		}

	} // namespace Controller
} // namespace VTX
