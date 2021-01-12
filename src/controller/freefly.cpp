#include "freefly.hpp"

namespace VTX
{
	namespace Controller
	{
		void Freefly::_updateInputs( const double & p_deltaTime )
		{
			// Rotation.
			if ( _mouseLeftPressed )
			{
				_camera.rotate( Vec3d( -VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.y * ( VTX_SETTING().yAxisInverted ? -1.f : 1.f ),
									   -VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.x,

									   0.f ) );
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
			if ( _mouseRightPressed )
			{
				_camera.rotateRoll( VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.x );
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
			const Vec3d defaultPos = VTXApp::get().getScene().getAABB().centroid() + VEC3F_Z * VTXApp::get().getScene().getAABB().diameter();

			_camera.setPosition( defaultPos );
			_camera.setRotation( Vec3d( 0.0, 0.0, 0.0 ) );
		}

		void Freefly::_updateOrient( const double & p_time )
		{
			// Compute target position.
			Vec3f direction = (Vec3f)_orientStartCamera.getPosition() - _orientTargetAabb.centroid();
			if ( direction == VEC3F_ZERO )
			{
				// TODO: finish if current position = target position.
				VTX_DEBUG( "direction == VEC3F_ZERO" );
			}

			Util::Math::normalizeSelf( direction );

			// const Vec3f targetPosition = _orientTargetAabb.centroid() + ( direction * (float)_orientTargetAabb.diagonal().length() );
			const float distanceToBBox = (float)_orientTargetAabb.diameter() / ( 2.f * tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
			const Vec3f targetPosition = _orientTargetAabb.centroid() + direction * distanceToBBox;

			// Move.
			direction			  = targetPosition - (Vec3f)_orientStartCamera.getPosition();
			const double distance = Util::Math::length( direction );
			Util::Math::normalizeSelf( direction );

			Vec3f position = _orientStartCamera.getPosition();
			position += direction * (float)Util::Math::easeInOutInterpolation( 0.0, distance, p_time );
			_camera.setPosition( position );

			// Rotation.
			// Vec3d eulerFrom	 = Util::Math::quaternionToEuler( _orientStartCamera.getRotation() );
			// Vec3d eulerTo	 = Util::Math::directionToEuler( -direction );
			// Quatd rotationTo = Util::Math::eulerToQuaternion( eulerTo );

			Vec3d up = VEC3D_Y;
			Util::Math::normalizeSelf( up );
			Quatd rotationTo = Util::Math::lookAt( _orientStartCamera.getPosition(), (Vec3d)_orientTargetAabb.centroid(), up );
			Quatd rotation	 = Util::Math::easeInOutInterpolation( _orientStartCamera.getRotation(), rotationTo, p_time );
			_camera.setRotation( rotation );
		}

	} // namespace Controller
} // namespace VTX
