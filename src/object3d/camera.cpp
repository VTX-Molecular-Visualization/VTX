#include "camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		void Camera::zoom( const float p_delta )
		{
			_fov = glm::clamp( _fov - p_delta, CAMERA_FOV_MIN, CAMERA_FOV_MAX );
		}

		void Camera::moveFront( const float p_delta ) { _position += p_delta * _front; }

		void Camera::moveLeft( const float p_delta ) { _position += p_delta * _left; }

		void Camera::moveUp( const float p_delta ) { _position += p_delta * _up; }

		void Camera::rotateLeft( const float p_angle )
		{
			_phi -= glm::radians( p_angle );
			_update();
		}

		void Camera::rotateUp( const float p_angle )
		{
			_theta -= glm::radians( p_angle );
			_update();
		}

		void Camera::rotateAroundLeft( const float p_angle, const Vec3f & p_target )
		{
			// TODO.
			_update();
		}

		void Camera::rotateAroundUp( const float p_angle, const Vec3f & p_target )
		{
			// TODO.
			_update();
		}

		void Camera::_update()
		{
			const float cosTheta = cosf( _theta );
			_front				 = Vec3f( cosTheta * sinf( _phi ), sinf( _theta ), cosTheta * cosf( _phi ) );
			_left				 = Vec3f( sinf( _phi + PI_2f ), 0.f, cosf( _phi + PI_2f ) );
			_up					 = glm::cross( _front, _left );
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + glm::to_string( _position ) );
			VTX_INFO( "Front: " + glm::to_string( _front ) );
			VTX_INFO( "Left: " + glm::to_string( _left ) );
			VTX_INFO( "Up: " + glm::to_string( _up ) );
			VTX_INFO( "Phi: " + std::to_string( _phi ) );
			VTX_INFO( "Theta: " + std::to_string( _theta ) );
			VTX_INFO( "Fov: " + std::to_string( _fov ) );
		}

	} // namespace Object3D
} // namespace VTX
