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
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
			if ( _mouseRightPressed )
			{
				_camera.rotateRoll( VTX_SETTING().getRotationSpeed() * _deltaMousePosition.x );
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}

			// Translation.
			Vec3f translation = VEC3F_ZERO;

			if ( _isKeyPressed( ScanCode::Z ) || _isKeyPressed( ScanCode::Up ) )
			{
				translation.z++;
			}
			if ( _isKeyPressed( ScanCode::S ) || _isKeyPressed( ScanCode::Down ) )
			{
				translation.z--;
			}
			if ( _isKeyPressed( ScanCode::Q ) || _isKeyPressed( ScanCode::Left ) )
			{
				translation.x--;
			}
			if ( _isKeyPressed( ScanCode::D ) || _isKeyPressed( ScanCode::Right ) )
			{
				translation.x++;
			}
			if ( _isKeyPressed( ScanCode::R ) )
			{
				translation.y++;
			}
			if ( _isKeyPressed( ScanCode::F ) )
			{
				translation.y--;
			}

			if ( translation == VEC3F_ZERO )
			{
				return;
			}

			translation *= VTX_SETTING().getTranslationSpeed();
			translation *= p_deltaTime;

			if ( _isKeyPressed( ScanCode::Shift ) )
			{
				translation *= VTX_SETTING().getTranslationSpeedFactor();
			}
			if ( _isKeyPressed( ScanCode::Control ) )
			{
				translation /= VTX_SETTING().getTranslationSpeedFactor();
			}

			_camera.move( translation );
		}

		void Freefly::reset()
		{
			const Vec3f defaultPos = -CAMERA_FRONT_DEFAULT * VTXApp::get().getScene().getAABB().radius()
									 / ( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );

			_camera.setPosition( defaultPos );
			_camera.setRotation( Vec3f( 0.f, 0.f, 0.f ) );
			_isOrienting = false;
		}

		void Freefly::_computeOrientPositions( const Math::AABB & p_aabb )
		{
			_orientStartingPosition	   = _camera.getPosition();
			const float targetDistance = p_aabb.radius() / ( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			_orientTargetPosition	   = p_aabb.centroid() - _camera.getFront() * targetDistance;
			_isOrienting = Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > ORIENT_THRESHOLD;
		}

		void Freefly::_updateOrient( const float & p_deltaTime )
		{
			_camera.setPosition(
				Util::Math::easeInOutInterpolation( _orientStartingPosition, _orientTargetPosition, p_deltaTime ) );
		}

	} // namespace Controller
} // namespace VTX
