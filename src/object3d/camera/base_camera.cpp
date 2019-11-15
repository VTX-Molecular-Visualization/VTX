#include "base_camera.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace VTX
{
	namespace Object3D
	{
		namespace Camera
		{
			Mat4f BaseCamera::getViewMatrix() const { return glm::lookAt( _pos, _pos + _front, _up ); }

			Mat4f BaseCamera::getProjectionMatrix() const
			{
				return glm::perspective( glm::radians( _fov ), _screenWidth / _screenHeight, _near, _far );
			}

			void BaseCamera::setScreenSize( const int p_width, const int p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
			}

			void BaseCamera::_zoom( const float p_delta )
			{
				_fov = glm::clamp( _fov - p_delta, CAMERA_FOV_MIN, CAMERA_FOV_MAX );
			}

			void BaseCamera::_moveFront( const float delta ) { _pos += delta * _front; }

			void BaseCamera::_moveLeft( const float delta ) { _pos += delta * _left; }

			void BaseCamera::_moveUp( const float delta ) { _pos += delta * _up; }

			void BaseCamera::_rotateLeft( const float angle )
			{
				_phi -= glm::radians( angle );
				_update();
			}

			void BaseCamera::_rotateUp( const float angle )
			{
				_theta -= glm::radians( angle );
				_update();
			}

			void BaseCamera::_update()
			{
				const float cosTheta = cosf( _theta );
				_front				 = Vec3f( cosTheta * sinf( _phi ), sinf( _theta ), cosTheta * cosf( _phi ) );
				_left				 = Vec3f( sinf( _phi + PI_2f ), 0.f, cosf( _phi + PI_2f ) );
				_up					 = glm::cross( _front, _left );
			}

		} // namespace Camera
	}	  // namespace Object3D
} // namespace VTX
