#include "freefly.hpp"

namespace VTX
{
	namespace Controller
	{
		void Freefly::update( const double & p_deltaTime )
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

		void Freefly::focus( const Math::AABB & ) { VTX_DEBUG( "FOCUS" ); }

	} // namespace Controller
} // namespace VTX
