#include "freefly.hpp"

namespace VTX
{
	namespace Controller
	{
		void Freefly::_updateInputs( const float & p_deltaTime )
		{
			// Rotation.
			if ( _mouseLeftPressed )
			{
				_camera.rotate( Vec3f( -VTX_SETTING().rotationSpeed * _deltaMousePosition.y * ( VTX_SETTING().yAxisInverted ? -1.f : 1.f ),
									   -VTX_SETTING().rotationSpeed * _deltaMousePosition.x,

									   0.f ) );
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
			if ( _mouseRightPressed )
			{
				_camera.rotateRoll( VTX_SETTING().rotationSpeed * _deltaMousePosition.x );
				_deltaMousePosition.x = 0;
			}

			// Translation.
			Vec3f translation = VEC3F_ZERO;

			if ( _isKeyPressed( Qt::Key_Z ) || _isKeyPressed( Qt::Key_Up ) )
			{
				translation.z++;
			}
			if ( _isKeyPressed( Qt::Key_S ) || _isKeyPressed( Qt::Key_Down ) )
			{
				translation.z--;
			}
			if ( _isKeyPressed( Qt::Key_Q ) || _isKeyPressed( Qt::Key_Left ) )
			{
				translation.x++;
			}
			if ( _isKeyPressed( Qt::Key_D ) || _isKeyPressed( Qt::Key_Right ) )
			{
				translation.x--;
			}
			if ( _isKeyPressed( Qt::Key_R ) )
			{
				translation.y++;
			}
			if ( _isKeyPressed( Qt::Key_F ) )
			{
				translation.y--;
			}

			if ( translation == VEC3F_ZERO )
			{
				return;
			}

			translation *= VTX_SETTING().translationSpeed;
			translation *= p_deltaTime;

			if ( _isKeyPressed( Qt::Key_Shift ) )
			{
				translation *= VTX_SETTING().translationFactorSpeed;
			}
			if ( _isKeyPressed( Qt::Key_Control ) )
			{
				translation /= VTX_SETTING().translationFactorSpeed;
			}

			_camera.move( translation );
		}

		void Freefly::reset()
		{
			const Vec3f defaultPos = VTXApp::get().getScene().getAABB().centroid() + VEC3F_Z * VTXApp::get().getScene().getAABB().diameter();

			_camera.setPosition( defaultPos );
			_camera.setRotation( Vec3f( 0.f, 0.f, 0.f ) );
		}

		void Freefly::_updateOrient( const float & p_time )
		{
			// Compute target position.
			Vec3f direction = _orientStartCamera.getPosition() - _orientTargetAabb.centroid();
			if ( direction == VEC3F_ZERO )
			{
				// TODO: finish if current position = target position.
				VTX_DEBUG( "direction == VEC3F_ZERO" );
			}

			Util::Math::normalizeSelf( direction );

			// const Vec3f targetPosition = _orientTargetAabb.centroid() + ( direction * _orientTargetAabb.diagonal().length() );
			const float distanceToBBox = _orientTargetAabb.diameter() / ( 2.f * tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			const Vec3f targetPosition = _orientTargetAabb.centroid() + direction * distanceToBBox;

			// Move.
			direction			 = targetPosition - _orientStartCamera.getPosition();
			const float distance = Util::Math::length( direction );
			Util::Math::normalizeSelf( direction );

			Vec3f position = _orientStartCamera.getPosition();
			position += direction * Util::Math::easeInOutInterpolation( 0.f, distance, p_time );
			_camera.setPosition( position );

			// Rotation.
			// Vec3f eulerFrom	 = Util::Math::quaternionToEuler( _orientStartCamera.getRotation() );
			// Vec3f eulerTo	 = Util::Math::directionToEuler( -direction );
			// Quatf rotationTo  = Util::Math::eulerToQuaternion( eulerTo );

			Vec3f up = _orientStartCamera.getUp() + direction - _orientStartCamera.getFront();
			// VTX_DEBUG( Util::Math::to_string( _orientStartCamera.getUp() ) );
			up				 = Util::Math::normalize( up );
			Quatf rotationTo = Util::Math::lookAt( targetPosition, _orientTargetAabb.centroid(), up );
			Quatf rotation	 = Util::Math::easeInOutInterpolation( _orientStartCamera.getRotation(), rotationTo, p_time );
			_camera.setRotation( rotation );
		}

	} // namespace Controller
} // namespace VTX
